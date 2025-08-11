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

	Pattern PatternEngine::applyIntensity(const Pattern& basePattern, float intensity) const {
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

			auto& tomHiTrack = result.getTrack(6);
			auto& tomLoTrack = result.getTrack(7);

			tomHiTrack.getStep(12).setActive(true);
			tomHiTrack.getStep(12).setVelocity(0.8f);
			tomHiTrack.getStep(13).setActive(true);
			tomHiTrack.getStep(13).setVelocity(0.7f);
			tomLoTrack.getStep(14).setActive(true);
			tomLoTrack.getStep(14).setVelocity(0.8f);
			tomLoTrack.getStep(15).setActive(true);
			tomLoTrack.getStep(15).setVelocity(0.9f);
		}

		return result;
	}

	void PatternEngine::generateNewPattern(StyleType style, float complexity) {
		// Garder le slot actuel et créer un nouveau pattern dedans
		if (!slots[activeSlot]) {
			slots[activeSlot] = std::make_unique<Pattern>("Generated");
		}

		auto& pattern = *slots[activeSlot];
		pattern.clear();
		pattern.setName("Generated " + juce::String((int)style));

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// Style-specific generation
		switch (style) {
		case StyleType::Rock:
			pattern.generateBasicRockPattern();
			if (complexity > 0.5f) {
				pattern.getTrack(0).getStep(2).setActive(true);
				pattern.getTrack(0).getStep(2).setVelocity(0.7f);
				pattern.getTrack(0).getStep(10).setActive(true);
				pattern.getTrack(0).getStep(10).setVelocity(0.7f);
			}
			break;

		case StyleType::Metal:
			pattern.generateBasicMetalPattern();
			break;

		case StyleType::Jazz:
			pattern.generateBasicJazzPattern();
			break;

		case StyleType::Funk:
			// Créer pattern funk directement ici
			pattern.clear();
			// Kick syncopé
			pattern.getTrack(0).getStep(0).setActive(true);
			pattern.getTrack(0).getStep(0).setVelocity(0.9f);
			pattern.getTrack(0).getStep(3).setActive(true);
			pattern.getTrack(0).getStep(3).setVelocity(0.7f);
			pattern.getTrack(0).getStep(10).setActive(true);
			pattern.getTrack(0).getStep(10).setVelocity(0.8f);

			// Snare avec ghost notes
			pattern.getTrack(1).getStep(4).setActive(true);
			pattern.getTrack(1).getStep(4).setVelocity(0.9f);
			pattern.getTrack(1).getStep(6).setActive(true);
			pattern.getTrack(1).getStep(6).setVelocity(0.3f); // Ghost
			pattern.getTrack(1).getStep(12).setActive(true);
			pattern.getTrack(1).getStep(12).setVelocity(0.85f);

			// Hi-hat dense
			for (int i = 0; i < 16; ++i) {
				pattern.getTrack(2).getStep(i).setActive(true);
				pattern.getTrack(2).getStep(i).setVelocity(0.4f + (i % 2) * 0.2f);
			}
			break;

		case StyleType::Electronic:
			// Pattern électro
			pattern.clear();
			// Kick sur tous les temps
			for (int i = 0; i < 16; i += 4) {
				pattern.getTrack(0).getStep(i).setActive(true);
				pattern.getTrack(0).getStep(i).setVelocity(0.9f);
			}
			// Snare sur 2 et 4
			pattern.getTrack(1).getStep(4).setActive(true);
			pattern.getTrack(1).getStep(4).setVelocity(0.8f);
			pattern.getTrack(1).getStep(12).setActive(true);
			pattern.getTrack(1).getStep(12).setVelocity(0.8f);
			// Hi-hat 16èmes
			for (int i = 0; i < 16; ++i) {
				pattern.getTrack(2).getStep(i).setActive(true);
				pattern.getTrack(2).getStep(i).setVelocity(0.3f + (i % 4 == 0 ? 0.2f : 0.0f));
			}
			break;

		case StyleType::HipHop:
			// Pattern hip-hop
			pattern.clear();
			// Kick lourd
			pattern.getTrack(0).getStep(0).setActive(true);
			pattern.getTrack(0).getStep(0).setVelocity(1.0f);
			pattern.getTrack(0).getStep(6).setActive(true);
			pattern.getTrack(0).getStep(6).setVelocity(0.8f);

			// Snare avec ghost
			pattern.getTrack(1).getStep(4).setActive(true);
			pattern.getTrack(1).getStep(4).setVelocity(0.9f);
			pattern.getTrack(1).getStep(10).setActive(true);
			pattern.getTrack(1).getStep(10).setVelocity(0.4f); // Ghost
			pattern.getTrack(1).getStep(12).setActive(true);
			pattern.getTrack(1).getStep(12).setVelocity(0.85f);

			// Hi-hat shuffle
			for (int i = 2; i < 16; i += 4) {
				pattern.getTrack(2).getStep(i).setActive(true);
				pattern.getTrack(2).getStep(i).setVelocity(0.6f);
			}
			break;

		case StyleType::Latin:
			// Pattern latin
			pattern.clear();
			// Kick pattern latin
			pattern.getTrack(0).getStep(0).setActive(true);
			pattern.getTrack(0).getStep(0).setVelocity(0.8f);
			pattern.getTrack(0).getStep(5).setActive(true);
			pattern.getTrack(0).getStep(5).setVelocity(0.7f);
			pattern.getTrack(0).getStep(8).setActive(true);
			pattern.getTrack(0).getStep(8).setVelocity(0.75f);
			pattern.getTrack(0).getStep(13).setActive(true);
			pattern.getTrack(0).getStep(13).setVelocity(0.7f);

			// Snare sur 2 et 4
			pattern.getTrack(1).getStep(4).setActive(true);
			pattern.getTrack(1).getStep(4).setVelocity(0.8f);
			pattern.getTrack(1).getStep(12).setActive(true);
			pattern.getTrack(1).getStep(12).setVelocity(0.8f);

			// Hi-hat ouvert sur off-beats
			pattern.getTrack(3).getStep(2).setActive(true);
			pattern.getTrack(3).getStep(2).setVelocity(0.6f);
			pattern.getTrack(3).getStep(6).setActive(true);
			pattern.getTrack(3).getStep(6).setVelocity(0.6f);
			pattern.getTrack(3).getStep(10).setActive(true);
			pattern.getTrack(3).getStep(10).setVelocity(0.6f);
			pattern.getTrack(3).getStep(14).setActive(true);
			pattern.getTrack(3).getStep(14).setVelocity(0.6f);
			break;

		case StyleType::Punk:
			// Pattern punk rapide et agressif
			pattern.clear();
			// Kick sur tous les temps
			for (int i = 0; i < 16; i += 4) {
				pattern.getTrack(0).getStep(i).setActive(true);
				pattern.getTrack(0).getStep(i).setVelocity(0.95f);
			}
			// Snare sur 2 et 4 très fort
			pattern.getTrack(1).getStep(4).setActive(true);
			pattern.getTrack(1).getStep(4).setVelocity(1.0f);
			pattern.getTrack(1).getStep(12).setActive(true);
			pattern.getTrack(1).getStep(12).setVelocity(1.0f);
			// Hi-hat 8èmes rapides
			for (int i = 0; i < 16; i += 2) {
				pattern.getTrack(2).getStep(i).setActive(true);
				pattern.getTrack(2).getStep(i).setVelocity(0.8f);
			}
			// Crash sur le 1
			pattern.getTrack(4).getStep(0).setActive(true);
			pattern.getTrack(4).getStep(0).setVelocity(0.9f);
			break;

		default:
			pattern.generateBasicRockPattern();
			break;
		}
	}

} // namespace BeatCrafter