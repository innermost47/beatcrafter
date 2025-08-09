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

		addParameter(playParam = new juce::AudioParameterBool(
			"play", "Play", false));
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

		// Check if we should be playing
		bool shouldPlay = playParam->get() || posInfo->getIsPlaying();
		if (shouldPlay && !patternEngine.getIsPlaying()) {
			patternEngine.start();
		}
		else if (!shouldPlay && patternEngine.getIsPlaying()) {
			patternEngine.stop();
		}

		// Process MIDI
		if (shouldPlay) {
			patternEngine.processBlock(midiMessages, buffer.getNumSamples(),
				currentSampleRate, *posInfo);
		}
	}

	juce::AudioProcessorEditor* BeatCrafterProcessor::createEditor() {
		return new BeatCrafterEditor(*this);
	}

	void BeatCrafterProcessor::getStateInformation(juce::MemoryBlock& destData) {
		// Store parameters
		auto state = juce::ValueTree("BeatCrafterState");
		state.setProperty("intensity", intensityParam->get(), nullptr);
		state.setProperty("style", styleParam->getIndex(), nullptr);

		juce::MemoryOutputStream stream(destData, false);
		state.writeToStream(stream);
	}

	void BeatCrafterProcessor::setStateInformation(const void* data, int sizeInBytes) {
		// Restore parameters
		auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
		if (tree.isValid()) {
			intensityParam->setValueNotifyingHost(tree.getProperty("intensity", 0.5f));
			styleParam->setValueNotifyingHost(tree.getProperty("style", 0));
		}
	}

} // namespace BeatCrafter

// Required for plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new BeatCrafter::BeatCrafterProcessor();
}