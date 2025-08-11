#pragma once
#include <juce_core/juce_core.h> /
#include "MidiConstants.h"
#include <array>
#include <vector>

namespace BeatCrafter {

	class Step {
	public:
		Step();

		bool isActive() const;
		void setActive(bool shouldBeActive);

		float getVelocity() const;
		void setVelocity(float v);

		float getMicroTiming() const;
		void setMicroTiming(float mt);

		float getProbability() const;
		void setProbability(float p);

	private:
		bool active;
		float velocity;      // 0.0 - 1.0
		float microTiming;   // -0.5 to +0.5 (in step units)
		float probability;   // 0.0 - 1.0 for random variations
	};

	class Track {
	public:
		Track(const juce::String& name, int midiNote);
		Step& getStep(int index);

		const Step& getStep(int index) const;
		void setLength(int numSteps);

		int getLength() const;

		juce::String getName() const;
		void setName(const juce::String& name);

		int getMidiNote() const;
		void setMidiNote(int note);

		void clear();

	private:
		juce::String trackName;
		int midiNoteNumber;
		std::vector<Step> steps;
	};

	class Pattern {
	public:
		Pattern(const juce::String& name = "Pattern 1");

		Track& getTrack(int index);
		const Track& getTrack(int index) const;

		int getNumTracks() const;

		juce::String getName() const;
		void setName(const juce::String& name);

		float getSwing() const;
		void setSwing(float s);

		int getCurrentStep() const;
		void setCurrentStep(int step);

		void advanceStep();

		int getLength() const;

		void setLength(int numSteps);

		TimeSignature getTimeSignature() const;
		void setTimeSignature(TimeSignature ts);
		void clear();
		void generateBasicRockPattern();
		void generateBasicMetalPattern();
		void generateBasicJazzPattern();
		void generateBasicFunkPattern();
		void generateBasicElectronicPattern(); // NOUVEAU
		void generateBasicHipHopPattern();     // NOUVEAU
		void generateBasicLatinPattern();      // NOUVEAU
		void generateBasicPunkPattern();       // NOUVEAU

	private:
		juce::String patternName;
		std::vector<Track> tracks;
		TimeSignature signature;
		float swing;
		int currentStep;
	};

} // namespace BeatCrafter