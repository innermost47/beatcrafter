#pragma once

namespace BeatCrafter {

	// General MIDI Drum Map (Channel 10)
	namespace GMDrum {
		constexpr int KICK_1 = 36;      // C1 - Bass Drum 1
		constexpr int KICK_2 = 35;      // B0 - Bass Drum 2
		constexpr int SNARE = 38;       // D1 - Snare 1
		constexpr int SNARE_2 = 40;     // E1 - Snare 2
		constexpr int CLAP = 39;        // D#1 - Hand Clap
		constexpr int HIHAT_CLOSED = 42;// F#1 - Closed Hi-Hat
		constexpr int HIHAT_OPEN = 46;  // A#1 - Open Hi-Hat
		constexpr int HIHAT_PEDAL = 44; // G#1 - Pedal Hi-Hat
		constexpr int CRASH_1 = 49;     // C#2 - Crash Cymbal 1
		constexpr int CRASH_2 = 57;     // A2 - Crash Cymbal 2
		constexpr int RIDE = 51;        // D#2 - Ride Cymbal 1
		constexpr int RIDE_BELL = 53;   // F2 - Ride Bell
		constexpr int TOM_HIGH = 50;    // D2 - High Tom
		constexpr int TOM_MID = 48;     // C2 - Hi-Mid Tom
		constexpr int TOM_LOW = 45;     // A1 - Low Tom
		constexpr int TOM_FLOOR_HI = 43;// G1 - High Floor Tom
		constexpr int TOM_FLOOR_LO = 41;// F1 - Low Floor Tom
		constexpr int COWBELL = 56;     // G#2 - Cowbell
		constexpr int TAMBOURINE = 54;  // F#2 - Tambourine
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