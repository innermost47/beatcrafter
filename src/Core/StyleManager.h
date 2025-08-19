#pragma once
#include <juce_core/juce_core.h> 
#include "Pattern.h"
#include "MidiConstants.h"
#include <random>

namespace BeatCrafter {
	class Pattern;
	class StyleManager {
	public:
		static void generateBasicPattern(Pattern& pattern, StyleType style);
		static Pattern applyIntensity(const Pattern& basePattern, float intensity, StyleType style, uint32_t seed);
		static void applyComplexityToPattern(Pattern& pattern, StyleType style, float complexity);

	private:
		static void generateRockPattern(Pattern& pattern);
		static void generateMetalPattern(Pattern& pattern);
		static void generateJazzPattern(Pattern& pattern);
		static void generateFunkPattern(Pattern& pattern);
		static void generateElectronicPattern(Pattern& pattern);
		static void generateHipHopPattern(Pattern& pattern);
		static void generateLatinPattern(Pattern& pattern);
		static void generatePunkPattern(Pattern& pattern);


		static std::mt19937& getRNG();
		static void applyBaseIntensityScaling(Pattern& pattern, float intensity);
		static void addSubtleVariations(Pattern& pattern, float intensity, StyleType style);
		static void changeSnarePattern(Pattern& pattern, float intensity, StyleType style);
		static void addHiHatRideVariations(Pattern& pattern, float intensity, StyleType style);
		static void addGhostNotes(Pattern& pattern, int track, float probability);
		static void addRandomFills(Pattern& pattern, float intensity);
		static void addBreakMode(Pattern& pattern, float intensity, StyleType style);
		static float randomFloat(float min, float max);
		static bool randomChance(float probability);
		static void addSoftIntro(Pattern& pattern, float intensity, StyleType style);
		static void generatePunkIntro(BeatCrafter::Pattern& pattern, float intensity);
	};

}