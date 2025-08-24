#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include "Core/PatternEngine.h"

namespace BeatCrafter
{

	class BeatCrafterProcessor : public juce::AudioProcessor
	{
	public:
		BeatCrafterProcessor();
		~BeatCrafterProcessor() override;

		void prepareToPlay(double sampleRate, int samplesPerBlock) override;
		void releaseResources() override;

		bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

		void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

		void processMidi(juce::MidiBuffer& midiMessages);

		void processPC(int programNumber, int channel);

		void processMidiNoteOn(int noteNumber, int channel, int velocity);

		void learnForNoteOn(int noteNumber, int channel);

		void learnForPC(int programNumber, int channel);

		void processForCC(int ccNumber, int channel, int value);

		void learnByType(int ccNumber, int channel);

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

		PatternEngine& getPatternEngine() { return patternEngine; }

		juce::AudioParameterFloat* intensityParam;
		juce::AudioParameterChoice* styleParam;
		std::array<juce::AudioParameterChoice*, 8> slotStyleParams;
		juce::AudioParameterFloat* liveJamIntensityParam = nullptr;

		void startMidiLearn(int targetType, int targetSlot = -1);
		void stopMidiLearn();
		bool isMidiLearning() const { return midiLearnMode; }
		int getMidiLearnTarget() const { return midiLearnTargetType; }
		int getMidiLearnSlot() const { return midiLearnTargetSlot; }

		void clearMidiMapping(int targetType, int targetSlot = -1);
		bool hasMidiMapping(int targetType, int targetSlot = -1) const;
		juce::String getMidiMappingDescription(int targetType, int targetSlot = -1) const;
		void setupDefaultMidiMappings();
		void setLiveJamMode(bool enabled);
		bool getLiveJamMode() const { return liveJamModeState; }

	private:
		PatternEngine patternEngine;
		double currentSampleRate = 44100.0;

		bool midiLearnMode = false;
		int midiLearnTargetType = -1;
		int midiLearnTargetSlot = -1;

		struct MidiMapping
		{
			int ccNumber = -1;
			int channel = -1;
			bool isNote = false;
			bool isProgramChange = false;
			bool isValid() const { return ccNumber >= 0 && channel >= 0; }
		};

		MidiMapping intensityMapping;
		MidiMapping liveJamIntensityMapping;
		std::array<MidiMapping, 8> slotMappings;

		bool liveJamMode = false;
		juce::Random liveJamRandom;
		int stepsSinceLastJam = 0;
		bool liveJamModeState = false;
		void updateEditorFromState();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatCrafterProcessor)
	};

}