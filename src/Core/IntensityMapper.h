#pragma once
#include "Pattern.h"

namespace BeatCrafter {

	class IntensityMapper {
	public:
		static Pattern applyIntensity(const Pattern& basePattern, float intensity);

	private:
		static void simplifyPattern(Pattern& pattern, float intensity);
		static void addGhostNotes(Pattern& pattern, float intensity);
		static void addComplexity(Pattern& pattern, float intensity);
		static void addMaximumIntensity(Pattern& pattern, float intensity);
	};

}