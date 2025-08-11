#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace BeatCrafter {

	BeatCrafterProcessor::BeatCrafterProcessor()
		: AudioProcessor(BusesProperties()
			.withOutput("Output", juce::AudioChannelSet::stereo(), true)) {

		// Add parameters
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
		// We only support stereo output (even though we produce MIDI)
		if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
			return false;

		return true;
	}

	void BeatCrafterProcessor::processBlock(juce::AudioBuffer<float>& buffer,
		juce::MidiBuffer& midiMessages) {
		buffer.clear();

		// Get play head position from host
		auto playHead = getPlayHead();
		if (playHead == nullptr) return;

		auto posInfo = playHead->getPosition();
		if (!posInfo.hasValue()) return;

		// Update parameters
		patternEngine.setIntensity(intensityParam->get());

		// SIMPLIFIER : se baser uniquement sur l'état du DAW
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

		// Paramètres existants
		state.setProperty("intensity", intensityParam->get(), nullptr);
		state.setProperty("style", styleParam->getIndex(), nullptr);
		state.setProperty("activeSlot", getPatternEngine().getActiveSlot(), nullptr);

		// Sauvegarder les styles et seeds de chaque slot
		for (int i = 0; i < 8; ++i) {
			juce::String styleProps = "slotStyle" + juce::String(i);
			juce::String seedProps = "slotSeed" + juce::String(i);

			state.setProperty(styleProps, slotStyleParams[i]->getIndex(), nullptr);
			state.setProperty(seedProps, (int)getPatternEngine().getSlotSeed(i), nullptr);
		}

		// NOUVEAU : Sauvegarder les patterns de chaque slot
		for (int slotIndex = 0; slotIndex < 8; ++slotIndex) {
			Pattern* pattern = getPatternEngine().getSlot(slotIndex);
			if (pattern) {
				auto slotNode = juce::ValueTree("Slot" + juce::String(slotIndex));

				// Propriétés du pattern
				slotNode.setProperty("name", pattern->getName(), nullptr);
				slotNode.setProperty("swing", pattern->getSwing(), nullptr);
				slotNode.setProperty("length", pattern->getLength(), nullptr);

				// Time signature
				auto ts = pattern->getTimeSignature();
				slotNode.setProperty("timeSignatureNum", ts.numerator, nullptr);
				slotNode.setProperty("timeSignatureDenom", ts.denominator, nullptr);

				// Sauvegarder chaque track
				for (int trackIndex = 0; trackIndex < pattern->getNumTracks(); ++trackIndex) {
					auto trackNode = juce::ValueTree("Track" + juce::String(trackIndex));
					const auto& track = pattern->getTrack(trackIndex);

					trackNode.setProperty("name", track.getName(), nullptr);
					trackNode.setProperty("midiNote", track.getMidiNote(), nullptr);

					// Sauvegarder chaque step
					for (int stepIndex = 0; stepIndex < track.getLength(); ++stepIndex) {
						const auto& step = track.getStep(stepIndex);

						if (step.isActive()) { // Sauvegarder seulement les steps actifs
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

		juce::MemoryOutputStream stream(destData, false);
		state.writeToStream(stream);
	}

	void BeatCrafterProcessor::setStateInformation(const void* data, int sizeInBytes) {
		auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
		if (tree.isValid()) {
			// Restaurer les paramètres de base
			intensityParam->setValueNotifyingHost(tree.getProperty("intensity", 0.5f));
			styleParam->setValueNotifyingHost(tree.getProperty("style", 0));

			// Restaurer les styles et seeds des slots
			for (int i = 0; i < 8; ++i) {
				juce::String styleProps = "slotStyle" + juce::String(i);
				juce::String seedProps = "slotSeed" + juce::String(i);

				int styleIndex = tree.getProperty(styleProps, 0);
				uint32_t seed = static_cast<uint32_t>(static_cast<int>(tree.getProperty(seedProps, 0)));

				slotStyleParams[i]->setValueNotifyingHost(styleIndex);
				getPatternEngine().setSlotStyle(i, static_cast<StyleType>(styleIndex));
				getPatternEngine().setSlotSeed(i, seed);
			}

			// NOUVEAU : Restaurer les patterns de chaque slot
			for (int slotIndex = 0; slotIndex < 8; ++slotIndex) {
				auto slotNode = tree.getChildWithName("Slot" + juce::String(slotIndex));
				if (slotNode.isValid()) {
					// Créer un nouveau pattern
					auto restoredPattern = std::make_unique<Pattern>(
						slotNode.getProperty("name", "Pattern " + juce::String(slotIndex + 1))
					);

					// Restaurer les propriétés du pattern
					restoredPattern->setSwing(slotNode.getProperty("swing", 0.0f));
					int patternLength = slotNode.getProperty("length", 16);
					restoredPattern->setLength(patternLength);

					// Time signature
					TimeSignature ts;
					ts.numerator = slotNode.getProperty("timeSignatureNum", 4);
					ts.denominator = slotNode.getProperty("timeSignatureDenom", 4);
					restoredPattern->setTimeSignature(ts);

					// Restaurer chaque track
					for (int trackIndex = 0; trackIndex < restoredPattern->getNumTracks(); ++trackIndex) {
						auto trackNode = slotNode.getChildWithName("Track" + juce::String(trackIndex));
						if (trackNode.isValid()) {
							auto& track = restoredPattern->getTrack(trackIndex);

							// Restaurer les propriétés de la track
							track.setName(trackNode.getProperty("name", track.getName()));
							track.setMidiNote(trackNode.getProperty("midiNote", track.getMidiNote()));

							// Effacer la track avant de restaurer
							track.clear();

							// Restaurer chaque step
							for (int childIndex = 0; childIndex < trackNode.getNumChildren(); ++childIndex) {
								auto stepNode = trackNode.getChild(childIndex);
								juce::String stepName = stepNode.getType().toString();

								// Extraire l'index du step depuis le nom (Step0, Step1, etc.)
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

					// Charger le pattern restauré dans le slot
					getPatternEngine().loadPatternToSlot(std::move(restoredPattern), slotIndex);
				}
			}

			// Restaurer le slot actif en dernier
			int activeSlot = tree.getProperty("activeSlot", 0);
			getPatternEngine().switchToSlot(activeSlot, true);
		}
	}

} // namespace BeatCrafter

// Required for plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new BeatCrafter::BeatCrafterProcessor();
}