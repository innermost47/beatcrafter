#include "StyleManager.h"

namespace BeatCrafter {

	void StyleManager::generateBasicPattern(Pattern& pattern, StyleType style) {
		pattern.clear();

		switch (style) {
		case StyleType::Rock: generateRockPattern(pattern); break;
		case StyleType::Metal: generateMetalPattern(pattern); break;
		case StyleType::Jazz: generateJazzPattern(pattern); break;
		case StyleType::Funk: generateFunkPattern(pattern); break;
		case StyleType::Electronic: generateElectronicPattern(pattern); break;
		case StyleType::HipHop: generateHipHopPattern(pattern); break;
		case StyleType::Latin: generateLatinPattern(pattern); break;
		case StyleType::Punk: generatePunkPattern(pattern); break;
		default: generateRockPattern(pattern); break;
		}
	}

	Pattern StyleManager::applyIntensity(const Pattern& basePattern, float intensity, StyleType style, uint32_t seed) {
		getRNG().seed(seed);
		Pattern result = basePattern;

		applyBaseIntensityScaling(result, intensity);

		if (intensity > 0.1f) {
			addSubtleVariations(result, intensity, style);
		}

		if (intensity > 0.3f) {
			changeSnarePattern(result, intensity, style);
		}

		if (intensity > 0.4f) {
			addHiHatRideVariations(result, intensity, style);
		}

		if (intensity > 0.5f) {
			addGhostNotes(result, 1, intensity * 0.7f);
		}

		if (intensity > 0.7f) {
			addRandomFills(result, intensity);
		}

		if (intensity > 0.9f) {
			addBreakMode(result, intensity, style);
		}

		return result;
	}

	void StyleManager::generateRockPattern(Pattern& pattern) {
		pattern.clear();

		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.85f + (rand() % 15) / 100.0f);

		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.9f);

		pattern.getTrack(2).getStep(4).setActive(true);
		pattern.getTrack(2).getStep(4).setVelocity(0.4f);
		pattern.getTrack(2).getStep(4).setProbability(0.7f);
	}

	void StyleManager::generateMetalPattern(Pattern& pattern) {
		pattern.clear();

		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.95f);
		pattern.getTrack(0).getStep(2).setActive(true);
		pattern.getTrack(0).getStep(2).setVelocity(0.7f);
		pattern.getTrack(0).getStep(2).setProbability(0.8f);

		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.95f);

		pattern.getTrack(5).getStep(6).setActive(true);
		pattern.getTrack(5).getStep(6).setVelocity(0.5f);
		pattern.getTrack(5).getStep(6).setProbability(0.7f);
	}

	void StyleManager::generateJazzPattern(Pattern& pattern) {
		pattern.clear();

		pattern.setSwing(0.67f);

		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.4f + (rand() % 20) / 100.0f);

		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.55f);

		pattern.getTrack(5).getStep(2).setActive(true);
		pattern.getTrack(5).getStep(2).setVelocity(0.3f);
		pattern.getTrack(5).getStep(10).setActive(true);
		pattern.getTrack(5).getStep(10).setVelocity(0.35f);
		pattern.getTrack(5).getStep(10).setProbability(0.7f);
	}

	void StyleManager::generateFunkPattern(Pattern& pattern) {
		pattern.clear();

		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.9f);
		pattern.getTrack(0).getStep(6).setActive(true);
		pattern.getTrack(0).getStep(6).setVelocity(0.6f);
		pattern.getTrack(0).getStep(6).setProbability(0.75f);

		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.85f);

		pattern.getTrack(2).getStep(4).setActive(true);
		pattern.getTrack(2).getStep(4).setVelocity(0.5f);
		pattern.getTrack(2).getStep(12).setActive(true);
		pattern.getTrack(2).getStep(12).setVelocity(0.4f);
		pattern.getTrack(2).getStep(12).setProbability(0.8f);
	}

	void StyleManager::generateElectronicPattern(Pattern& pattern) {
		pattern.clear();

		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.9f);

		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.7f);
		pattern.getTrack(1).getStep(8).setProbability(0.8f);

		pattern.getTrack(2).getStep(4).setActive(true);
		pattern.getTrack(2).getStep(4).setVelocity(0.4f);
		pattern.getTrack(2).getStep(12).setActive(true);
		pattern.getTrack(2).getStep(12).setVelocity(0.5f);
	}

	void StyleManager::generateHipHopPattern(Pattern& pattern) {
		pattern.clear();

		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.95f);

		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.9f);

		pattern.getTrack(2).getStep(4).setActive(true);
		pattern.getTrack(2).getStep(4).setVelocity(0.45f);
		pattern.getTrack(2).getStep(12).setActive(true);
		pattern.getTrack(2).getStep(12).setVelocity(0.35f);
		pattern.getTrack(2).getStep(12).setProbability(0.6f);
	}

	void StyleManager::generateLatinPattern(Pattern& pattern) {
		pattern.clear();

		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.75f);
		pattern.getTrack(0).getStep(6).setActive(true);
		pattern.getTrack(0).getStep(6).setVelocity(0.5f);
		pattern.getTrack(0).getStep(6).setProbability(0.8f);

		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.8f);

		pattern.getTrack(3).getStep(4).setActive(true);
		pattern.getTrack(3).getStep(4).setVelocity(0.55f);
		pattern.getTrack(3).getStep(12).setActive(true);
		pattern.getTrack(3).getStep(12).setVelocity(0.5f);
		pattern.getTrack(3).getStep(12).setProbability(0.75f);
	}

	void StyleManager::generatePunkPattern(Pattern& pattern) {
		pattern.clear();

		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.9f);

		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(1.0f);

		pattern.getTrack(2).getStep(4).setActive(true);
		pattern.getTrack(2).getStep(4).setVelocity(0.6f);
		pattern.getTrack(2).getStep(4).setProbability(0.7f);
		pattern.getTrack(2).getStep(12).setActive(true);
		pattern.getTrack(2).getStep(12).setVelocity(0.65f);
	}

	void StyleManager::applyComplexityToPattern(Pattern& pattern, StyleType style, float complexity) {
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

	std::mt19937& StyleManager::getRNG() {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return gen;
	}

	void StyleManager::applyBaseIntensityScaling(Pattern& pattern, float intensity) {
		for (int trackIndex = 0; trackIndex < pattern.getNumTracks(); ++trackIndex) {
			auto& track = pattern.getTrack(trackIndex);
			for (int i = 0; i < pattern.getLength(); ++i) {
				auto& step = track.getStep(i);
				if (step.isActive()) {
					float velocityMultiplier = 0.3f + intensity * 0.7f;
					step.setVelocity(step.getVelocity() * velocityMultiplier);
					if (step.getProbability() < 1.0f) {
						step.setProbability(step.getProbability() * (0.7f + intensity * 0.3f));
					}
				}
			}
		}
	}

	void StyleManager::addSubtleVariations(Pattern& pattern, float intensity, StyleType style) {
		auto& kickTrack = pattern.getTrack(0);
		auto& snareTrack = pattern.getTrack(1);
		auto& hihatTrack = pattern.getTrack(2);

		for (int i = 0; i < pattern.getLength(); ++i) {
			if (kickTrack.getStep(i).isActive() && randomChance(intensity * 0.15f)) {
				kickTrack.getStep(i).setVelocity(kickTrack.getStep(i).getVelocity() + randomFloat(-0.05f, 0.08f));
			}

			if (snareTrack.getStep(i).isActive() && randomChance(intensity * 0.12f)) {
				snareTrack.getStep(i).setVelocity(snareTrack.getStep(i).getVelocity() + randomFloat(-0.04f, 0.1f));
			}
		}

		if (intensity > 0.1f) {
			for (int i = 0; i < 16; ++i) {
				if (!hihatTrack.getStep(i).isActive() && randomChance(intensity * 0.25f)) {
					hihatTrack.getStep(i).setActive(true);
					hihatTrack.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
					hihatTrack.getStep(i).setProbability(0.5f + intensity * 0.3f);
				}
			}
		}

		for (int i = 1; i < 16; ++i) {
			if (!kickTrack.getStep(i).isActive() && randomChance(intensity * 0.1f)) {
				kickTrack.getStep(i).setActive(true);
				kickTrack.getStep(i).setVelocity(randomFloat(0.35f, 0.55f));
				kickTrack.getStep(i).setProbability(0.5f + intensity * 0.3f);
			}
		}

		if (style == StyleType::Metal && intensity > 0.3f) {
			float doubleKickChance = (intensity - 0.3f) * 1.2f;

			for (int i = 0; i < 16; ++i) {
				if (kickTrack.getStep(i).isActive() && randomChance(doubleKickChance)) {
					int nextStep = (i + 1) % 16;
					if (!kickTrack.getStep(nextStep).isActive()) {
						kickTrack.getStep(nextStep).setActive(true);
						kickTrack.getStep(nextStep).setVelocity(randomFloat(0.7f, 0.9f));
						kickTrack.getStep(nextStep).setProbability(0.6f + intensity * 0.3f);
					}
				}
			}

			if (intensity > 0.7f) {
				for (int i = 0; i < 16; i += 2) {
					if (randomChance(doubleKickChance * 0.8f)) {
						kickTrack.getStep(i).setActive(true);
						kickTrack.getStep(i).setVelocity(randomFloat(0.8f, 0.95f));
						if (i + 1 < 16) {
							kickTrack.getStep(i + 1).setActive(true);
							kickTrack.getStep(i + 1).setVelocity(randomFloat(0.75f, 0.9f));
						}
					}
				}
			}
		}
	}

	void StyleManager::addBreakMode(Pattern& pattern, float intensity, StyleType style) {
		if (randomChance(0.6f)) {
			auto& kickTrack = pattern.getTrack(0);
			auto& snareTrack = pattern.getTrack(1);
			auto& hihatTrack = pattern.getTrack(2);
			auto& openHihatTrack = pattern.getTrack(3);
			auto& rideTrack = pattern.getTrack(5);
			auto& tomHiTrack = pattern.getTrack(6);
			auto& tomLoTrack = pattern.getTrack(7);
			auto& splashTrack = pattern.getTrack(10);
			auto& chinaTrack = pattern.getTrack(11);

			for (int i = 0; i < 16; ++i) {
				snareTrack.getStep(i).setActive(false);
				hihatTrack.getStep(i).setActive(false);
				openHihatTrack.getStep(i).setActive(false);
				rideTrack.getStep(i).setActive(false);
				tomHiTrack.getStep(i).setActive(false);
				tomLoTrack.getStep(i).setActive(false);
				splashTrack.getStep(i).setActive(false);
				chinaTrack.getStep(i).setActive(false);
			}

			switch (style) {
			case StyleType::Rock:
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(0.9f);
				kickTrack.getStep(4).setActive(true);
				kickTrack.getStep(4).setVelocity(0.85f);
				kickTrack.getStep(8).setActive(true);
				kickTrack.getStep(8).setVelocity(0.9f);
				kickTrack.getStep(12).setActive(true);
				kickTrack.getStep(12).setVelocity(0.8f);

				tomHiTrack.getStep(2).setActive(true);
				tomHiTrack.getStep(2).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(6).setActive(true);
				tomHiTrack.getStep(6).setVelocity(randomFloat(0.6f, 0.8f));
				tomHiTrack.getStep(10).setActive(true);
				tomHiTrack.getStep(10).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(14).setActive(true);
				tomHiTrack.getStep(14).setVelocity(randomFloat(0.8f, 0.95f));

				tomLoTrack.getStep(1).setActive(true);
				tomLoTrack.getStep(1).setVelocity(randomFloat(0.6f, 0.8f));
				tomLoTrack.getStep(5).setActive(true);
				tomLoTrack.getStep(5).setVelocity(randomFloat(0.7f, 0.9f));
				tomLoTrack.getStep(13).setActive(true);
				tomLoTrack.getStep(13).setVelocity(randomFloat(0.7f, 0.9f));

				splashTrack.getStep(0).setActive(true);
				splashTrack.getStep(0).setVelocity(randomFloat(0.8f, 1.0f));
				splashTrack.getStep(8).setActive(true);
				splashTrack.getStep(8).setVelocity(randomFloat(0.7f, 0.9f));
				break;

			case StyleType::Metal:
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(0.95f);
				kickTrack.getStep(1).setActive(true);
				kickTrack.getStep(1).setVelocity(0.9f);
				kickTrack.getStep(2).setActive(true);
				kickTrack.getStep(2).setVelocity(0.85f);
				kickTrack.getStep(4).setActive(true);
				kickTrack.getStep(4).setVelocity(0.9f);
				kickTrack.getStep(5).setActive(true);
				kickTrack.getStep(5).setVelocity(0.85f);
				kickTrack.getStep(8).setActive(true);
				kickTrack.getStep(8).setVelocity(0.95f);
				kickTrack.getStep(9).setActive(true);
				kickTrack.getStep(9).setVelocity(0.9f);
				kickTrack.getStep(12).setActive(true);
				kickTrack.getStep(12).setVelocity(0.9f);
				kickTrack.getStep(13).setActive(true);
				kickTrack.getStep(13).setVelocity(0.85f);

				tomHiTrack.getStep(3).setActive(true);
				tomHiTrack.getStep(3).setVelocity(randomFloat(0.8f, 0.95f));
				tomHiTrack.getStep(6).setActive(true);
				tomHiTrack.getStep(6).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(10).setActive(true);
				tomHiTrack.getStep(10).setVelocity(randomFloat(0.8f, 0.95f));
				tomHiTrack.getStep(14).setActive(true);
				tomHiTrack.getStep(14).setVelocity(randomFloat(0.8f, 0.95f));

				tomLoTrack.getStep(7).setActive(true);
				tomLoTrack.getStep(7).setVelocity(randomFloat(0.8f, 0.95f));
				tomLoTrack.getStep(11).setActive(true);
				tomLoTrack.getStep(11).setVelocity(randomFloat(0.7f, 0.9f));
				tomLoTrack.getStep(15).setActive(true);
				tomLoTrack.getStep(15).setVelocity(randomFloat(0.8f, 0.95f));

				chinaTrack.getStep(0).setActive(true);
				chinaTrack.getStep(0).setVelocity(randomFloat(0.9f, 1.0f));
				splashTrack.getStep(8).setActive(true);
				splashTrack.getStep(8).setVelocity(randomFloat(0.8f, 0.95f));
				break;

			case StyleType::Jazz:
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(0.6f);
				kickTrack.getStep(6).setActive(true);
				kickTrack.getStep(6).setVelocity(0.5f);
				kickTrack.getStep(10).setActive(true);
				kickTrack.getStep(10).setVelocity(0.55f);

				tomHiTrack.getStep(1).setActive(true);
				tomHiTrack.getStep(1).setVelocity(randomFloat(0.5f, 0.7f));
				tomHiTrack.getStep(3).setActive(true);
				tomHiTrack.getStep(3).setVelocity(randomFloat(0.4f, 0.6f));
				tomHiTrack.getStep(7).setActive(true);
				tomHiTrack.getStep(7).setVelocity(randomFloat(0.5f, 0.7f));
				tomHiTrack.getStep(11).setActive(true);
				tomHiTrack.getStep(11).setVelocity(randomFloat(0.6f, 0.8f));
				tomHiTrack.getStep(13).setActive(true);
				tomHiTrack.getStep(13).setVelocity(randomFloat(0.5f, 0.7f));

				tomLoTrack.getStep(5).setActive(true);
				tomLoTrack.getStep(5).setVelocity(randomFloat(0.6f, 0.8f));
				tomLoTrack.getStep(9).setActive(true);
				tomLoTrack.getStep(9).setVelocity(randomFloat(0.5f, 0.7f));
				tomLoTrack.getStep(15).setActive(true);
				tomLoTrack.getStep(15).setVelocity(randomFloat(0.6f, 0.8f));

				splashTrack.getStep(0).setActive(true);
				splashTrack.getStep(0).setVelocity(randomFloat(0.6f, 0.8f));
				break;

			case StyleType::Funk:
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(0.9f);
				kickTrack.getStep(3).setActive(true);
				kickTrack.getStep(3).setVelocity(0.7f);
				kickTrack.getStep(7).setActive(true);
				kickTrack.getStep(7).setVelocity(0.6f);
				kickTrack.getStep(10).setActive(true);
				kickTrack.getStep(10).setVelocity(0.8f);

				tomHiTrack.getStep(1).setActive(true);
				tomHiTrack.getStep(1).setVelocity(randomFloat(0.6f, 0.8f));
				tomHiTrack.getStep(4).setActive(true);
				tomHiTrack.getStep(4).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(6).setActive(true);
				tomHiTrack.getStep(6).setVelocity(randomFloat(0.5f, 0.7f));
				tomHiTrack.getStep(12).setActive(true);
				tomHiTrack.getStep(12).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(14).setActive(true);
				tomHiTrack.getStep(14).setVelocity(randomFloat(0.6f, 0.8f));

				tomLoTrack.getStep(2).setActive(true);
				tomLoTrack.getStep(2).setVelocity(randomFloat(0.6f, 0.8f));
				tomLoTrack.getStep(8).setActive(true);
				tomLoTrack.getStep(8).setVelocity(randomFloat(0.7f, 0.9f));
				tomLoTrack.getStep(15).setActive(true);
				tomLoTrack.getStep(15).setVelocity(randomFloat(0.7f, 0.9f));

				splashTrack.getStep(0).setActive(true);
				splashTrack.getStep(0).setVelocity(randomFloat(0.7f, 0.9f));
				splashTrack.getStep(8).setActive(true);
				splashTrack.getStep(8).setVelocity(randomFloat(0.6f, 0.8f));
				break;

			case StyleType::Electronic:
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(0.9f);
				kickTrack.getStep(4).setActive(true);
				kickTrack.getStep(4).setVelocity(0.85f);
				kickTrack.getStep(8).setActive(true);
				kickTrack.getStep(8).setVelocity(0.9f);
				kickTrack.getStep(12).setActive(true);
				kickTrack.getStep(12).setVelocity(0.8f);

				tomHiTrack.getStep(2).setActive(true);
				tomHiTrack.getStep(2).setVelocity(0.7f);
				tomHiTrack.getStep(6).setActive(true);
				tomHiTrack.getStep(6).setVelocity(0.6f);
				tomHiTrack.getStep(10).setActive(true);
				tomHiTrack.getStep(10).setVelocity(0.7f);
				tomHiTrack.getStep(14).setActive(true);
				tomHiTrack.getStep(14).setVelocity(0.8f);

				tomLoTrack.getStep(1).setActive(true);
				tomLoTrack.getStep(1).setVelocity(0.6f);
				tomLoTrack.getStep(5).setActive(true);
				tomLoTrack.getStep(5).setVelocity(0.7f);
				tomLoTrack.getStep(9).setActive(true);
				tomLoTrack.getStep(9).setVelocity(0.6f);
				tomLoTrack.getStep(13).setActive(true);
				tomLoTrack.getStep(13).setVelocity(0.7f);

				splashTrack.getStep(0).setActive(true);
				splashTrack.getStep(0).setVelocity(0.8f);
				splashTrack.getStep(8).setActive(true);
				splashTrack.getStep(8).setVelocity(0.7f);
				break;

			case StyleType::HipHop:
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(0.95f);
				kickTrack.getStep(7).setActive(true);
				kickTrack.getStep(7).setVelocity(0.6f);
				kickTrack.getStep(11).setActive(true);
				kickTrack.getStep(11).setVelocity(0.7f);

				tomHiTrack.getStep(2).setActive(true);
				tomHiTrack.getStep(2).setVelocity(randomFloat(0.6f, 0.8f));
				tomHiTrack.getStep(4).setActive(true);
				tomHiTrack.getStep(4).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(6).setActive(true);
				tomHiTrack.getStep(6).setVelocity(randomFloat(0.5f, 0.7f));
				tomHiTrack.getStep(12).setActive(true);
				tomHiTrack.getStep(12).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(14).setActive(true);
				tomHiTrack.getStep(14).setVelocity(randomFloat(0.6f, 0.8f));

				tomLoTrack.getStep(1).setActive(true);
				tomLoTrack.getStep(1).setVelocity(randomFloat(0.5f, 0.7f));
				tomLoTrack.getStep(8).setActive(true);
				tomLoTrack.getStep(8).setVelocity(randomFloat(0.7f, 0.9f));
				tomLoTrack.getStep(15).setActive(true);
				tomLoTrack.getStep(15).setVelocity(randomFloat(0.7f, 0.9f));

				splashTrack.getStep(0).setActive(true);
				splashTrack.getStep(0).setVelocity(randomFloat(0.7f, 0.9f));
				break;

			case StyleType::Latin:
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(0.8f);
				kickTrack.getStep(3).setActive(true);
				kickTrack.getStep(3).setVelocity(0.6f);
				kickTrack.getStep(6).setActive(true);
				kickTrack.getStep(6).setVelocity(0.7f);
				kickTrack.getStep(10).setActive(true);
				kickTrack.getStep(10).setVelocity(0.75f);

				tomHiTrack.getStep(1).setActive(true);
				tomHiTrack.getStep(1).setVelocity(randomFloat(0.6f, 0.8f));
				tomHiTrack.getStep(4).setActive(true);
				tomHiTrack.getStep(4).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(8).setActive(true);
				tomHiTrack.getStep(8).setVelocity(randomFloat(0.6f, 0.8f));
				tomHiTrack.getStep(12).setActive(true);
				tomHiTrack.getStep(12).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(14).setActive(true);
				tomHiTrack.getStep(14).setVelocity(randomFloat(0.6f, 0.8f));

				tomLoTrack.getStep(2).setActive(true);
				tomLoTrack.getStep(2).setVelocity(randomFloat(0.6f, 0.8f));
				tomLoTrack.getStep(5).setActive(true);
				tomLoTrack.getStep(5).setVelocity(randomFloat(0.5f, 0.7f));
				tomLoTrack.getStep(9).setActive(true);
				tomLoTrack.getStep(9).setVelocity(randomFloat(0.6f, 0.8f));
				tomLoTrack.getStep(15).setActive(true);
				tomLoTrack.getStep(15).setVelocity(randomFloat(0.7f, 0.9f));

				splashTrack.getStep(0).setActive(true);
				splashTrack.getStep(0).setVelocity(randomFloat(0.7f, 0.9f));
				splashTrack.getStep(8).setActive(true);
				splashTrack.getStep(8).setVelocity(randomFloat(0.6f, 0.8f));
				break;

			case StyleType::Punk:
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(0.9f);
				kickTrack.getStep(4).setActive(true);
				kickTrack.getStep(4).setVelocity(0.85f);
				kickTrack.getStep(8).setActive(true);
				kickTrack.getStep(8).setVelocity(0.9f);
				kickTrack.getStep(12).setActive(true);
				kickTrack.getStep(12).setVelocity(0.8f);

				tomHiTrack.getStep(2).setActive(true);
				tomHiTrack.getStep(2).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(6).setActive(true);
				tomHiTrack.getStep(6).setVelocity(randomFloat(0.8f, 0.95f));
				tomHiTrack.getStep(10).setActive(true);
				tomHiTrack.getStep(10).setVelocity(randomFloat(0.7f, 0.9f));
				tomHiTrack.getStep(14).setActive(true);
				tomHiTrack.getStep(14).setVelocity(randomFloat(0.8f, 0.95f));

				tomLoTrack.getStep(1).setActive(true);
				tomLoTrack.getStep(1).setVelocity(randomFloat(0.6f, 0.8f));
				tomLoTrack.getStep(5).setActive(true);
				tomLoTrack.getStep(5).setVelocity(randomFloat(0.7f, 0.9f));
				tomLoTrack.getStep(9).setActive(true);
				tomLoTrack.getStep(9).setVelocity(randomFloat(0.6f, 0.8f));
				tomLoTrack.getStep(13).setActive(true);
				tomLoTrack.getStep(13).setVelocity(randomFloat(0.7f, 0.9f));

				chinaTrack.getStep(0).setActive(true);
				chinaTrack.getStep(0).setVelocity(randomFloat(0.8f, 1.0f));
				splashTrack.getStep(8).setActive(true);
				splashTrack.getStep(8).setVelocity(randomFloat(0.7f, 0.9f));
				break;
			}
		}
	}


	void StyleManager::addGhostNotes(Pattern& pattern, int track, float probability) {
		auto& targetTrack = pattern.getTrack(track);
		for (int i = 0; i < pattern.getLength(); ++i) {
			if (!targetTrack.getStep(i).isActive() && randomChance(probability * 0.3f)) {
				targetTrack.getStep(i).setActive(true);
				targetTrack.getStep(i).setVelocity(randomFloat(0.2f, 0.4f));
				targetTrack.getStep(i).setProbability(randomFloat(0.6f, 0.9f));
			}
		}
	}

	void StyleManager::addHiHatRideVariations(Pattern& pattern, float intensity, StyleType style) {
		auto& hihatTrack = pattern.getTrack(2);
		auto& openHihatTrack = pattern.getTrack(3);
		auto& rideTrack = pattern.getTrack(5);
		auto& rideBellTrack = pattern.getTrack(8);
		auto& hihatPedalTrack = pattern.getTrack(9);
		auto& splashTrack = pattern.getTrack(10);
		auto& chinaTrack = pattern.getTrack(11);

		for (int i = 0; i < pattern.getLength(); ++i) {
			hihatTrack.getStep(i).setActive(false);
			openHihatTrack.getStep(i).setActive(false);
			rideTrack.getStep(i).setActive(false);
			rideBellTrack.getStep(i).setActive(false);
			hihatPedalTrack.getStep(i).setActive(false);
			splashTrack.getStep(i).setActive(false);
			chinaTrack.getStep(i).setActive(false);
		}

		switch (style) {
		case StyleType::Rock:
			if (intensity <= 0.4f) {
				for (int i = 0; i < 16; i += 2) {
					hihatTrack.getStep(i).setActive(true);
					hihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
			}
			else if (intensity <= 0.6f) {
				for (int i = 0; i < 16; i += 2) {
					if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
				}
			}
			else if (intensity <= 0.8f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
				}
				hihatPedalTrack.getStep(2).setActive(true);
				hihatPedalTrack.getStep(2).setVelocity(0.4f);
				hihatPedalTrack.getStep(10).setActive(true);
				hihatPedalTrack.getStep(10).setVelocity(0.4f);
			}
			else {
				for (int i = 0; i < 16; ++i) {
					if (i == 0 && randomChance(0.6f)) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else if (i == 8 && randomChance(0.4f)) {
						chinaTrack.getStep(i).setActive(true);
						chinaTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
				}
				hihatPedalTrack.getStep(2).setActive(true);
				hihatPedalTrack.getStep(2).setVelocity(0.4f);
				hihatPedalTrack.getStep(10).setActive(true);
				hihatPedalTrack.getStep(10).setVelocity(0.4f);
			}
			break;

		case StyleType::Metal:
			if (intensity <= 0.4f) {
				for (int i = 0; i < 16; i += 2) {
					rideTrack.getStep(i).setActive(true);
					rideTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
			}
			else if (intensity <= 0.6f) {
				for (int i = 0; i < 16; i += 2) {
					if (i % 8 == 0 && randomChance(0.5f)) {
						rideBellTrack.getStep(i).setActive(true);
						rideBellTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else {
						rideTrack.getStep(i).setActive(true);
						rideTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
				}
			}
			else if (intensity <= 0.8f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 0 && randomChance(0.5f)) {
						chinaTrack.getStep(i).setActive(true);
						chinaTrack.getStep(i).setVelocity(randomFloat(0.8f, 1.0f));
					}
					else if (i % 4 == 0 && randomChance(0.4f)) {
						rideBellTrack.getStep(i).setActive(true);
						rideBellTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else {
						rideTrack.getStep(i).setActive(true);
						rideTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
				}
				hihatPedalTrack.getStep(4).setActive(true);
				hihatPedalTrack.getStep(4).setVelocity(0.3f);
				hihatPedalTrack.getStep(12).setActive(true);
				hihatPedalTrack.getStep(12).setVelocity(0.3f);
			}
			else {
				for (int i = 0; i < 16; ++i) {
					if (randomChance(0.7f)) {
						if (randomChance(0.6f)) {
							chinaTrack.getStep(i).setActive(true);
							chinaTrack.getStep(i).setVelocity(randomFloat(0.8f, 1.0f));
						}
						else {
							splashTrack.getStep(i).setActive(true);
							splashTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
						}
					}
				}
			}
			break;

		case StyleType::Jazz:
			if (intensity <= 0.5f) {
				for (int i = 0; i < 16; i += 2) {
					rideTrack.getStep(i).setActive(true);
					rideTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
				hihatPedalTrack.getStep(2).setActive(true);
				hihatPedalTrack.getStep(2).setVelocity(0.3f);
				hihatPedalTrack.getStep(10).setActive(true);
				hihatPedalTrack.getStep(10).setVelocity(0.3f);
			}
			else if (intensity <= 0.7f) {
				for (int i = 0; i < 16; i += 2) {
					if (i == 4 || i == 12) {
						rideBellTrack.getStep(i).setActive(true);
						rideBellTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else {
						rideTrack.getStep(i).setActive(true);
						rideTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
				}
				hihatPedalTrack.getStep(2).setActive(true);
				hihatPedalTrack.getStep(2).setVelocity(0.4f);
				hihatPedalTrack.getStep(6).setActive(true);
				hihatPedalTrack.getStep(6).setVelocity(0.3f);
				hihatPedalTrack.getStep(10).setActive(true);
				hihatPedalTrack.getStep(10).setVelocity(0.4f);
				hihatPedalTrack.getStep(14).setActive(true);
				hihatPedalTrack.getStep(14).setVelocity(0.3f);
			}
			else {
				for (int i = 0; i < 16; ++i) {
					if (i == 8 && randomChance(0.5f)) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else if (i % 4 == 0 && randomChance(0.4f)) {
						rideBellTrack.getStep(i).setActive(true);
						rideBellTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else if (i == 6 && randomChance(0.3f)) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
					else if (randomChance(0.8f)) {
						rideTrack.getStep(i).setActive(true);
						rideTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
				}
				for (int i = 2; i < 16; i += 4) {
					hihatPedalTrack.getStep(i).setActive(true);
					hihatPedalTrack.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
				}
			}
			break;

		case StyleType::Funk:
			if (intensity <= 0.5f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.7f));
					}
				}
			}
			else if (intensity <= 0.7f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.7f));
					}
				}
				hihatPedalTrack.getStep(1).setActive(true);
				hihatPedalTrack.getStep(1).setVelocity(0.4f);
				hihatPedalTrack.getStep(5).setActive(true);
				hihatPedalTrack.getStep(5).setVelocity(0.4f);
				hihatPedalTrack.getStep(9).setActive(true);
				hihatPedalTrack.getStep(9).setVelocity(0.4f);
				hihatPedalTrack.getStep(13).setActive(true);
				hihatPedalTrack.getStep(13).setVelocity(0.4f);
			}
			else {
				for (int i = 0; i < 16; ++i) {
					if (i == 14 && randomChance(0.6f)) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else if (i == 6) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.7f));
					}
				}
				hihatPedalTrack.getStep(1).setActive(true);
				hihatPedalTrack.getStep(1).setVelocity(0.4f);
				hihatPedalTrack.getStep(5).setActive(true);
				hihatPedalTrack.getStep(5).setVelocity(0.4f);
				hihatPedalTrack.getStep(9).setActive(true);
				hihatPedalTrack.getStep(9).setVelocity(0.4f);
				hihatPedalTrack.getStep(13).setActive(true);
				hihatPedalTrack.getStep(13).setVelocity(0.4f);
			}
			break;

		case StyleType::Electronic:
			if (intensity <= 0.5f) {
				for (int i = 0; i < 16; ++i) {
					hihatTrack.getStep(i).setActive(true);
					hihatTrack.getStep(i).setVelocity(0.3f + (i % 4 == 0 ? 0.2f : 0.0f));
				}
			}
			else if (intensity <= 0.7f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 2 || i == 10) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(0.3f + (i % 4 == 0 ? 0.2f : 0.0f));
					}
				}
			}
			else if (intensity <= 0.8f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 0 || i == 8) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else if (i == 2 || i == 10) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(0.3f + (i % 4 == 0 ? 0.2f : 0.0f));
					}
				}
			}
			else {
				for (int i = 0; i < 16; ++i) {
					if (i == 12 && randomChance(0.5f)) {
						chinaTrack.getStep(i).setActive(true);
						chinaTrack.getStep(i).setVelocity(randomFloat(0.8f, 1.0f));
					}
					else if (i == 0 || i == 8) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else if (i == 2 || i == 10) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(0.3f + (i % 4 == 0 ? 0.2f : 0.0f));
					}
				}
			}
			break;

		case StyleType::HipHop:
			if (intensity <= 0.4f) {
				for (int i = 2; i < 16; i += 4) {
					hihatTrack.getStep(i).setActive(true);
					hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
			}
			else if (intensity <= 0.6f) {
				for (int i = 2; i < 16; i += 4) {
					hihatTrack.getStep(i).setActive(true);
					hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
				for (int i = 1; i < 16; i += 4) {
					hihatTrack.getStep(i).setActive(true);
					hihatTrack.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
				}
			}
			else if (intensity <= 0.8f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 3 || i == 11) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else if (i % 2 == 1 || i % 4 == 2) {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
					}
				}
				hihatPedalTrack.getStep(6).setActive(true);
				hihatPedalTrack.getStep(6).setVelocity(0.3f);
				hihatPedalTrack.getStep(14).setActive(true);
				hihatPedalTrack.getStep(14).setVelocity(0.3f);
			}
			else {
				for (int i = 0; i < 16; ++i) {
					if (i == 0 && randomChance(0.5f)) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else if (i == 3 || i == 11) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else if (i % 2 == 1 || i % 4 == 2) {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
					}
				}
				hihatPedalTrack.getStep(6).setActive(true);
				hihatPedalTrack.getStep(6).setVelocity(0.3f);
				hihatPedalTrack.getStep(14).setActive(true);
				hihatPedalTrack.getStep(14).setVelocity(0.3f);
			}
			break;

		case StyleType::Latin:
			if (intensity <= 0.5f) {
				for (int i = 0; i < 16; i += 2) {
					if (i == 2 || i == 6 || i == 10 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
				}
			}
			else if (intensity <= 0.7f) {
				for (int i = 0; i < 16; i += 2) {
					if (i == 2 || i == 6 || i == 10 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
				}
				hihatPedalTrack.getStep(3).setActive(true);
				hihatPedalTrack.getStep(3).setVelocity(0.4f);
				hihatPedalTrack.getStep(7).setActive(true);
				hihatPedalTrack.getStep(7).setVelocity(0.4f);
				hihatPedalTrack.getStep(11).setActive(true);
				hihatPedalTrack.getStep(11).setVelocity(0.4f);
			}
			else if (intensity <= 0.8f) {
				for (int i = 0; i < 16; i += 2) {
					if (i == 8 && randomChance(0.6f)) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else if (i == 2 || i == 6 || i == 10 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
				}
				hihatPedalTrack.getStep(3).setActive(true);
				hihatPedalTrack.getStep(3).setVelocity(0.4f);
				hihatPedalTrack.getStep(7).setActive(true);
				hihatPedalTrack.getStep(7).setVelocity(0.4f);
				hihatPedalTrack.getStep(11).setActive(true);
				hihatPedalTrack.getStep(11).setVelocity(0.4f);
			}
			else {
				for (int i = 0; i < 16; ++i) {
					if (i % 4 == 0) {
						rideTrack.getStep(i).setActive(true);
						rideTrack.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
					}
					else if (i == 8) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
					else if (i == 2 || i == 6 || i == 10 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
				}
				hihatPedalTrack.getStep(3).setActive(true);
				hihatPedalTrack.getStep(3).setVelocity(0.4f);
				hihatPedalTrack.getStep(7).setActive(true);
				hihatPedalTrack.getStep(7).setVelocity(0.4f);
				hihatPedalTrack.getStep(11).setActive(true);
				hihatPedalTrack.getStep(11).setVelocity(0.4f);
			}
			break;

		case StyleType::Punk:
			if (intensity <= 0.5f) {
				for (int i = 0; i < 16; i += 2) {
					if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
					}
				}
			}
			else if (intensity <= 0.7f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
				}
			}
			else if (intensity <= 0.8f) {
				for (int i = 0; i < 16; ++i) {
					if (i == 4 || i == 12) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
				}
			}
			else {
				for (int i = 0; i < 16; ++i) {
					if (i == 0 || i == 8) {
						chinaTrack.getStep(i).setActive(true);
						chinaTrack.getStep(i).setVelocity(randomFloat(0.8f, 1.0f));
					}
					else if (i == 4 || i == 12) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else if (i == 6 || i == 14) {
						openHihatTrack.getStep(i).setActive(true);
						openHihatTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else {
						hihatTrack.getStep(i).setActive(true);
						hihatTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
					}
				}
			}
			break;
		}
	}

	void StyleManager::changeSnarePattern(Pattern& pattern, float intensity, StyleType style) {
		auto& snareTrack = pattern.getTrack(1);
		for (int i = 0; i < pattern.getLength(); ++i) {
			snareTrack.getStep(i).setActive(false);
		}
		switch (style) {
		case StyleType::Rock:
		case StyleType::Punk:
			if (intensity <= 0.3f) {
				snareTrack.getStep(8).setActive(true);
				snareTrack.getStep(8).setVelocity(randomFloat(0.8f, 0.9f));
			}
			else if (intensity <= 0.5f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(randomFloat(0.7f, 0.85f));
				snareTrack.getStep(10).setActive(true);
				snareTrack.getStep(10).setVelocity(randomFloat(0.7f, 0.85f));
			}
			else if (intensity <= 0.7f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(randomFloat(0.8f, 0.9f));
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(randomFloat(0.8f, 0.9f));
			}
			else if (intensity <= 0.85f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(randomFloat(0.8f, 0.9f));
				snareTrack.getStep(6).setActive(true);
				snareTrack.getStep(6).setVelocity(randomFloat(0.6f, 0.8f));
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(randomFloat(0.8f, 0.9f));
				snareTrack.getStep(14).setActive(true);
				snareTrack.getStep(14).setVelocity(randomFloat(0.6f, 0.8f));
			}
			else {
				for (int i = 2; i < 16; i += 2) {
					if (randomChance(0.8f)) {
						snareTrack.getStep(i).setActive(true);
						snareTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
				}
			}
			break;

		case StyleType::Metal:
			if (intensity <= 0.3f) {
				snareTrack.getStep(8).setActive(true);
				snareTrack.getStep(8).setVelocity(randomFloat(0.9f, 1.0f));
			}
			else if (intensity <= 0.5f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(randomFloat(0.85f, 0.95f));
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(randomFloat(0.85f, 0.95f));
			}
			else if (intensity <= 0.7f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(randomFloat(0.9f, 1.0f));
				snareTrack.getStep(5).setActive(true);
				snareTrack.getStep(5).setVelocity(randomFloat(0.7f, 0.8f));
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(randomFloat(0.9f, 1.0f));
				snareTrack.getStep(13).setActive(true);
				snareTrack.getStep(13).setVelocity(randomFloat(0.7f, 0.8f));
			}
			else if (intensity <= 0.85f) {
				for (int i = 2; i < 16; i += 2) {
					snareTrack.getStep(i).setActive(true);
					snareTrack.getStep(i).setVelocity(randomFloat(0.8f, 0.95f));
				}
			}
			else {
				for (int i = 1; i < 16; i += 2) {
					snareTrack.getStep(i).setActive(true);
					snareTrack.getStep(i).setVelocity(randomFloat(0.85f, 1.0f));
				}
			}
			break;

		case StyleType::Jazz:
			if (intensity <= 0.4f) {
				snareTrack.getStep(8).setActive(true);
				snareTrack.getStep(8).setVelocity(randomFloat(0.5f, 0.7f));
				addGhostNotes(pattern, 1, intensity * 0.6f);
			}
			else if (intensity <= 0.7f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(randomFloat(0.5f, 0.7f));
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(randomFloat(0.5f, 0.7f));
				addGhostNotes(pattern, 1, intensity * 0.8f);
			}
			else {
				for (int i = 1; i < 16; ++i) {
					if (randomChance(intensity * 0.4f)) {
						snareTrack.getStep(i).setActive(true);
						snareTrack.getStep(i).setVelocity(randomFloat(0.3f, 0.8f));
					}
				}
			}
			break;

		case StyleType::Electronic:
			if (intensity <= 0.4f) {
				snareTrack.getStep(8).setActive(true);
				snareTrack.getStep(8).setVelocity(0.8f);
			}
			else if (intensity <= 0.6f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(0.8f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(0.8f);
			}
			else if (intensity <= 0.8f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(0.9f);
				snareTrack.getStep(10).setActive(true);
				snareTrack.getStep(10).setVelocity(0.7f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(0.8f);
			}
			else {
				std::vector<int> breakPattern = { 4, 6, 10, 12, 14 };
				for (int step : breakPattern) {
					snareTrack.getStep(step).setActive(true);
					snareTrack.getStep(step).setVelocity(randomFloat(0.7f, 0.9f));
				}
			}
			break;

		case StyleType::HipHop:
			if (intensity <= 0.4f) {
				snareTrack.getStep(8).setActive(true);
				snareTrack.getStep(8).setVelocity(0.9f);
			}
			else if (intensity <= 0.6f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(0.9f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(0.9f);
			}
			else if (intensity <= 0.8f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(0.9f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(0.9f);
				addGhostNotes(pattern, 1, intensity * 0.7f);
			}
			else {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(1.0f);
				snareTrack.getStep(6).setActive(true);
				snareTrack.getStep(6).setVelocity(0.8f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(1.0f);
				snareTrack.getStep(14).setActive(true);
				snareTrack.getStep(14).setVelocity(0.8f);
			}
			break;

		case StyleType::Funk:
			if (intensity <= 0.3f) {
				snareTrack.getStep(8).setActive(true);
				snareTrack.getStep(8).setVelocity(0.9f);
				addGhostNotes(pattern, 1, 0.5f);
			}
			else if (intensity <= 0.6f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(0.9f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(0.85f);
				addGhostNotes(pattern, 1, intensity * 0.8f);
			}
			else {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(0.9f);
				snareTrack.getStep(6).setActive(true);
				snareTrack.getStep(6).setVelocity(0.3f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(0.85f);
				snareTrack.getStep(15).setActive(true);
				snareTrack.getStep(15).setVelocity(0.4f);
				addGhostNotes(pattern, 1, intensity * 0.9f);
			}
			break;

		case StyleType::Latin:
			if (intensity <= 0.4f) {
				snareTrack.getStep(8).setActive(true);
				snareTrack.getStep(8).setVelocity(0.8f);
			}
			else if (intensity <= 0.7f) {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(0.7f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(0.7f);
			}
			else {
				snareTrack.getStep(4).setActive(true);
				snareTrack.getStep(4).setVelocity(0.8f);
				snareTrack.getStep(6).setActive(true);
				snareTrack.getStep(6).setVelocity(0.6f);
				snareTrack.getStep(12).setActive(true);
				snareTrack.getStep(12).setVelocity(0.8f);
				snareTrack.getStep(14).setActive(true);
				snareTrack.getStep(14).setVelocity(0.6f);
			}
			break;
		}
	}

	void StyleManager::addRandomFills(Pattern& pattern, float intensity) {
		if (intensity > 0.7f && randomChance(intensity * 0.6f)) {
			auto& tomHiTrack = pattern.getTrack(6);
			auto& tomLoTrack = pattern.getTrack(7);

			int fillStart = 12 + static_cast<int>(randomFloat(0, 4));

			for (int i = fillStart; i < 16; ++i) {
				if (randomChance(0.6f)) {
					if (randomChance(0.5f)) {
						tomHiTrack.getStep(i).setActive(true);
						tomHiTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.9f));
					}
					else {
						tomLoTrack.getStep(i).setActive(true);
						tomLoTrack.getStep(i).setVelocity(randomFloat(0.6f, 0.9f));
					}
				}
			}
		}
	}

	float StyleManager::randomFloat(float min, float max) {
		std::uniform_real_distribution<float> dis(min, max);
		return dis(getRNG());
	}

	bool StyleManager::randomChance(float probability) {
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		return dis(getRNG()) < probability;
	}

}