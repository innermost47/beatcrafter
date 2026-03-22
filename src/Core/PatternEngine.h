#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Pattern.h"
#include "StyleManager.h"
#include <array>
#include <memory>
#include <random>

namespace BeatCrafter
{
	class PatternEngine
	{
	public:
		struct PerformanceParams
		{
			bool humanizeEnabled = true;
			float humanizeAmount = 0.08f;
			float omitChance = 0.05f;
			bool  tripletMode = false;
			bool surpriseMeEnabled = false;
			float surpriseMeRange = 0.15f;
		};

		PatternEngine();
		~PatternEngine() = default;

		Pattern& getCurrentPattern() { return slots[activeSlot] ? *slots[activeSlot] : dummyPattern; }
		const Pattern& getCurrentPattern() const { return *slots[activeSlot]; }
		const Pattern* getDisplayPattern() const { return &intensifiedPatternCache; }
		Pattern* getCurrentBasePattern() { return slots[activeSlot].get(); }
		Pattern* getSlot(int index) { return (index >= 0 && index < 8 && slots[index]) ? slots[index].get() : nullptr; }

		PerformanceParams perfParams;

		void loadPatternToSlot(std::unique_ptr<Pattern> pattern, int slot);
		void generateNewPattern(StyleType style, float complexity = 0.5f);
		void generateNewPatternForSlot(int slot, StyleType style, float complexity = 0.5f);
		Pattern applyIntensity(const Pattern& basePattern, float intensity) const;

		void switchToSlot(int slot, bool immediate = false, float intensity = -1.0f);
		int getActiveSlot() const { return activeSlot; }
		int getCurrentStep() const { return slots[activeSlot] ? slots[activeSlot]->getCurrentStep() : -1; }

		void setSlotStyle(int slot, StyleType style)
		{
			if (slot >= 0 && slot < 8)
				slotStyles[slot] = style;
		}
		StyleType getSlotStyle(int slot) const { return (slot >= 0 && slot < 8) ? slotStyles[slot] : StyleType::Rock; }
		StyleType getCurrentSlotStyle() const { return slotStyles[activeSlot]; }

		std::function<void(float)> onIntensityChanged;

		void regenerateSlotSeed(int slot)
		{
			if (slot >= 0 && slot < 8)
			{
				std::random_device rd;
				slotRandomSeeds[slot] = rd();
			}
		}
		uint32_t getSlotSeed(int slot) const { return (slot >= 0 && slot < 8) ? slotRandomSeeds[slot] : 0; }
		void setSlotSeed(int slot, uint32_t seed)
		{
			if (slot >= 0 && slot < 8)
				slotRandomSeeds[slot] = seed;
		}

		void setIntensity(float intensity)
		{
			if (std::abs(currentIntensity - intensity) > 0.001f)
			{
				currentIntensity = intensity;
				intensityCacheValid = false;
			}
		}
		float getIntensity() const { return currentIntensity; }
		void invalidateCache() { intensityCacheValid = false; }

		void setLiveJamMode(bool enabled) { liveJamMode = enabled; }
		void setLiveJamIntensity(float intensity) { currentLiveJamIntensity = intensity; }
		float getLiveJamIntensity() const { return currentLiveJamIntensity; }

		void start();
		void stop();
		void resetToStart();
		bool getIsPlaying() const { return isPlaying; }

		void processBlock(juce::MidiBuffer& midiMessages,
			int numSamples,
			double sampleRate,
			const juce::AudioPlayHead::PositionInfo& posInfo);

	private:
		std::array<std::unique_ptr<Pattern>, 8> slots;
		std::array<StyleType, 8> slotStyles;
		std::array<uint32_t, 8> slotRandomSeeds;
		int activeSlot = 0;
		int queuedSlot = -1;

		mutable Pattern intensifiedPatternCache;
		mutable Pattern cachedIntensifiedPattern;
		mutable bool intensityCacheValid = false;
		mutable float lastCachedIntensity = -1.0f;
		mutable int lastCachedSlot = -1;
		Pattern dummyPattern{ "Empty" };

		float currentIntensity = -1.0f;
		float currentLiveJamIntensity = 0.5f;

		bool isPlaying = false;
		double lastPpqPosition = 0.0;
		int samplesPerStep = 0;
		int sampleCounter = 0;

		float surpriseMeCenter = 0.5f;
		float surpriseMePhase = 0.0f;
		float surpriseMeFreq = 0.3f;
		float surpriseMeAmplitude = 0.15f;
		float surpriseMeTargetFreq = 0.3f;
		float surpriseMeTargetAmplitude = 0.15f;
		int lastSurpriseMeMeasure = -1;

		bool liveJamMode = false;
		juce::Random liveJamRandom;
		int stepsSinceLastJam = 0;

		std::atomic<int> pendingImmediateSlot{ -1 };

		void generateMidiForStep(juce::MidiBuffer& midiMessages,
			int samplePosition,
			const Pattern& pattern,
			int stepIndex);
		void applyComplexityToPattern(Pattern& pattern, StyleType style, float complexity);
		void addLiveJamElements(Pattern& pattern, int stepIndex, float intensity);
		void sendAllNotesOff(juce::MidiBuffer& midiMessages);
		void applyHumanization(Pattern& pattern, int stepIndex);
		void updateSurpriseMe(int currentMeasure, double ppqPosition);
	};
}