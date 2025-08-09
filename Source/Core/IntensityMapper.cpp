#include "IntensityMapper.h"
#include <random>

namespace BeatCrafter {

	Pattern IntensityMapper::applyIntensity(const Pattern& basePattern, float intensity) {
		Pattern result = basePattern;

		// Clamp intensity
		intensity = juce::jlimit(0.0f, 1.0f, intensity);

		// Apply transformations based on intensity level
		if (intensity <= 0.25f) {
			// 0-25%: Simplify pattern
			simplifyPattern(result, intensity);
		}
		else if (intensity <= 0.5f) {
			// 25-50%: Add ghost notes and subtle variations
			addGhostNotes(result, intensity);
		}
		else if (intensity <= 0.75f) {
			// 50-75%: Add complexity and accents
			addComplexity(result, intensity);
		}
		else {
			// 75-100%: Maximum intensity with fills
			addMaximumIntensity(result, intensity);
		}

		return result;
	}


	void IntensityMapper::simplifyPattern(Pattern& pattern, float intensity) {
		// Remove weak hits for cleaner pattern
		for (int track = 0; track < pattern.getNumTracks(); ++track) {
			for (int step = 0; step < pattern.getLength(); ++step) {
				auto& s = pattern.getTrack(track).getStep(step);
				if (s.isActive() && s.getVelocity() < 0.5f) {
					s.setProbability(intensity * 4.0f); // Lower probability for weak hits
				}
			}
		}
	}

	void IntensityMapper::addGhostNotes(Pattern& pattern, float intensity) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// Add ghost notes on snare
		auto& snareTrack = pattern.getTrack(1); // Snare is track 1
		for (int step = 0; step < pattern.getLength(); ++step) {
			if (!snareTrack.getStep(step).isActive() &&
				(step % 4 == 1 || step % 4 == 3)) {

				if (dis(gen) < (intensity - 0.25f) * 2.0f) {
					snareTrack.getStep(step).setActive(true);
					snareTrack.getStep(step).setVelocity(0.2f + (intensity - 0.25f));
					snareTrack.getStep(step).setProbability(0.6f);
				}
			}
		}

		// Add hi-hat variations
		auto& hihatTrack = pattern.getTrack(2);
		for (int step = 0; step < pattern.getLength(); ++step) {
			if (hihatTrack.getStep(step).isActive()) {
				// Vary velocity based on intensity
				float baseVel = hihatTrack.getStep(step).getVelocity();
				float variation = (dis(gen) - 0.5f) * intensity * 0.3f;
				hihatTrack.getStep(step).setVelocity(
					juce::jlimit(0.1f, 1.0f, baseVel + variation));
			}
		}
	}

	void IntensityMapper::addComplexity(Pattern& pattern, float intensity) {
		// Add double kicks
		auto& kickTrack = pattern.getTrack(0);
		for (int step = 0; step < pattern.getLength() - 1; ++step) {
			if (kickTrack.getStep(step).isActive() &&
				!kickTrack.getStep(step + 1).isActive()) {

				float chance = (intensity - 0.5f) * 2.0f;
				if (std::rand() / float(RAND_MAX) < chance * 0.3f) {
					kickTrack.getStep(step + 1).setActive(true);
					kickTrack.getStep(step + 1).setVelocity(0.6f);
					kickTrack.getStep(step + 1).setMicroTiming(0.1f); // Slightly late
				}
			}
		}

		// Occasional crash accents
		auto& crashTrack = pattern.getTrack(4);
		if (!crashTrack.getStep(0).isActive()) {
			crashTrack.getStep(0).setActive(true);
			crashTrack.getStep(0).setVelocity(0.5f + (intensity - 0.5f));
			crashTrack.getStep(0).setProbability(0.2f + (intensity - 0.5f) * 0.3f);
		}

		// Open hi-hats on off-beats
		auto& openHihatTrack = pattern.getTrack(3);
		for (int step = 2; step < pattern.getLength(); step += 4) {
			if (!openHihatTrack.getStep(step).isActive()) {
				openHihatTrack.getStep(step).setActive(true);
				openHihatTrack.getStep(step).setVelocity(0.4f + (intensity - 0.5f));
				openHihatTrack.getStep(step).setProbability(0.5f);
			}
		}
	}

	void IntensityMapper::addMaximumIntensity(Pattern& pattern, float intensity) {
		// All previous transformations
		addComplexity(pattern, intensity);

		// Add tom fills in last bar
		int lastBarStart = pattern.getLength() - 4;
		auto& tomHiTrack = pattern.getTrack(6);
		auto& tomLoTrack = pattern.getTrack(7);
		auto& crashTrack = pattern.getTrack(4);

		// Epic fill pattern
		if (pattern.getCurrentStep() >= lastBarStart) {
			tomHiTrack.getStep(lastBarStart).setActive(true);
			tomHiTrack.getStep(lastBarStart).setVelocity(0.8f);

			tomHiTrack.getStep(lastBarStart + 1).setActive(true);
			tomHiTrack.getStep(lastBarStart + 1).setVelocity(0.7f);

			tomLoTrack.getStep(lastBarStart + 2).setActive(true);
			tomLoTrack.getStep(lastBarStart + 2).setVelocity(0.8f);

			tomLoTrack.getStep(lastBarStart + 3).setActive(true);
			tomLoTrack.getStep(lastBarStart + 3).setVelocity(0.9f);

			// Big crash on next downbeat
			crashTrack.getStep(0).setActive(true);
			crashTrack.getStep(0).setVelocity(1.0f);
			crashTrack.getStep(0).setProbability(1.0f);
		}

		// Blast beat mode for extreme intensity
		if (intensity > 0.9f) {
			auto& kickTrack = pattern.getTrack(0);
			auto& snareTrack = pattern.getTrack(1);

			for (int step = 0; step < pattern.getLength(); ++step) {
				if (step % 2 == 0) {
					kickTrack.getStep(step).setActive(true);
					kickTrack.getStep(step).setVelocity(0.9f);
				}
				else {
					snareTrack.getStep(step).setActive(true);
					snareTrack.getStep(step).setVelocity(0.8f);
				}
			}
		}
	}
}