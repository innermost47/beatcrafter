#pragma once
#include <juce_core/juce_core.h>  
#include <juce_audio_basics/juce_audio_basics.h>  
#include "Pattern.h"
#include <array>
#include <memory>
#include <random>

namespace BeatCrafter {

	class PatternEngine {
	public:
		PatternEngine();
		~PatternEngine() = default;

		const Pattern& getCurrentPattern() const { return *slots[activeSlot]; }

		Pattern* getCurrentBasePattern() {
			return slots[activeSlot].get();
		}

		Pattern* getSlot(int index) {
			if (index >= 0 && index < 8 && slots[index])
				return slots[index].get();
			return nullptr;
		}

		void loadPatternToSlot(std::unique_ptr<Pattern> pattern, int slot);
		void switchToSlot(int slot, bool immediate = false);
		int getActiveSlot() const { return activeSlot; }
		void resetToStart();
		void processBlock(juce::MidiBuffer& midiMessages,
			int numSamples,
			double sampleRate,
			const juce::AudioPlayHead::PositionInfo& posInfo);

		void setIntensity(float intensity) {
			currentIntensity = intensity;
			intensityCacheValid = false;
		}
		float getIntensity() const { return currentIntensity; }

		void start();
		void stop();
		bool getIsPlaying() const { return isPlaying; }

		void generateNewPattern(StyleType style, float complexity = 0.5f);

		const Pattern* getDisplayPattern() const {
			if (!intensityCacheValid && slots[activeSlot]) {
				intensifiedPatternCache = applyIntensity(*slots[activeSlot], currentIntensity);
				intensityCacheValid = true;
			}
			return &intensifiedPatternCache;
		}

		Pattern applyIntensity(const Pattern& basePattern, float intensity) const;

		Pattern& getCurrentPattern() {
			return slots[activeSlot] ? *slots[activeSlot] : dummyPattern;
		}

		void setSlotStyle(int slot, StyleType style) {
			if (slot >= 0 && slot < 8) {
				slotStyles[slot] = style;
			}
		}

		void clearCurrentPattern() {
			if (slots[activeSlot]) {
				slots[activeSlot]->clear();
				setIntensity(0.0f);
			}
		}

		void generateNewPatternForSlot(int slot, StyleType style, float complexity = 0.5f);

		StyleType getSlotStyle(int slot) const {
			if (slot >= 0 && slot < 8) {
				return slotStyles[slot];
			}
			return StyleType::Rock;
		}

		StyleType getCurrentSlotStyle() const {
			return slotStyles[activeSlot];
		}

		void regenerateSlotSeed(int slot) {
			if (slot >= 0 && slot < 8) {
				std::random_device rd;
				slotRandomSeeds[slot] = rd();
			}
		}

		uint32_t getSlotSeed(int slot) const {
			if (slot >= 0 && slot < 8) {
				return slotRandomSeeds[slot];
			}
			return 0;
		}

		void setSlotSeed(int slot, uint32_t seed) {
			if (slot >= 0 && slot < 8) {
				slotRandomSeeds[slot] = seed;
			}
		}

	private:
		std::array<std::unique_ptr<Pattern>, 8> slots;
		int activeSlot = 0;
		int queuedSlot = -1;

		std::array<StyleType, 8> slotStyles;
		std::array<uint32_t, 8> slotRandomSeeds;

		mutable Pattern intensifiedPatternCache;
		mutable bool intensityCacheValid = false;
		Pattern dummyPattern{ "Empty" };

		float currentIntensity = 0.5f;
		bool isPlaying = false;

		double lastPpqPosition = 0.0;
		int samplesPerStep = 0;
		int sampleCounter = 0;

		void generateMidiForStep(juce::MidiBuffer& midiMessages,
			int samplePosition,
			const Pattern& pattern,
			int stepIndex);
		void applyComplexityToPattern(Pattern& pattern, StyleType style, float complexity);

	};
}