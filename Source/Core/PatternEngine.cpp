#include "PatternEngine.h"
#include <random>

namespace BeatCrafter {

	PatternEngine::PatternEngine() {
		// Initialize with one basic pattern in slot 0
		auto pattern = std::make_unique<Pattern>("Rock Basic");
		pattern->generateBasicRockPattern();
		slots[0] = std::move(pattern);
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

		// Get tempo and position from DAW
		double bpm = posInfo.getBpm().orFallback(120.0);
		double ppqPosition = posInfo.getPpqPosition().orFallback(0.0);
		bool isPlayingDAW = posInfo.getIsPlaying();

		if (!isPlayingDAW) {
			return;
		}

		// Calculate samples per 16th note
		double beatsPerSecond = bpm / 60.0;
		double sixteenthsPerSecond = beatsPerSecond * 4.0;
		samplesPerStep = static_cast<int>(sampleRate / sixteenthsPerSecond);

		auto& pattern = *slots[activeSlot];
		int patternLength = pattern.getLength();

		// Calculate current step from PPQ position
		double ppqPerStep = 0.25; // 16th notes
		int currentStepFromPPQ = static_cast<int>(ppqPosition / ppqPerStep) % patternLength;

		// Check if we've moved to a new step
		if (currentStepFromPPQ != pattern.getCurrentStep()) {
			pattern.setCurrentStep(currentStepFromPPQ);

			// Check for queued slot change at bar boundaries
			if (queuedSlot >= 0 && currentStepFromPPQ == 0) {
				activeSlot = queuedSlot;
				queuedSlot = -1;
			}

			// Generate MIDI for this step
			generateMidiForStep(midiMessages, 0, pattern, currentStepFromPPQ);
		}

		lastPpqPosition = ppqPosition;
	}

	void PatternEngine::generateMidiForStep(juce::MidiBuffer& midiMessages,
		int samplePosition,
		const Pattern& pattern,
		int stepIndex) {

		// Apply intensity transformations
		Pattern intensifiedPattern = applyIntensity(pattern, currentIntensity);

		// Generate MIDI for each track
		for (int trackIdx = 0; trackIdx < intensifiedPattern.getNumTracks(); ++trackIdx) {
			const auto& track = intensifiedPattern.getTrack(trackIdx);
			const auto& step = track.getStep(stepIndex);

			if (step.isActive()) {
				// Check probability for variation
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<> dis(0.0, 1.0);

				if (dis(gen) <= step.getProbability()) {
					int midiNote = track.getMidiNote();
					int velocity = static_cast<int>(step.getVelocity() * 127.0f);

					// Add micro-timing offset (convert to samples)
					int timingOffset = static_cast<int>(step.getMicroTiming() * samplesPerStep * 0.1f);
					int finalSamplePos = juce::jmax(0, samplePosition + timingOffset);

					// Note on
					auto noteOn = juce::MidiMessage::noteOn(10, midiNote, (juce::uint8)velocity);
					midiMessages.addEvent(noteOn, finalSamplePos);

					// Note off after 100ms
					int noteLength = static_cast<int>(0.1 * 44100); // 100ms at 44.1kHz
					auto noteOff = juce::MidiMessage::noteOff(10, midiNote);
					midiMessages.addEvent(noteOff, juce::jmin(finalSamplePos + noteLength,
						samplePosition + samplesPerStep - 1));
				}
			}
		}
	}

	Pattern PatternEngine::applyIntensity(const Pattern& basePattern, float intensity) {
		Pattern result = basePattern;

		// 0-25%: Base pattern only
		if (intensity <= 0.25f) {
			return result;
		}

		// 25-50%: Add ghost notes
		if (intensity > 0.25f && intensity <= 0.5f) {
			// Add soft snare ghost notes
			auto& snareTrack = result.getTrack(1);
			for (int i = 1; i < result.getLength(); i += 4) {
				if (!snareTrack.getStep(i).isActive()) {
					snareTrack.getStep(i).setActive(true);
					snareTrack.getStep(i).setVelocity(0.3f);
					snareTrack.getStep(i).setProbability(0.7f);
				}
			}
		}

		// 50-75%: Add double kicks and occasional crashes
		if (intensity > 0.5f && intensity <= 0.75f) {
			// Double kicks before snare hits
			auto& kickTrack = result.getTrack(0);
			auto& snareTrack = result.getTrack(1);

			for (int i = 0; i < result.getLength(); ++i) {
				if (snareTrack.getStep(i).isActive() && i > 0) {
					kickTrack.getStep(i - 1).setActive(true);
					kickTrack.getStep(i - 1).setVelocity(0.6f);
				}
			}

			// Occasional crash
			auto& crashTrack = result.getTrack(4);
			crashTrack.getStep(0).setActive(true);
			crashTrack.getStep(0).setProbability(0.3f);
		}

		// 75-100%: Maximum complexity
		if (intensity > 0.75f) {
			// Add busy hi-hats
			auto& hihatTrack = result.getTrack(2);
			for (int i = 0; i < result.getLength(); ++i) {
				hihatTrack.getStep(i).setActive(true);
				hihatTrack.getStep(i).setVelocity(0.4f + (i % 2) * 0.3f);
			}

			// Tom fills on last bar
			if (result.getCurrentStep() >= 12) {
				auto& tomHiTrack = result.getTrack(6);
				auto& tomLoTrack = result.getTrack(7);

				tomHiTrack.getStep(12).setActive(true);
				tomHiTrack.getStep(13).setActive(true);
				tomLoTrack.getStep(14).setActive(true);
				tomLoTrack.getStep(15).setActive(true);
			}
		}

		return result;
	}

	void PatternEngine::generateNewPattern(StyleType style, float complexity) {
		auto newPattern = std::make_unique<Pattern>("Generated");

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// Style-specific generation
		switch (style) {
		case StyleType::Rock:
			// Basic rock pattern
			newPattern->getTrack(0).getStep(0).setActive(true);  // Kick
			newPattern->getTrack(0).getStep(8).setActive(true);
			newPattern->getTrack(1).getStep(4).setActive(true);  // Snare
			newPattern->getTrack(1).getStep(12).setActive(true);

			// Hi-hats based on complexity
			for (int i = 0; i < 16; i += (complexity > 0.5f ? 1 : 2)) {
				newPattern->getTrack(2).getStep(i).setActive(true);
				newPattern->getTrack(2).getStep(i).setVelocity(0.5f + complexity * 0.3f);
			}
			break;

		case StyleType::Metal:
			// Double kicks
			for (int i = 0; i < 16; i += 2) {
				if (dis(gen) < 0.6f + complexity * 0.3f) {
					newPattern->getTrack(0).getStep(i).setActive(true);
				}
			}
			// Snare on 2 and 4
			newPattern->getTrack(1).getStep(4).setActive(true);
			newPattern->getTrack(1).getStep(12).setActive(true);
			break;

		case StyleType::Jazz:
			// Swing pattern
			newPattern->setSwing(0.67f);
			// Ride pattern
			for (int i = 0; i < 16; i += 2) {
				newPattern->getTrack(5).getStep(i).setActive(true);
				newPattern->getTrack(5).getStep(i).setVelocity(0.4f);
			}
			// Subtle kick
			newPattern->getTrack(0).getStep(0).setActive(true);
			newPattern->getTrack(0).getStep(0).setVelocity(0.5f);
			break;

		default:
			newPattern->generateBasicRockPattern();
			break;
		}

		loadPatternToSlot(std::move(newPattern), activeSlot);
	}

} // namespace BeatCrafter