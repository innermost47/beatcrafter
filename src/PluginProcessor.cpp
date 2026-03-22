#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace BeatCrafter
{

	BeatCrafterProcessor::BeatCrafterProcessor()
		: AudioProcessor(BusesProperties()
							 .withOutput("Output", juce::AudioChannelSet::stereo(), true))
	{
		setupDefaultMidiMappings();
		addParameter(intensityParam = new juce::AudioParameterFloat(
						 "intensity", "Intensity", 0.0f, 1.0f, 0.5f));
		addParameter(liveJamIntensityParam = new juce::AudioParameterFloat(
						 "liveJamIntensity", "Live Jam Intensity", 0.0f, 1.0f, 0.5f));
		addParameter(surpriseMeParam = new juce::AudioParameterBool(
						 "surpriseMe", "Surprise Me", false));
		addParameter(tripletModeParam = new juce::AudioParameterBool(
						 "tripletMode", "Triplet Mode", false));

		patternEngine.setLiveJamMode(true);
		liveJamModeState = true;
		patternEngine.onIntensityChanged = [this](float newIntensity)
		{
			intensityParam->setValueNotifyingHost(newIntensity);
			juce::MessageManager::callAsync([this, newIntensity]()
											{
						if (auto* editor = dynamic_cast<BeatCrafterEditor*>(getActiveEditor()))
							editor->updateIntensitySlider(newIntensity); });
		};
	}

	BeatCrafterProcessor::~BeatCrafterProcessor()
	{
	}

	void BeatCrafterProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
	{
		currentSampleRate = sampleRate;
	}

	void BeatCrafterProcessor::releaseResources()
	{
	}

	bool BeatCrafterProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
	{
		if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
			return false;

		return true;
	}

	void BeatCrafterProcessor::processBlock(juce::AudioBuffer<float> &buffer,
											juce::MidiBuffer &midiMessages)
	{
		buffer.clear();

		processMidi(midiMessages);

		auto playHeadLocal = getPlayHead();
		if (playHeadLocal == nullptr)
			return;

		auto posInfo = playHeadLocal->getPosition();
		if (!posInfo.hasValue())
			return;

		patternEngine.setIntensity(intensityParam->get());
		patternEngine.setLiveJamIntensity(liveJamIntensityParam->get());
		patternEngine.perfParams.surpriseMeEnabled =
			surpriseMeParam->get();
		patternEngine.perfParams.tripletMode = tripletModeParam->get();

		bool hostIsPlaying = posInfo->getIsPlaying();

		if (hostIsPlaying)
		{
			if (!patternEngine.getIsPlaying())
			{
				patternEngine.start();
				patternEngine.resetToStart();
			}
		}
		else
		{
			if (patternEngine.getIsPlaying())
			{
				patternEngine.stop();
			}
		}
		patternEngine.processBlock(midiMessages, buffer.getNumSamples(),
								   currentSampleRate, *posInfo);
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

	void BeatCrafterProcessor::processMidi(juce::MidiBuffer &midiMessages)
	{
		juce::MidiBuffer processedMidi;
		bool slotJustSwitched = false;

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
					if (slotJustSwitched &&
						intensityMapping.isValid() && !intensityMapping.isNote &&
						ccNumber == intensityMapping.ccNumber &&
						channel == intensityMapping.channel)
						continue;
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
					slotJustSwitched = true;
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
					slotJustSwitched = true;
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
				if (!getPatternEngine().getSlot(i))
				{
					StyleType style = getPatternEngine().getSlotStyle(i);
					getPatternEngine().generateNewPatternForSlot(
						i, style, intensityParam->get());
				}

				getPatternEngine().switchToSlot(i, true, intensityParam->get());
				juce::MessageManager::callAsync([this, i]()
												{
						if (auto* editor = getActiveEditor())
							if (auto* ce = dynamic_cast<BeatCrafterEditor*>(editor))
								ce->updateSlotButtons(i); });
				break;
			}
		}
	}

	void BeatCrafterProcessor::processMidiNoteOn(int noteNumber, int channel, int /*velocity*/)
	{
		for (int i = 0; i < 8; ++i)
		{
			if (slotMappings[i].isValid() && slotMappings[i].isNote &&
				noteNumber == slotMappings[i].ccNumber &&
				channel == slotMappings[i].channel)
			{
				if (!getPatternEngine().getSlot(i))
				{
					StyleType style = getPatternEngine().getSlotStyle(i);
					getPatternEngine().generateNewPatternForSlot(
						i, style, intensityParam->get());
				}
				float dbgIntensity = intensityParam->get();
				getPatternEngine().setIntensity(dbgIntensity);
				getPatternEngine().switchToSlot(i, true, dbgIntensity);
				juce::MessageManager::callAsync([this, i]()
												{
						if (auto* editor = getActiveEditor())
							if (auto* ce = dynamic_cast<BeatCrafterEditor*>(editor))
								ce->updateSlotButtons(i); });
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
		if ((ccNumber == 0 || ccNumber == 32) && value == 0)
			return;
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
					} });
		}
		if (surpriseMeMapping.isValid() && !surpriseMeMapping.isNote &&
			ccNumber == surpriseMeMapping.ccNumber &&
			channel == surpriseMeMapping.channel)
		{
			bool active = value >= 64;
			surpriseMeParam->setValueNotifyingHost(active ? 1.0f : 0.0f);
			getPatternEngine().perfParams.surpriseMeEnabled = active;
		}
		if (tripletModeMapping.isValid() && !tripletModeMapping.isNote &&
			ccNumber == tripletModeMapping.ccNumber &&
			channel == tripletModeMapping.channel)
		{
			bool active = value >= 64;
			tripletModeParam->setValueNotifyingHost(active ? 1.0f : 0.0f);
			getPatternEngine().perfParams.tripletMode = active;
		}
		for (int i = 0; i < 8; ++i)
		{
			if (slotMappings[i].isValid() && !slotMappings[i].isNote &&
				ccNumber == slotMappings[i].ccNumber &&
				channel == slotMappings[i].channel)
			{
				if (value >= 64)
				{
					getPatternEngine().setIntensity(intensityParam->get());
					getPatternEngine().switchToSlot(i, true, intensityParam->get());
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

	juce::AudioProcessorEditor *BeatCrafterProcessor::createEditor()
	{
		auto *editor = new BeatCrafterEditor(*this);
		return editor;
	}

	void BeatCrafterProcessor::updateEditorFromState()
	{
		juce::Timer::callAfterDelay(100, [this]()
									{
				if (auto* editor = dynamic_cast<BeatCrafterEditor*>(getActiveEditor())) {
					editor->updateFromProcessorState();
				} });
	}

	void BeatCrafterProcessor::getStateInformation(juce::MemoryBlock &destData)
	{
		auto state = juce::ValueTree("BeatCrafterState");

		state.setProperty("intensity", intensityParam->get(), nullptr);
		state.setProperty("activeSlot", getPatternEngine().getActiveSlot(), nullptr);
		state.setProperty("liveJamMode", liveJamModeState, nullptr);
		state.setProperty("liveJamIntensity", liveJamIntensityParam->get(), nullptr);
		state.setProperty("surpriseMe", surpriseMeParam->get(), nullptr);
		state.setProperty("tripletMode", tripletModeParam->get(), nullptr);

		for (int i = 0; i < 8; ++i)
		{
			juce::String styleProps = "slotStyle" + juce::String(i);
			juce::String seedProps = "slotSeed" + juce::String(i);
			state.setProperty(seedProps, (int)getPatternEngine().getSlotSeed(i), nullptr);
		}

		for (int slotIndex = 0; slotIndex < 8; ++slotIndex)
		{
			Pattern *pattern = getPatternEngine().getSlot(slotIndex);
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
					const auto &track = pattern->getTrack(trackIndex);

					trackNode.setProperty("name", track.getName(), nullptr);
					trackNode.setProperty("midiNote", track.getMidiNote(), nullptr);

					for (int stepIndex = 0; stepIndex < track.getLength(); ++stepIndex)
					{
						const auto &step = track.getStep(stepIndex);

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

		if (liveJamIntensityMapping.isValid())
		{
			state.setProperty("liveJamIntensityMidiCC", liveJamIntensityMapping.ccNumber, nullptr);
			state.setProperty("liveJamIntensityMidiChannel", liveJamIntensityMapping.channel, nullptr);
			state.setProperty("liveJamIntensityMidiIsNote", liveJamIntensityMapping.isNote, nullptr);
		}
		if (surpriseMeMapping.isValid())
		{
			state.setProperty("surpriseMeMidiCC", surpriseMeMapping.ccNumber, nullptr);
			state.setProperty("surpriseMeMidiChannel", surpriseMeMapping.channel, nullptr);
			state.setProperty("surpriseMeMidiIsNote", surpriseMeMapping.isNote, nullptr);
		}
		if (tripletModeMapping.isValid())
		{
			state.setProperty("tripletModeMidiCC", tripletModeMapping.ccNumber, nullptr);
			state.setProperty("tripletModeMidiChannel", tripletModeMapping.channel, nullptr);
			state.setProperty("tripletModeMidiIsNote", tripletModeMapping.isNote, nullptr);
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

	void BeatCrafterProcessor::setStateInformation(const void *data, int sizeInBytes)
	{
		auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
		if (tree.isValid())
		{
			intensityParam->setValueNotifyingHost(tree.getProperty("intensity", 0.5f));
			liveJamModeState = tree.getProperty("liveJamMode", false);
			patternEngine.setLiveJamMode(liveJamModeState);
			liveJamIntensityParam->setValueNotifyingHost(tree.getProperty("liveJamIntensity", 0.5f));
			bool surpriseMeActive = tree.getProperty("surpriseMe", false);
			surpriseMeParam->setValueNotifyingHost(surpriseMeActive ? 1.0f : 0.0f);
			patternEngine.perfParams.surpriseMeEnabled = surpriseMeActive;
			bool tripletActive = tree.getProperty("tripletMode", false);
			tripletModeParam->setValueNotifyingHost(tripletActive ? 1.0f : 0.0f);
			patternEngine.perfParams.tripletMode = tripletActive;

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
							auto &track = restoredPattern->getTrack(trackIndex);

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
										auto &step = track.getStep(stepIndex);

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

			if (tree.hasProperty("liveJamIntensityMidiCC"))
			{
				liveJamIntensityMapping.ccNumber = tree.getProperty("liveJamIntensityMidiCC", -1);
				liveJamIntensityMapping.channel = tree.getProperty("liveJamIntensityMidiChannel", -1);
				liveJamIntensityMapping.isNote = tree.getProperty("liveJamIntensityMidiIsNote", false);
				hasSavedMidiMappings = true;
			}

			if (tree.hasProperty("surpriseMeMidiCC"))
			{
				surpriseMeMapping.ccNumber = tree.getProperty("surpriseMeMidiCC", -1);
				surpriseMeMapping.channel = tree.getProperty("surpriseMeMidiChannel", -1);
				surpriseMeMapping.isNote = tree.getProperty("surpriseMeMidiIsNote", false);
			}

			if (tree.hasProperty("tripletModeMidiCC"))
			{
				tripletModeMapping.ccNumber = tree.getProperty("tripletModeMidiCC", -1);
				tripletModeMapping.channel = tree.getProperty("tripletModeMidiChannel", -1);
				tripletModeMapping.isNote = tree.getProperty("tripletModeMidiIsNote", false);
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
			getPatternEngine().switchToSlot(activeSlot, true, intensityParam->get());

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
		else if (targetType == 2)
		{
			liveJamIntensityMapping = MidiMapping{};
		}
		else if (targetType == 3)
			surpriseMeMapping = {};
		else if (targetType == 4)
			tripletModeMapping = {};
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
		else if (targetType == 2)
		{
			return liveJamIntensityMapping.isValid();
		}
		else if (targetType == 3)
			return surpriseMeMapping.isValid();
		else if (targetType == 4)
			return tripletModeMapping.isValid();
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
		else if (targetType == 2)
		{
			mapping = liveJamIntensityMapping;
		}
		else if (targetType == 4)
			mapping = tripletModeMapping;
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

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
	return new BeatCrafter::BeatCrafterProcessor();
}