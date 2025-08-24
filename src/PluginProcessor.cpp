#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace BeatCrafter
{

	BeatCrafterProcessor::BeatCrafterProcessor()
		: AudioProcessor(BusesProperties()
			.withOutput("Output", juce::AudioChannelSet::stereo(), true))
	{

		addParameter(intensityParam = new juce::AudioParameterFloat(
			"intensity", "Intensity", 0.0f, 1.0f, 0.5f));
		addParameter(liveJamIntensityParam = new juce::AudioParameterFloat(
			"liveJamIntensity", "Live Jam Intensity", 0.0f, 1.0f, 0.5f));
		addParameter(styleParam = new juce::AudioParameterChoice(
			"style", "Style",
			juce::StringArray{ "Rock", "Metal", "Jazz", "Funk", "Electronic", "HipHop", "Latin", "Punk" },
			0));

		for (int i = 0; i < 8; ++i)
		{
			juce::String paramId = "slotStyle" + juce::String(i);
			juce::String paramName = "Slot " + juce::String(i + 1) + " Style";
			slotStyleParams[i] = new juce::AudioParameterChoice(
				paramId, paramName,
				juce::StringArray{ "Rock", "Metal", "Jazz", "Funk", "Electronic", "HipHop", "Latin", "Punk" },
				0);
			addParameter(slotStyleParams[i]);
		}

		for (int i = 0; i < 8; ++i)
		{
			if (slotStyleParams[i] != nullptr) {
				StyleType paramStyle = static_cast<StyleType>(slotStyleParams[i]->getIndex());
				getPatternEngine().setSlotStyle(i, paramStyle);
			}
		}
	}

	BeatCrafterProcessor::~BeatCrafterProcessor()
	{
	}

	void BeatCrafterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
	{
		currentSampleRate = sampleRate;
	}

	void BeatCrafterProcessor::releaseResources()
	{
	}

	bool BeatCrafterProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
	{
		if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
			return false;

		return true;
	}

	void BeatCrafterProcessor::processBlock(juce::AudioBuffer<float>& buffer,
		juce::MidiBuffer& midiMessages)
	{
		buffer.clear();

		processMidi(midiMessages);

		auto playHead = getPlayHead();
		if (playHead == nullptr)
			return;

		auto posInfo = playHead->getPosition();
		if (!posInfo.hasValue())
			return;

		patternEngine.setIntensity(intensityParam->get());
		patternEngine.setLiveJamIntensity(liveJamIntensityParam->get());

		bool hostIsPlaying = posInfo->getIsPlaying();

		if (hostIsPlaying)
		{
			if (!patternEngine.getIsPlaying())
			{
				patternEngine.start();
				patternEngine.resetToStart();
			}
			patternEngine.processBlock(midiMessages, buffer.getNumSamples(),
				currentSampleRate, *posInfo);
		}
		else
		{
			if (patternEngine.getIsPlaying())
			{
				patternEngine.stop();
			}
		}
	}

	void BeatCrafterProcessor::setLiveJamMode(bool enabled)
	{
		liveJamModeState = enabled;
		patternEngine.setLiveJamMode(enabled);
	}

	void BeatCrafterProcessor::setupDefaultMidiMappings()
	{
		intensityMapping.ccNumber = 7;
		intensityMapping.channel = 0;
		intensityMapping.isNote = false;
		intensityMapping.isProgramChange = false;

		for (int i = 0; i < 4; ++i)
		{
			slotMappings[i].ccNumber = i;
			slotMappings[i].channel = 0;
			slotMappings[i].isNote = false;
			slotMappings[i].isProgramChange = true;
		}

		for (int i = 4; i < 8; ++i)
		{
			slotMappings[i] = MidiMapping{};
		}
	}

	void BeatCrafterProcessor::processMidi(juce::MidiBuffer& midiMessages)
	{
		juce::MidiBuffer processedMidi;

		for (const auto metadata : midiMessages)
		{
			auto message = metadata.getMessage();
			if (message.isController())
			{
				int ccNumber = message.getControllerNumber();
				int channel = message.getChannel() - 1;
				int value = message.getControllerValue();
				if (midiLearnMode)
				{
					learnByType(ccNumber, channel);
				}
				else
				{
					processForCC(ccNumber, channel, value);
				}
			}
			else if (message.isProgramChange())
			{
				int programNumber = message.getProgramChangeNumber();
				int channel = message.getChannel() - 1;

				if (midiLearnMode)
				{
					learnForPC(programNumber, channel);
				}
				else
				{
					processPC(programNumber, channel);
				}
			}
			else if (message.isNoteOn())
			{
				int noteNumber = message.getNoteNumber();
				int channel = message.getChannel() - 1;
				int velocity = message.getVelocity();

				if (midiLearnMode)
				{
					learnForNoteOn(noteNumber, channel);
				}
				else
				{
					processMidiNoteOn(noteNumber, channel, velocity);
				}
			}
			else
			{
				processedMidi.addEvent(message, metadata.samplePosition);
			}
		}

		midiMessages.swapWith(processedMidi);
	}

	void BeatCrafterProcessor::processPC(int programNumber, int channel)
	{
		for (int i = 0; i < 8; ++i)
		{
			if (slotMappings[i].isValid() &&
				slotMappings[i].isProgramChange &&
				programNumber == slotMappings[i].ccNumber &&
				channel == slotMappings[i].channel)
			{
				getPatternEngine().switchToSlot(i, true);
				juce::MessageManager::callAsync([this, i]()
					{
						if (auto* editor = getActiveEditor()) {
							if (auto* customEditor = dynamic_cast<BeatCrafterEditor*>(editor)) {
								customEditor->updateSlotButtons(i);
							}
						} });
						break;
			}
		}
	}

	void BeatCrafterProcessor::processMidiNoteOn(int noteNumber, int channel, int velocity)
	{
		if (intensityMapping.isValid() && intensityMapping.isNote &&
			noteNumber == intensityMapping.ccNumber &&
			channel == intensityMapping.channel)
		{
			float newIntensity = velocity / 127.0f;
			intensityParam->setValueNotifyingHost(newIntensity);
		}
		for (int i = 0; i < 8; ++i)
		{
			if (slotMappings[i].isValid() && slotMappings[i].isNote &&
				noteNumber == slotMappings[i].ccNumber &&
				channel == slotMappings[i].channel)
			{
				getPatternEngine().switchToSlot(i, true);
				juce::MessageManager::callAsync([this, i]()
					{
						if (auto* editor = getActiveEditor()) {
							if (auto* customEditor = dynamic_cast<BeatCrafterEditor*>(editor)) {
								customEditor->updateSlotButtons(i);
							}
						} });
						break;
			}
		}
	}

	void BeatCrafterProcessor::learnForNoteOn(int noteNumber, int channel)
	{
		if (midiLearnTargetType == 0)
		{
			intensityMapping.ccNumber = noteNumber;
			intensityMapping.channel = channel;
			intensityMapping.isNote = true;
			intensityMapping.isProgramChange = false;
			stopMidiLearn();
		}
		else if (midiLearnTargetType == 1)
		{
			if (midiLearnTargetSlot >= 0 && midiLearnTargetSlot < 8)
			{
				slotMappings[midiLearnTargetSlot].ccNumber = noteNumber;
				slotMappings[midiLearnTargetSlot].channel = channel;
				slotMappings[midiLearnTargetSlot].isNote = true;
				slotMappings[midiLearnTargetSlot].isProgramChange = false;
				stopMidiLearn();
			}
			else
			{
				DBG("Invalid slot index in MIDI learn: " + juce::String(midiLearnTargetSlot));
				stopMidiLearn();
			}
		}
		else if (midiLearnTargetType == 2)
		{
			liveJamIntensityMapping.ccNumber = noteNumber;
			liveJamIntensityMapping.channel = channel;
			liveJamIntensityMapping.isNote = true;
			liveJamIntensityMapping.isProgramChange = false;
			stopMidiLearn();
		}
	}

	void BeatCrafterProcessor::learnForPC(int programNumber, int channel)
	{
		if (midiLearnTargetType == 1)
		{
			if (midiLearnTargetSlot >= 0 && midiLearnTargetSlot < 8)
			{
				slotMappings[midiLearnTargetSlot].ccNumber = programNumber;
				slotMappings[midiLearnTargetSlot].channel = channel;
				slotMappings[midiLearnTargetSlot].isNote = false;
				slotMappings[midiLearnTargetSlot].isProgramChange = true;
				stopMidiLearn();
			}
			else
			{
				DBG("Invalid slot index in MIDI learn: " + juce::String(midiLearnTargetSlot));
				stopMidiLearn();
			}
		}
	}

	void BeatCrafterProcessor::processForCC(int ccNumber, int channel, int value)
	{
		if (intensityMapping.isValid() && !intensityMapping.isNote &&
			ccNumber == intensityMapping.ccNumber &&
			channel == intensityMapping.channel)
		{
			float newIntensity = value / 127.0f;
			intensityParam->setValueNotifyingHost(newIntensity);
			juce::MessageManager::callAsync([this, newIntensity]()
				{
					if (auto* editor = getActiveEditor()) {
						if (auto* customEditor = dynamic_cast<BeatCrafterEditor*>(editor)) {
							customEditor->updateIntensitySlider(newIntensity);
						}
					} });
		}
		if (liveJamIntensityMapping.isValid() && !liveJamIntensityMapping.isNote &&
			ccNumber == liveJamIntensityMapping.ccNumber &&
			channel == liveJamIntensityMapping.channel)
		{
			float newLiveJamIntensity = value / 127.0f;
			liveJamIntensityParam->setValueNotifyingHost(newLiveJamIntensity);
			juce::MessageManager::callAsync([this, newLiveJamIntensity]()
				{
					if (auto* editor = getActiveEditor()) {
						if (auto* customEditor = dynamic_cast<BeatCrafterEditor*>(editor)) {
							customEditor->updateLiveJamIntensitySlider(newLiveJamIntensity);
						}
					}
				});
		}
		for (int i = 0; i < 8; ++i)
		{
			if (slotMappings[i].isValid() && !slotMappings[i].isNote &&
				ccNumber == slotMappings[i].ccNumber &&
				channel == slotMappings[i].channel)
			{
				if (value >= 64)
				{
					getPatternEngine().switchToSlot(i, true);
					juce::MessageManager::callAsync([this, i]()
						{
							if (auto* editor = getActiveEditor()) {
								if (auto* customEditor = dynamic_cast<BeatCrafterEditor*>(editor)) {
									customEditor->updateSlotButtons(i);
								}
							} });
				}
				break;
			}
		}
	}

	void BeatCrafterProcessor::learnByType(int ccNumber, int channel)
	{
		if (midiLearnTargetType == 0)
		{
			intensityMapping.ccNumber = ccNumber;
			intensityMapping.channel = channel;
			intensityMapping.isNote = false;
			intensityMapping.isProgramChange = false;
			stopMidiLearn();
		}
		else if (midiLearnTargetType == 1)
		{
			if (midiLearnTargetSlot >= 0 && midiLearnTargetSlot < 8)
			{
				slotMappings[midiLearnTargetSlot].ccNumber = ccNumber;
				slotMappings[midiLearnTargetSlot].channel = channel;
				slotMappings[midiLearnTargetSlot].isNote = false;
				slotMappings[midiLearnTargetSlot].isProgramChange = false;
				stopMidiLearn();
			}
			else
			{
				DBG("Invalid slot index in MIDI learn: " + juce::String(midiLearnTargetSlot));
				stopMidiLearn();
			}
		}
		else if (midiLearnTargetType == 2)
		{
			liveJamIntensityMapping.ccNumber = ccNumber;
			liveJamIntensityMapping.channel = channel;
			liveJamIntensityMapping.isNote = false;
			liveJamIntensityMapping.isProgramChange = false;
			stopMidiLearn();
		}
	}

	juce::AudioProcessorEditor* BeatCrafterProcessor::createEditor()
	{
		return new BeatCrafterEditor(*this);
	}

	void BeatCrafterProcessor::updateEditorFromState()
	{
		juce::Timer::callAfterDelay(100, [this]()
			{
				if (auto* editor = dynamic_cast<BeatCrafterEditor*>(getActiveEditor())) {
					editor->updateFromProcessorState();
				} });
	}

	void BeatCrafterProcessor::getStateInformation(juce::MemoryBlock& destData)
	{
		auto state = juce::ValueTree("BeatCrafterState");

		state.setProperty("intensity", intensityParam->get(), nullptr);
		state.setProperty("style", styleParam->getIndex(), nullptr);
		state.setProperty("activeSlot", getPatternEngine().getActiveSlot(), nullptr);
		state.setProperty("liveJamMode", liveJamModeState, nullptr);
		state.setProperty("liveJamIntensity", liveJamIntensityParam->get(), nullptr);

		for (int i = 0; i < 8; ++i)
		{
			juce::String styleProps = "slotStyle" + juce::String(i);
			juce::String seedProps = "slotSeed" + juce::String(i);

			state.setProperty(styleProps, slotStyleParams[i]->getIndex(), nullptr);
			state.setProperty(seedProps, (int)getPatternEngine().getSlotSeed(i), nullptr);
		}

		for (int slotIndex = 0; slotIndex < 8; ++slotIndex)
		{
			Pattern* pattern = getPatternEngine().getSlot(slotIndex);
			if (pattern)
			{
				auto slotNode = juce::ValueTree("Slot" + juce::String(slotIndex));

				slotNode.setProperty("name", pattern->getName(), nullptr);
				slotNode.setProperty("swing", pattern->getSwing(), nullptr);
				slotNode.setProperty("length", pattern->getLength(), nullptr);

				auto ts = pattern->getTimeSignature();
				slotNode.setProperty("timeSignatureNum", ts.numerator, nullptr);
				slotNode.setProperty("timeSignatureDenom", ts.denominator, nullptr);

				for (int trackIndex = 0; trackIndex < pattern->getNumTracks(); ++trackIndex)
				{
					auto trackNode = juce::ValueTree("Track" + juce::String(trackIndex));
					const auto& track = pattern->getTrack(trackIndex);

					trackNode.setProperty("name", track.getName(), nullptr);
					trackNode.setProperty("midiNote", track.getMidiNote(), nullptr);

					for (int stepIndex = 0; stepIndex < track.getLength(); ++stepIndex)
					{
						const auto& step = track.getStep(stepIndex);

						if (step.isActive())
						{
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

		if (intensityMapping.isValid())
		{
			state.setProperty("intensityMidiCC", intensityMapping.ccNumber, nullptr);
			state.setProperty("intensityMidiChannel", intensityMapping.channel, nullptr);
			state.setProperty("intensityMidiIsNote", intensityMapping.isNote, nullptr);
		}

		for (int i = 0; i < 8; ++i)
		{
			if (slotMappings[i].isValid())
			{
				juce::String ccProp = "slot" + juce::String(i) + "MidiCC";
				juce::String channelProp = "slot" + juce::String(i) + "MidiChannel";
				juce::String isNoteProp = "slot" + juce::String(i) + "MidiIsNote";
				juce::String isProgramChangeProp = "slot" + juce::String(i) + "MidiIsProgramChange";

				state.setProperty(ccProp, slotMappings[i].ccNumber, nullptr);
				state.setProperty(channelProp, slotMappings[i].channel, nullptr);
				state.setProperty(isNoteProp, slotMappings[i].isNote, nullptr);
				state.setProperty(isProgramChangeProp, slotMappings[i].isProgramChange, nullptr);
			}
		}

		juce::MemoryOutputStream stream(destData, false);
		state.writeToStream(stream);
	}

	void BeatCrafterProcessor::setStateInformation(const void* data, int sizeInBytes)
	{
		auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
		if (tree.isValid())
		{
			intensityParam->setValueNotifyingHost(tree.getProperty("intensity", 0.5f));
			int styleIndex = tree.getProperty("style", 0);
			float normalizedStyle = styleIndex / (float)(styleParam->choices.size() - 1);
			styleParam->setValueNotifyingHost(normalizedStyle);
			liveJamModeState = tree.getProperty("liveJamMode", false);
			patternEngine.setLiveJamMode(liveJamModeState);
			liveJamIntensityParam->setValueNotifyingHost(tree.getProperty("liveJamIntensity", 0.5f));

			for (int i = 0; i < 8; ++i)
			{
				juce::String styleProps = "slotStyle" + juce::String(i);
				juce::String seedProps = "slotSeed" + juce::String(i);

				int styleIndex = tree.getProperty(styleProps, 0);
				uint32_t seed = static_cast<uint32_t>(static_cast<int>(tree.getProperty(seedProps, 0)));

				float normalizedValue = styleIndex / (float)(slotStyleParams[i]->choices.size() - 1);
				slotStyleParams[i]->setValueNotifyingHost(normalizedValue);

				getPatternEngine().setSlotStyle(i, static_cast<StyleType>(styleIndex));
				getPatternEngine().setSlotSeed(i, seed);
			}

			for (int slotIndex = 0; slotIndex < 8; ++slotIndex)
			{
				auto slotNode = tree.getChildWithName("Slot" + juce::String(slotIndex));
				if (slotNode.isValid())
				{
					auto restoredPattern = std::make_unique<Pattern>(
						slotNode.getProperty("name", "Pattern " + juce::String(slotIndex + 1)));

					restoredPattern->setSwing(slotNode.getProperty("swing", 0.0f));
					int patternLength = slotNode.getProperty("length", 16);
					restoredPattern->setLength(patternLength);

					TimeSignature ts;
					ts.numerator = slotNode.getProperty("timeSignatureNum", 4);
					ts.denominator = slotNode.getProperty("timeSignatureDenom", 4);
					restoredPattern->setTimeSignature(ts);

					for (int trackIndex = 0; trackIndex < restoredPattern->getNumTracks(); ++trackIndex)
					{
						auto trackNode = slotNode.getChildWithName("Track" + juce::String(trackIndex));
						if (trackNode.isValid())
						{
							auto& track = restoredPattern->getTrack(trackIndex);

							track.setName(trackNode.getProperty("name", track.getName()));
							track.setMidiNote(trackNode.getProperty("midiNote", track.getMidiNote()));

							track.clear();

							for (int childIndex = 0; childIndex < trackNode.getNumChildren(); ++childIndex)
							{
								auto stepNode = trackNode.getChild(childIndex);
								juce::String stepName = stepNode.getType().toString();

								if (stepName.startsWith("Step"))
								{
									int stepIndex = stepName.substring(4).getIntValue();
									if (stepIndex >= 0 && stepIndex < track.getLength())
									{
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
			bool hasSavedMidiMappings = false;
			if (tree.hasProperty("intensityMidiCC"))
			{
				intensityMapping.ccNumber = tree.getProperty("intensityMidiCC", -1);
				intensityMapping.channel = tree.getProperty("intensityMidiChannel", -1);
				intensityMapping.isNote = tree.getProperty("intensityMidiIsNote", false);
				hasSavedMidiMappings = true;
			}

			for (int i = 0; i < 8; ++i)
			{
				juce::String ccProp = "slot" + juce::String(i) + "MidiCC";
				juce::String channelProp = "slot" + juce::String(i) + "MidiChannel";
				juce::String isNoteProp = "slot" + juce::String(i) + "MidiIsNote";
				juce::String isProgramChangeProp = "slot" + juce::String(i) + "MidiIsProgramChange";

				if (tree.hasProperty(ccProp))
				{
					slotMappings[i].ccNumber = tree.getProperty(ccProp, -1);
					slotMappings[i].channel = tree.getProperty(channelProp, -1);
					slotMappings[i].isNote = tree.getProperty(isNoteProp, false);
					slotMappings[i].isProgramChange = tree.getProperty(isProgramChangeProp, false);
					hasSavedMidiMappings = true;
				}
			}
			if (!hasSavedMidiMappings)
			{
				setupDefaultMidiMappings();
			}
			int activeSlot = tree.getProperty("activeSlot", 0);
			getPatternEngine().switchToSlot(activeSlot, true);

			updateEditorFromState();
		}
		else
		{
			setupDefaultMidiMappings();
		}
	}

	void BeatCrafterProcessor::startMidiLearn(int targetType, int targetSlot)
	{
		midiLearnMode = true;
		midiLearnTargetType = targetType;
		midiLearnTargetSlot = targetSlot;
	}

	void BeatCrafterProcessor::stopMidiLearn()
	{
		midiLearnMode = false;
		midiLearnTargetType = -1;
		midiLearnTargetSlot = -1;
	}

	void BeatCrafterProcessor::clearMidiMapping(int targetType, int targetSlot)
	{
		if (targetType == 0)
		{
			intensityMapping = MidiMapping{};
		}
		else if (targetType >= 1 && targetType <= 8 && targetSlot >= 0 && targetSlot < 8)
		{
			slotMappings[targetSlot] = MidiMapping{};
		}
	}

	bool BeatCrafterProcessor::hasMidiMapping(int targetType, int targetSlot) const
	{
		if (targetType == 0)
		{
			return intensityMapping.isValid();
		}
		else if (targetType >= 1 && targetType <= 8 && targetSlot >= 0 && targetSlot < 8)
		{
			return slotMappings[targetSlot].isValid();
		}
		return false;
	}

	juce::String BeatCrafterProcessor::getMidiMappingDescription(int targetType, int targetSlot) const
	{
		MidiMapping mapping;
		if (targetType == 0)
		{
			mapping = intensityMapping;
		}
		else if (targetType >= 1 && targetType <= 8 && targetSlot >= 0 && targetSlot < 8)
		{
			mapping = slotMappings[targetSlot];
		}

		if (mapping.isValid())
		{
			if (mapping.isProgramChange)
			{
				return "CH" + juce::String(mapping.channel + 1) + " PC" + juce::String(mapping.ccNumber);
			}
			else if (mapping.isNote)
			{
				return "CH" + juce::String(mapping.channel + 1) + " Note" + juce::String(mapping.ccNumber);
			}
			else
			{
				return "CH" + juce::String(mapping.channel + 1) + " CC" + juce::String(mapping.ccNumber);
			}
		}
		return "Not mapped";
	}
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new BeatCrafter::BeatCrafterProcessor();
}