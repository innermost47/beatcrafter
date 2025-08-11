#pragma once
#include "Pattern.h"
#include <random>

namespace BeatCrafter {

	class StyleIntensityGenerator {
	public:
		static Pattern applyStyleBasedIntensity(const Pattern& basePattern,
			float intensity,
			StyleType style,
			uint32_t seed);

	private:
		static std::mt19937& getRNG();

		static void addGhostNotes(Pattern& pattern, int track, float probability);
		static void changeSnarePattern(Pattern& pattern, float intensity, StyleType style);
		static void addHiHatRideVariations(Pattern& pattern, float intensity, StyleType style);
		static void addRandomFills(Pattern& pattern, float intensity);
		static float randomFloat(float min, float max);
		static bool randomChance(float probability);
		static void applyBaseIntensityScaling(Pattern& pattern, float intensity);
		static void addSubtleVariations(Pattern& pattern, float intensity, StyleType style);
		static void addBreakMode(Pattern& pattern, float intensity, StyleType style);

	};

}