#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace BeatCrafter {

	BeatCrafterProcessor::BeatCrafterProcessor()
		: AudioProcessor(BusesProperties()
			.withOutput("Output", juce::AudioChannelSet::stereo(), true)) {

		addParameter(intensityParam = new juce::AudioParameterFloat(
			"intensity", "Intensity", 0.0f, 1.0f, 0.5f));

		addParameter(styleParam = new juce::AudioParameterChoice(
			"style", "Style",
			juce::StringArray{ "Rock", "Metal", "Jazz", "Funk", "Electronic", "HipHop", "Latin", "Punk" },
			0));

		for (int i = 0; i < 8; ++i) {
			juce::String paramId = "slotStyle" + juce::String(i);
			juce::String paramName = "Slot " + juce::String(i + 1) + " Style";

			slotStyleParams[i] = new juce::AudioParameterChoice(
				paramId, paramName,
				juce::StringArray{ "Rock", "Metal", "Jazz", "Funk", "Electronic", "HipHop", "Latin", "Punk" },
				0);
			addParameter(slotStyleParams[i]);
		}
	}

	BeatCrafterProcessor::~BeatCrafterProcessor() {
	}

	void BeatCrafterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
		currentSampleRate = sampleRate;
	}

	void BeatCrafterProcessor::releaseResources() {
	}

	bool BeatCrafterProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
		if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
			return false;

		return true;
	}

	void BeatCrafterProcessor::processBlock(juce::AudioBuffer<float>& buffer,
		juce::MidiBuffer& midiMessages) {
		buffer.clear();

		juce::MidiBuffer processedMidi;

		for (const auto metadata : midiMessages) {
			auto message = metadata.getMessage();

			if (message.isController()) {
				int ccNumber = message.getControllerNumber();
				int channel = message.getChannel() - 1;
				int value = message.getControllerValue();

				if (midiLearnMode) {
					if (midiLearnTargetType == 0) {
						intensityMapping.ccNumber = ccNumber;
						intensityMapping.channel = channel;
						stopMidiLearn();
					}
					else if (midiLearnTargetType >= 1 && midiLearnTargetType <= 8) {
						slotMappings[midiLearnTargetSlot].ccNumber = ccNumber;
						slotMappings[midiLearnTargetSlot].channel = channel;
						stopMidiLearn();
					}
				}
				else {
					if (intensityMapping.isValid() &&
						ccNumber == intensityMapping.ccNumber &&
						channel == intensityMapping.channel) {
						float newIntensity = value / 127.0f;
						intensityParam->setValueNotifyingHost(newIntensity);
					}

					for (int i = 0; i < 8; ++i) {
						if (slotMappings[i].isValid() &&
							ccNumber == slotMappings[i].ccNumber &&
							channel == slotMappings[i].channel) {
							if (value > 63) {
								getPatternEngine().switchToSlot(i, false);
							}
							break;
						}
					}
				}
			}
			else {
				processedMidi.addEvent(message, metadata.samplePosition);
			}
		}

		midiMessages.swapWith(processedMidi);

		auto playHead = getPlayHead();
		if (playHead == nullptr) return;

		auto posInfo = playHead->getPosition();
		if (!posInfo.hasValue()) return;

		patternEngine.setIntensity(intensityParam->get());

		bool hostIsPlaying = posInfo->getIsPlaying();

		if (hostIsPlaying) {
			if (!patternEngine.getIsPlaying()) {
				patternEngine.start();
			}
			patternEngine.processBlock(midiMessages, buffer.getNumSamples(),
				currentSampleRate, *posInfo);
		}
		else {
			if (patternEngine.getIsPlaying()) {
				patternEngine.stop();
			}
		}
	}

	juce::AudioProcessorEditor* BeatCrafterProcessor::createEditor() {
		return new BeatCrafterEditor(*this);
	}

	void BeatCrafterProcessor::getStateInformation(juce::MemoryBlock& destData) {
		auto state = juce::ValueTree("BeatCrafterState");

		state.setProperty("intensity", intensityParam->get(), nullptr);
		state.setProperty("style", styleParam->getIndex(), nullptr);
		state.setProperty("activeSlot", getPatternEngine().getActiveSlot(), nullptr);

		for (int i = 0; i < 8; ++i) {
			juce::String styleProps = "slotStyle" + juce::String(i);
			juce::String seedProps = "slotSeed" + juce::String(i);

			state.setProperty(styleProps, slotStyleParams[i]->getIndex(), nullptr);
			state.setProperty(seedProps, (int)getPatternEngine().getSlotSeed(i), nullptr);
		}

		for (int slotIndex = 0; slotIndex < 8; ++slotIndex) {
			Pattern* pattern = getPatternEngine().getSlot(slotIndex);
			if (pattern) {
				auto slotNode = juce::ValueTree("Slot" + juce::String(slotIndex));

				slotNode.setProperty("name", pattern->getName(), nullptr);
				slotNode.setProperty("swing", pattern->getSwing(), nullptr);
				slotNode.setProperty("length", pattern->getLength(), nullptr);

				auto ts = pattern->getTimeSignature();
				slotNode.setProperty("timeSignatureNum", ts.numerator, nullptr);
				slotNode.setProperty("timeSignatureDenom", ts.denominator, nullptr);

				for (int trackIndex = 0; trackIndex < pattern->getNumTracks(); ++trackIndex) {
					auto trackNode = juce::ValueTree("Track" + juce::String(trackIndex));
					const auto& track = pattern->getTrack(trackIndex);

					trackNode.setProperty("name", track.getName(), nullptr);
					trackNode.setProperty("midiNote", track.getMidiNote(), nullptr);

					for (int stepIndex = 0; stepIndex < track.getLength(); ++stepIndex) {
						const auto& step = track.getStep(stepIndex);

						if (step.isActive()) {
							auto stepNode = juce::ValueTree("Step" + juce::String(stepIndex));
							stepNode.setProperty("active", step.isActive(), nullptr);
							stepNode.setProperty("velocity", step.getVelocity(), nullptr);
							stepNode.setProperty("microTiming", step.getMicroTiming(), nullptr);
							stepNode.setProperty("probability", step.getProbability(), nullptr);

							trackNode.addChild(stepNode, -1, nullptr);
						}
					}

					slotNode.addChild(trackNode, -1, nullptr);
				}

				state.addChild(slotNode, -1, nullptr);
			}
		}

		if (intensityMapping.isValid()) {
			state.setProperty("intensityMidiCC", intensityMapping.ccNumber, nullptr);
			state.setProperty("intensityMidiChannel", intensityMapping.channel, nullptr);
		}

		for (int i = 0; i < 8; ++i) {
			if (slotMappings[i].isValid()) {
				juce::String ccProp = "slot" + juce::String(i) + "MidiCC";
				juce::String channelProp = "slot" + juce::String(i) + "MidiChannel";
				state.setProperty(ccProp, slotMappings[i].ccNumber, nullptr);
				state.setProperty(channelProp, slotMappings[i].channel, nullptr);
			}
		}

		juce::MemoryOutputStream stream(destData, false);
		state.writeToStream(stream);
	}

	void BeatCrafterProcessor::setStateInformation(const void* data, int sizeInBytes) {
		auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
		if (tree.isValid()) {
			intensityParam->setValueNotifyingHost(tree.getProperty("intensity", 0.5f));
			styleParam->setValueNotifyingHost(tree.getProperty("style", 0));

			for (int i = 0; i < 8; ++i) {
				juce::String styleProps = "slotStyle" + juce::String(i);
				juce::String seedProps = "slotSeed" + juce::String(i);

				int styleIndex = tree.getProperty(styleProps, 0);
				uint32_t seed = static_cast<uint32_t>(static_cast<int>(tree.getProperty(seedProps, 0)));

				slotStyleParams[i]->setValueNotifyingHost(styleIndex);
				getPatternEngine().setSlotStyle(i, static_cast<StyleType>(styleIndex));
				getPatternEngine().setSlotSeed(i, seed);
			}

			for (int slotIndex = 0; slotIndex < 8; ++slotIndex) {
				auto slotNode = tree.getChildWithName("Slot" + juce::String(slotIndex));
				if (slotNode.isValid()) {
					auto restoredPattern = std::make_unique<Pattern>(
						slotNode.getProperty("name", "Pattern " + juce::String(slotIndex + 1))
					);

					restoredPattern->setSwing(slotNode.getProperty("swing", 0.0f));
					int patternLength = slotNode.getProperty("length", 16);
					restoredPattern->setLength(patternLength);

					TimeSignature ts;
					ts.numerator = slotNode.getProperty("timeSignatureNum", 4);
					ts.denominator = slotNode.getProperty("timeSignatureDenom", 4);
					restoredPattern->setTimeSignature(ts);

					for (int trackIndex = 0; trackIndex < restoredPattern->getNumTracks(); ++trackIndex) {
						auto trackNode = slotNode.getChildWithName("Track" + juce::String(trackIndex));
						if (trackNode.isValid()) {
							auto& track = restoredPattern->getTrack(trackIndex);

							track.setName(trackNode.getProperty("name", track.getName()));
							track.setMidiNote(trackNode.getProperty("midiNote", track.getMidiNote()));

							track.clear();

							for (int childIndex = 0; childIndex < trackNode.getNumChildren(); ++childIndex) {
								auto stepNode = trackNode.getChild(childIndex);
								juce::String stepName = stepNode.getType().toString();

								if (stepName.startsWith("Step")) {
									int stepIndex = stepName.substring(4).getIntValue();
									if (stepIndex >= 0 && stepIndex < track.getLength()) {
										auto& step = track.getStep(stepIndex);

										step.setActive(stepNode.getProperty("active", false));
										step.setVelocity(stepNode.getProperty("velocity", 0.8f));
										step.setMicroTiming(stepNode.getProperty("microTiming", 0.0f));
										step.setProbability(stepNode.getProperty("probability", 1.0f));
									}
								}
							}
						}
					}
					getPatternEngine().loadPatternToSlot(std::move(restoredPattern), slotIndex);
				}


			}

			if (tree.hasProperty("intensityMidiCC")) {
				intensityMapping.ccNumber = tree.getProperty("intensityMidiCC", -1);
				intensityMapping.channel = tree.getProperty("intensityMidiChannel", -1);
			}

			for (int i = 0; i < 8; ++i) {
				juce::String ccProp = "slot" + juce::String(i) + "MidiCC";
				juce::String channelProp = "slot" + juce::String(i) + "MidiChannel";

				if (tree.hasProperty(ccProp)) {
					slotMappings[i].ccNumber = tree.getProperty(ccProp, -1);
					slotMappings[i].channel = tree.getProperty(channelProp, -1);
				}
			}
			int activeSlot = tree.getProperty("activeSlot", 0);
			getPatternEngine().switchToSlot(activeSlot, true);
		}
	}

	void BeatCrafterProcessor::startMidiLearn(int targetType, int targetSlot) {
		midiLearnMode = true;
		midiLearnTargetType = targetType;
		midiLearnTargetSlot = targetSlot;
	}

	void BeatCrafterProcessor::stopMidiLearn() {
		midiLearnMode = false;
		midiLearnTargetType = -1;
		midiLearnTargetSlot = -1;
	}

	void BeatCrafterProcessor::clearMidiMapping(int targetType, int targetSlot) {
		if (targetType == 0) {
			intensityMapping = MidiMapping{};
		}
		else if (targetType >= 1 && targetType <= 8 && targetSlot >= 0 && targetSlot < 8) {
			slotMappings[targetSlot] = MidiMapping{};
		}
	}

	bool BeatCrafterProcessor::hasMidiMapping(int targetType, int targetSlot) const {
		if (targetType == 0) {
			return intensityMapping.isValid();
		}
		else if (targetType >= 1 && targetType <= 8 && targetSlot >= 0 && targetSlot < 8) {
			return slotMappings[targetSlot].isValid();
		}
		return false;
	}

	juce::String BeatCrafterProcessor::getMidiMappingDescription(int targetType, int targetSlot) const {
		MidiMapping mapping;
		if (targetType == 0) {
			mapping = intensityMapping;
		}
		else if (targetType >= 1 && targetType <= 8 && targetSlot >= 0 && targetSlot < 8) {
			mapping = slotMappings[targetSlot];
		}

		if (mapping.isValid()) {
			return "CH" + juce::String(mapping.channel + 1) + " CC" + juce::String(mapping.ccNumber);
		}
		return "Not mapped";
	}
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new BeatCrafter::BeatCrafterProcessor();
}