#pragma once
#include <juce_audio_processors/juce_audio_processors.h> 
#include <juce_core/juce_core.h>   
#include "Core/PatternEngine.h"

namespace BeatCrafter {

	class BeatCrafterProcessor : public juce::AudioProcessor {
	public:
		BeatCrafterProcessor();
		~BeatCrafterProcessor() override;

		void prepareToPlay(double sampleRate, int samplesPerBlock) override;
		void releaseResources() override;

		bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

		void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

		juce::AudioProcessorEditor* createEditor() override;
		bool hasEditor() const override { return true; }

		const juce::String getName() const override { return "BeatCrafter"; }

		bool acceptsMidi() const override { return true; }
		bool producesMidi() const override { return true; }
		bool isMidiEffect() const override { return true; }
		double getTailLengthSeconds() const override { return 0.0; }

		int getNumPrograms() override { return 1; }
		int getCurrentProgram() override { return 0; }
		void setCurrentProgram(int index) override {}
		const juce::String getProgramName(int index) override { return {}; }
		void changeProgramName(int index, const juce::String& newName) override {}

		void getStateInformation(juce::MemoryBlock& destData) override;
		void setStateInformation(const void* data, int sizeInBytes) override;

		// Public access to engine for GUI
		PatternEngine& getPatternEngine() { return patternEngine; }

		// Parameters
		juce::AudioParameterFloat* intensityParam;
		juce::AudioParameterChoice* styleParam;
		juce::AudioParameterBool* playParam;

	private:
		PatternEngine patternEngine;
		double currentSampleRate = 44100.0;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatCrafterProcessor)
	};

} // namespace BeatCrafter