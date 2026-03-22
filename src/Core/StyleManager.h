#pragma once
#include <juce_core/juce_core.h>
#include "Pattern.h"
#include "MidiConstants.h"
#include <random>

namespace BeatCrafter
{
	class Pattern;

	class StyleManager
	{
	public:
		static void generateBasicPattern(Pattern &pattern, StyleType style);
		static Pattern applyIntensity(const Pattern &basePattern, float intensity,
									  StyleType style, uint32_t seed);
		static void applyComplexityToPattern(Pattern &pattern, StyleType style,
											 float complexity, uint32_t seed);

	private:
		static void generateRockPattern(Pattern &pattern);
		static void generateRockSnare(Pattern &pattern, float intensity);
		static void generateRockHiHat(Pattern &pattern, float intensity);
		static void generateRockBreak(Pattern &pattern, float intensity);

		static void generateMetalPattern(Pattern &pattern);
		static void generateMetalSnare(Pattern &pattern, float intensity);
		static void generateMetalHiHat(Pattern &pattern, float intensity);
		static void generateMetalBreak(Pattern &pattern, float intensity);

		static void generateJazzPattern(Pattern &pattern);
		static void generateJazzSnare(Pattern &pattern, float intensity);
		static void generateJazzHiHat(Pattern &pattern, float intensity);
		static void generateJazzBreak(Pattern &pattern, float intensity);

		static void generateFunkPattern(Pattern &pattern);
		static void generateFunkSnare(Pattern &pattern, float intensity);
		static void generateFunkHiHat(Pattern &pattern, float intensity);
		static void generateFunkBreak(Pattern &pattern, float intensity);

		static void generateElectronicPattern(Pattern &pattern);
		static void generateElectronicSnare(Pattern &pattern, float intensity);
		static void generateElectronicHiHat(Pattern &pattern, float intensity);
		static void generateElectronicBreak(Pattern &pattern, float intensity);

		static void generateHipHopPattern(Pattern &pattern);
		static void generateHipHopSnare(Pattern &pattern, float intensity);
		static void generateHipHopHiHat(Pattern &pattern, float intensity);
		static void generateHipHopBreak(Pattern &pattern, float intensity);

		static void generateLatinPattern(Pattern &pattern);
		static void generateLatinSnare(Pattern &pattern, float intensity);
		static void generateLatinHiHat(Pattern &pattern, float intensity);
		static void generateLatinBreak(Pattern &pattern, float intensity);

		static void generatePunkPattern(Pattern &pattern);
		static void generatePunkSnare(Pattern &pattern, float intensity);
		static void generatePunkHiHat(Pattern &pattern, float intensity);
		static void generatePunkBreak(Pattern &pattern, float intensity);
		static void generatePunkIntro(Pattern &pattern, float intensity);

		static std::mt19937 &getRNG();
		static float randomFloat(float min, float max);
		static bool randomChance(float probability);

		static void applyBaseIntensityScaling(Pattern &pattern, float intensity);
		static void addSubtleVariations(Pattern &pattern, float intensity, StyleType style);
		static void addGhostNotes(Pattern &pattern, int track, float probability);
		static void addRandomFills(Pattern &pattern, float intensity);
		static void addSoftIntro(Pattern &pattern, float intensity, StyleType style);
		static void clearCymbalsAndToms(Pattern &pattern);

		static void changeSnarePattern(Pattern &pattern, float intensity, StyleType style);
		static void addHiHatRideVariations(Pattern &pattern, float intensity, StyleType style);
		static void addBreakMode(Pattern &pattern, float intensity, StyleType style);
	};
}