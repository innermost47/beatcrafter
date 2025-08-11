#include "StyleIntensityGenerator.h"
#include <algorithm>

namespace BeatCrafter {

	std::mt19937& StyleIntensityGenerator::getRNG() {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return gen;
	}

	Pattern StyleIntensityGenerator::applyStyleBasedIntensity(const Pattern& basePattern,
		float intensity,
		StyleType style,
		uint32_t seed) {

		getRNG().seed(seed);

		Pattern result = basePattern;

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

		return result;
	}

	void StyleIntensityGenerator::addGhostNotes(Pattern& pattern, int track, float probability) {
		auto& targetTrack = pattern.getTrack(track);
		for (int i = 0; i < pattern.getLength(); ++i) {
			if (!targetTrack.getStep(i).isActive() && randomChance(probability * 0.3f)) {
				targetTrack.getStep(i).setActive(true);
				targetTrack.getStep(i).setVelocity(randomFloat(0.2f, 0.4f));
				targetTrack.getStep(i).setProbability(randomFloat(0.6f, 0.9f));
			}
		}
	}

	void StyleIntensityGenerator::addHiHatRideVariations(Pattern& pattern, float intensity, StyleType style) {
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
					if (i == 0) {
						chinaTrack.getStep(i).setActive(true);
						chinaTrack.getStep(i).setVelocity(randomFloat(0.8f, 1.0f));
					}
					else if (i == 12 && randomChance(0.4f)) {
						splashTrack.getStep(i).setActive(true);
						splashTrack.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
					else {
						rideTrack.getStep(i).setActive(true);
						rideTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
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

	void StyleIntensityGenerator::changeSnarePattern(Pattern& pattern, float intensity, StyleType style) {
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

	void StyleIntensityGenerator::addRandomFills(Pattern& pattern, float intensity) {
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

	float StyleIntensityGenerator::randomFloat(float min, float max) {
		std::uniform_real_distribution<float> dis(min, max);
		return dis(getRNG());
	}

	bool StyleIntensityGenerator::randomChance(float probability) {
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		return dis(getRNG()) < probability;
	}
}