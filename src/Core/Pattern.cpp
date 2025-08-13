#include "Pattern.h"

namespace BeatCrafter {

	Step::Step()
		: active(false),
		velocity(0.8f),
		microTiming(0.0f),
		probability(1.0f) {
	}

	bool Step::isActive() const {
		return active;
	}

	void Step::setActive(bool shouldBeActive) {
		active = shouldBeActive;
	}

	float Step::getVelocity() const {
		return velocity;
	}

	void Step::setVelocity(float v) {
		velocity = juce::jlimit(0.0f, 1.0f, v);
	}

	float Step::getMicroTiming() const {
		return microTiming;
	}

	void Step::setMicroTiming(float mt) {
		microTiming = juce::jlimit(-0.5f, 0.5f, mt);
	}

	float Step::getProbability() const {
		return probability;
	}

	void Step::setProbability(float p) {
		probability = juce::jlimit(0.0f, 1.0f, p);
	}

	Track::Track(const juce::String& name, int midiNote)
		: trackName(name),
		midiNoteNumber(midiNote) {
		steps.resize(16);
	}

	Step& Track::getStep(int index) {
		jassert(index >= 0 && index < steps.size());
		return steps[index];
	}

	const Step& Track::getStep(int index) const {
		jassert(index >= 0 && index < steps.size());
		return steps[index];
	}

	void Track::setLength(int numSteps) {
		steps.resize(numSteps);
	}

	int Track::getLength() const {
		return static_cast<int>(steps.size());
	}

	juce::String Track::getName() const {
		return trackName;
	}

	void Track::setName(const juce::String& name) {
		trackName = name;
	}

	int Track::getMidiNote() const {
		return midiNoteNumber;
	}

	void Track::setMidiNote(int note) {
		midiNoteNumber = note;
	}

	void Track::clear() {
		for (auto& step : steps) {
			step.setActive(false);
			step.setVelocity(0.8f);
			step.setMicroTiming(0.0f);
			step.setProbability(1.0f);
		}
	}

	Pattern::Pattern(const juce::String& name)
		: patternName(name),
		swing(0.0f),
		currentStep(0) {

		tracks.emplace_back("Kick", GMDrum::KICK_1);
		tracks.emplace_back("Snare", GMDrum::SNARE);
		tracks.emplace_back("Hi-Hat", GMDrum::HIHAT_CLOSED);
		tracks.emplace_back("Open HH", GMDrum::HIHAT_OPEN);
		tracks.emplace_back("Crash", GMDrum::CRASH_1);
		tracks.emplace_back("Ride", GMDrum::RIDE);
		tracks.emplace_back("Tom Hi", GMDrum::TOM_HIGH);
		tracks.emplace_back("Tom Low", GMDrum::TOM_LOW);
		tracks.emplace_back("Ride Bell", GMDrum::RIDE_BELL);
		tracks.emplace_back("HH Pedal", GMDrum::HIHAT_PEDAL);
		tracks.emplace_back("Splash", GMDrum::CRASH_2);
		tracks.emplace_back("China", 52);

		signature = { 4, 4 };
	}

	Track& Pattern::getTrack(int index) {
		jassert(index >= 0 && index < tracks.size());
		return tracks[index];
	}

	const Track& Pattern::getTrack(int index) const {
		jassert(index >= 0 && index < tracks.size());
		return tracks[index];
	}

	int Pattern::getNumTracks() const {
		return static_cast<int>(tracks.size());
	}

	juce::String Pattern::getName() const {
		return patternName;
	}

	void Pattern::setName(const juce::String& name) {
		patternName = name;
	}

	float Pattern::getSwing() const {
		return swing;
	}

	void Pattern::setSwing(float s) {
		swing = juce::jlimit(0.0f, 0.75f, s);
	}

	int Pattern::getCurrentStep() const {
		return currentStep;
	}

	void Pattern::setCurrentStep(int step) {
		currentStep = step;
	}

	void Pattern::advanceStep() {
		currentStep = (currentStep + 1) % getLength();
	}

	int Pattern::getLength() const {
		return tracks.empty() ? 16 : tracks[0].getLength();
	}

	void Pattern::setLength(int numSteps) {
		for (auto& track : tracks) {
			track.setLength(numSteps);
		}
	}

	TimeSignature Pattern::getTimeSignature() const {
		return signature;
	}

	void Pattern::setTimeSignature(TimeSignature ts) {
		signature = ts;
		setLength(ts.getStepsPerBar());
	}

	void Pattern::clear() {
		for (auto& track : tracks) {
			track.clear();
		}
		currentStep = 0;
	}
}