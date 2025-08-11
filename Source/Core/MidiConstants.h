#pragma once

namespace BeatCrafter {

	// General MIDI Drum Map (Channel 10)
	namespace GMDrum {
		constexpr int KICK_1 = 36;
		constexpr int KICK_2 = 35;
		constexpr int SNARE = 38;
		constexpr int SNARE_2 = 40;
		constexpr int CLAP = 39;
		constexpr int HIHAT_CLOSED = 42;
		constexpr int HIHAT_OPEN = 46;
		constexpr int HIHAT_PEDAL = 44;
		constexpr int CRASH_1 = 49;
		constexpr int CRASH_2 = 57;
		constexpr int RIDE = 51;
		constexpr int RIDE_BELL = 53;
		constexpr int TOM_HIGH = 50;
		constexpr int TOM_MID = 48;
		constexpr int TOM_LOW = 45;
		constexpr int TOM_FLOOR_HI = 43;
		constexpr int TOM_FLOOR_LO = 41;
		constexpr int COWBELL = 56;
		constexpr int TAMBOURINE = 54;
		constexpr int CHINA_CYMBAL = 52;    // NOUVEAU
		constexpr int SPLASH_CYMBAL = 55;   // NOUVEAU
	}

	// Style types
	enum class StyleType {
		Rock,
		Metal,
		Jazz,
		Funk,
		Electronic,
		HipHop,
		Latin,
		Punk
	};

	// Time signatures
	struct TimeSignature {
		int numerator;
		int denominator;

		int getStepsPerBar() const {
			return (numerator * 4) / denominator * 4; // 16th notes
		}
	};

} // namespace BeatCrafter