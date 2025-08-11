#include "PatternEngine.h"
#include "StyleIntensityGenerator.h" 
#include <random>

namespace BeatCrafter {

	PatternEngine::PatternEngine() {
		auto pattern = std::make_unique<Pattern>("Rock Basic");
		pattern->generateBasicRockPattern();
		slots[0] = std::move(pattern);

		std::random_device rd;
		for (int i = 0; i < 8; ++i) {
			slotStyles[i] = StyleType::Rock;
			slotRandomSeeds[i] = rd();
		}
	}

	void PatternEngine::generateNewPatternForSlot(int slot, StyleType style, float complexity) {
		if (slot < 0 || slot >= 8) return;

		auto newPattern = std::make_unique<Pattern>("Generated " + juce::String(slot + 1));

		switch (style) {
		case StyleType::Rock:
			newPattern->generateBasicRockPattern();
			break;
		case StyleType::Metal:
			newPattern->generateBasicMetalPattern();
			break;
		case StyleType::Jazz:
			newPattern->generateBasicJazzPattern();
			break;
		case StyleType::Funk:
			newPattern->generateBasicFunkPattern();
			break;
		case StyleType::Electronic:
			newPattern->generateBasicElectronicPattern();
			break;
		case StyleType::HipHop:
			newPattern->generateBasicHipHopPattern();
			break;
		case StyleType::Latin:
			newPattern->generateBasicLatinPattern();
			break;
		case StyleType::Punk:
			newPattern->generateBasicPunkPattern();
			break;
		default:
			newPattern->generateBasicRockPattern();
			break;
		}

		if (complexity > 0.5f) {
			applyComplexityToPattern(*newPattern, style, complexity);
		}

		loadPatternToSlot(std::move(newPattern), slot);
	}

	void PatternEngine::loadPatternToSlot(std::unique_ptr<Pattern> pattern, int slot) {
		if (slot >= 0 && slot < 8) {
			slots[slot] = std::move(pattern);
		}
	}

	void PatternEngine::switchToSlot(int slot, bool immediate) {
		if (slot >= 0 && slot < 8 && slots[slot]) {
			if (immediate) {
				activeSlot = slot;
				queuedSlot = -1;
			}
			else {
				queuedSlot = slot;
			}
		}
	}

	void PatternEngine::processBlock(juce::MidiBuffer& midiMessages,
		int numSamples,
		double sampleRate,
		const juce::AudioPlayHead::PositionInfo& posInfo) {

		midiMessages.clear();

		if (!isPlaying || !slots[activeSlot])
			return;

		double bpm = posInfo.getBpm().orFallback(120.0);
		double ppqPosition = posInfo.getPpqPosition().orFallback(0.0);
		bool isPlayingDAW = posInfo.getIsPlaying();

		if (!isPlayingDAW) {
			return;
		}

		double beatsPerSecond = bpm / 60.0;
		double sixteenthsPerSecond = beatsPerSecond * 4.0;
		samplesPerStep = static_cast<int>(sampleRate / sixteenthsPerSecond);

		auto& pattern = *slots[activeSlot];
		int patternLength = pattern.getLength();

		double ppqPerStep = 0.25;
		int currentStepFromPPQ = static_cast<int>(ppqPosition / ppqPerStep) % patternLength;

		if (currentStepFromPPQ != pattern.getCurrentStep()) {
			pattern.setCurrentStep(currentStepFromPPQ);

			if (queuedSlot >= 0 && currentStepFromPPQ == 0) {
				activeSlot = queuedSlot;
				queuedSlot = -1;
			}

			generateMidiForStep(midiMessages, 0, pattern, currentStepFromPPQ);
		}

		lastPpqPosition = ppqPosition;
	}

	void PatternEngine::generateMidiForStep(juce::MidiBuffer& midiMessages,
		int samplePosition,
		const Pattern& pattern,
		int stepIndex) {

		Pattern intensifiedPattern = applyIntensity(pattern, currentIntensity);

		for (int trackIdx = 0; trackIdx < intensifiedPattern.getNumTracks(); ++trackIdx) {
			const auto& track = intensifiedPattern.getTrack(trackIdx);
			const auto& step = track.getStep(stepIndex);

			if (step.isActive()) {
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<> dis(0.0, 1.0);

				if (dis(gen) <= step.getProbability()) {
					int midiNote = track.getMidiNote();
					int velocity = static_cast<int>(step.getVelocity() * 127.0f);

					int timingOffset = static_cast<int>(step.getMicroTiming() * samplesPerStep * 0.1f);
					int finalSamplePos = juce::jmax(0, samplePosition + timingOffset);

					auto noteOn = juce::MidiMessage::noteOn(10, midiNote, (juce::uint8)velocity);
					midiMessages.addEvent(noteOn, finalSamplePos);

					int noteLength = static_cast<int>(0.1 * 44100);
					auto noteOff = juce::MidiMessage::noteOff(10, midiNote);
					midiMessages.addEvent(noteOff, juce::jmin(finalSamplePos + noteLength,
						samplePosition + samplesPerStep - 1));
				}
			}
		}
	}

	Pattern PatternEngine::applyIntensity(const Pattern& basePattern, float intensity) const {
		StyleType currentStyle = getSlotStyle(activeSlot);
		uint32_t currentSeed = slotRandomSeeds[activeSlot];
		return StyleIntensityGenerator::applyStyleBasedIntensity(basePattern, intensity, currentStyle, currentSeed);
	}

	void PatternEngine::generateNewPattern(StyleType style, float complexity) {
		if (!slots[activeSlot]) {
			slots[activeSlot] = std::make_unique<Pattern>("Generated");
		}

		auto& pattern = *slots[activeSlot];
		pattern.setName("Generated " + juce::String((int)style));

		switch (style) {
		case StyleType::Rock:
			pattern.generateBasicRockPattern();
			break;
		case StyleType::Metal:
			pattern.generateBasicMetalPattern();
			break;
		case StyleType::Jazz:
			pattern.generateBasicJazzPattern();
			break;
		case StyleType::Funk:
			pattern.generateBasicFunkPattern();
			break;
		case StyleType::Electronic:
			pattern.generateBasicElectronicPattern();
			break;
		case StyleType::HipHop:
			pattern.generateBasicHipHopPattern();
			break;
		case StyleType::Latin:
			pattern.generateBasicLatinPattern();
			break;
		case StyleType::Punk:
			pattern.generateBasicPunkPattern();
			break;
		default:
			pattern.generateBasicRockPattern();
			break;
		}

		if (complexity > 0.5f) {
			applyComplexityToPattern(pattern, style, complexity);
		}
	}

	void PatternEngine::applyComplexityToPattern(Pattern& pattern, StyleType style, float complexity) {
		switch (style) {
		case StyleType::Rock:
			if (complexity > 0.5f) {
				pattern.getTrack(0).getStep(2).setActive(true);
				pattern.getTrack(0).getStep(2).setVelocity(0.7f);
				pattern.getTrack(0).getStep(10).setActive(true);
				pattern.getTrack(0).getStep(10).setVelocity(0.7f);
			}
			if (complexity > 0.7f) {
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.8f);
			}
			break;

		case StyleType::Metal:
			if (complexity > 0.6f) {
				pattern.getTrack(0).getStep(1).setActive(true);
				pattern.getTrack(0).getStep(1).setVelocity(0.7f);
				pattern.getTrack(0).getStep(9).setActive(true);
				pattern.getTrack(0).getStep(9).setVelocity(0.7f);
			}
			if (complexity > 0.7f) {
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.9f);
			}
			break;

		case StyleType::Jazz:
			if (complexity > 0.6f) {
				pattern.getTrack(1).getStep(7).setActive(true);
				pattern.getTrack(1).getStep(7).setVelocity(0.25f);
				pattern.getTrack(1).getStep(15).setActive(true);
				pattern.getTrack(1).getStep(15).setVelocity(0.3f);
			}
			break;

		case StyleType::Electronic:
			if (complexity > 0.6f) {
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.8f);
			}
			if (complexity > 0.7f) {
				pattern.getTrack(0).getStep(6).setActive(true);
				pattern.getTrack(0).getStep(6).setVelocity(0.7f);
			}
			break;

		case StyleType::HipHop:
			if (complexity > 0.6f) {
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.9f);
			}
			if (complexity > 0.7f) {
				pattern.getTrack(0).getStep(3).setActive(true);
				pattern.getTrack(0).getStep(3).setVelocity(0.8f);
			}
			break;

		case StyleType::Funk:
			if (complexity > 0.6f) {
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.9f);
			}
			if (complexity > 0.7f) {
				pattern.getTrack(1).getStep(10).setActive(true);
				pattern.getTrack(1).getStep(10).setVelocity(0.35f);
			}
			break;

		case StyleType::Latin:
			if (complexity > 0.6f) {
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.8f);
			}
			if (complexity > 0.7f) {
				pattern.getTrack(0).getStep(13).setActive(true);
				pattern.getTrack(0).getStep(13).setVelocity(0.7f);
			}
			break;

		case StyleType::Punk:
			if (complexity > 0.6f) {
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(1.0f);
			}
			if (complexity > 0.7f) {
				for (int i = 1; i < 16; i += 2) {
					pattern.getTrack(2).getStep(i).setActive(true);
					pattern.getTrack(2).getStep(i).setVelocity(0.6f);
				}
			}
			break;
		}
	}
}