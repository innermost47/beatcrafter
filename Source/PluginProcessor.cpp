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
		state.setProperty("intensity", intensityParam->get(), nullptr);
		state.setProperty("style", styleParam->getIndex(), nullptr);

		// NOUVEAU : Sauvegarder les styles de chaque slot
		for (int i = 0; i < 8; ++i) {
			juce::String propName = "slotStyle" + juce::String(i);
			state.setProperty(propName, slotStyleParams[i]->getIndex(), nullptr);
		}

		// Sauvegarder le slot actif
		state.setProperty("activeSlot", getPatternEngine().getActiveSlot(), nullptr);

		juce::MemoryOutputStream stream(destData, false);
		state.writeToStream(stream);
	}

	void BeatCrafterProcessor::setStateInformation(const void* data, int sizeInBytes) {
		auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
		if (tree.isValid()) {
			intensityParam->setValueNotifyingHost(tree.getProperty("intensity", 0.5f));
			styleParam->setValueNotifyingHost(tree.getProperty("style", 0));

			// NOUVEAU : Restaurer les styles des slots
			for (int i = 0; i < 8; ++i) {
				juce::String propName = "slotStyle" + juce::String(i);
				int styleIndex = tree.getProperty(propName, 0);
				slotStyleParams[i]->setValueNotifyingHost(styleIndex);
				getPatternEngine().setSlotStyle(i, static_cast<StyleType>(styleIndex));
			}

			// Restaurer le slot actif
			int activeSlot = tree.getProperty("activeSlot", 0);
			getPatternEngine().switchToSlot(activeSlot, true);
		}
	}

} // namespace BeatCrafter

// Required for plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new BeatCrafter::BeatCrafterProcessor();
}