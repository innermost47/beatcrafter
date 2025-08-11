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

	void Pattern::generateBasicRockPattern() {
		clear();

		getTrack(0).getStep(0).setActive(true);
		getTrack(0).getStep(0).setVelocity(0.85f + (rand() % 15) / 100.0f);

		getTrack(1).getStep(8).setActive(true);
		getTrack(1).getStep(8).setVelocity(0.9f);

		getTrack(2).getStep(4).setActive(true);
		getTrack(2).getStep(4).setVelocity(0.4f);
		getTrack(2).getStep(4).setProbability(0.7f);
	}

	void Pattern::generateBasicMetalPattern() {
		clear();

		getTrack(0).getStep(0).setActive(true);
		getTrack(0).getStep(0).setVelocity(0.95f);
		getTrack(0).getStep(2).setActive(true);
		getTrack(0).getStep(2).setVelocity(0.7f);
		getTrack(0).getStep(2).setProbability(0.8f);

		getTrack(1).getStep(8).setActive(true);
		getTrack(1).getStep(8).setVelocity(0.95f);

		getTrack(5).getStep(6).setActive(true);
		getTrack(5).getStep(6).setVelocity(0.5f);
		getTrack(5).getStep(6).setProbability(0.7f);
	}

	void Pattern::generateBasicJazzPattern() {
		clear();

		setSwing(0.67f);

		getTrack(0).getStep(0).setActive(true);
		getTrack(0).getStep(0).setVelocity(0.4f + (rand() % 20) / 100.0f);

		getTrack(1).getStep(8).setActive(true);
		getTrack(1).getStep(8).setVelocity(0.55f);

		getTrack(5).getStep(2).setActive(true);
		getTrack(5).getStep(2).setVelocity(0.3f);
		getTrack(5).getStep(10).setActive(true);
		getTrack(5).getStep(10).setVelocity(0.35f);
		getTrack(5).getStep(10).setProbability(0.7f);
	}

	void Pattern::generateBasicFunkPattern() {
		clear();

		getTrack(0).getStep(0).setActive(true);
		getTrack(0).getStep(0).setVelocity(0.9f);
		getTrack(0).getStep(6).setActive(true);
		getTrack(0).getStep(6).setVelocity(0.6f);
		getTrack(0).getStep(6).setProbability(0.75f);

		getTrack(1).getStep(8).setActive(true);
		getTrack(1).getStep(8).setVelocity(0.85f);

		getTrack(2).getStep(4).setActive(true);
		getTrack(2).getStep(4).setVelocity(0.5f);
		getTrack(2).getStep(12).setActive(true);
		getTrack(2).getStep(12).setVelocity(0.4f);
		getTrack(2).getStep(12).setProbability(0.8f);
	}

	void Pattern::generateBasicElectronicPattern() {
		clear();

		getTrack(0).getStep(0).setActive(true);
		getTrack(0).getStep(0).setVelocity(0.9f);

		getTrack(1).getStep(8).setActive(true);
		getTrack(1).getStep(8).setVelocity(0.7f);
		getTrack(1).getStep(8).setProbability(0.8f);

		getTrack(2).getStep(4).setActive(true);
		getTrack(2).getStep(4).setVelocity(0.4f);
		getTrack(2).getStep(12).setActive(true);
		getTrack(2).getStep(12).setVelocity(0.5f);
	}

	void Pattern::generateBasicHipHopPattern() {
		clear();

		getTrack(0).getStep(0).setActive(true);
		getTrack(0).getStep(0).setVelocity(0.95f);

		getTrack(1).getStep(8).setActive(true);
		getTrack(1).getStep(8).setVelocity(0.9f);

		getTrack(2).getStep(4).setActive(true);
		getTrack(2).getStep(4).setVelocity(0.45f);
		getTrack(2).getStep(12).setActive(true);
		getTrack(2).getStep(12).setVelocity(0.35f);
		getTrack(2).getStep(12).setProbability(0.6f);
	}

	void Pattern::generateBasicLatinPattern() {
		clear();

		getTrack(0).getStep(0).setActive(true);
		getTrack(0).getStep(0).setVelocity(0.75f);
		getTrack(0).getStep(6).setActive(true);
		getTrack(0).getStep(6).setVelocity(0.5f);
		getTrack(0).getStep(6).setProbability(0.8f);

		getTrack(1).getStep(8).setActive(true);
		getTrack(1).getStep(8).setVelocity(0.8f);

		getTrack(3).getStep(4).setActive(true);
		getTrack(3).getStep(4).setVelocity(0.55f);
		getTrack(3).getStep(12).setActive(true);
		getTrack(3).getStep(12).setVelocity(0.5f);
		getTrack(3).getStep(12).setProbability(0.75f);
	}

	void Pattern::generateBasicPunkPattern() {
		clear();

		getTrack(0).getStep(0).setActive(true);
		getTrack(0).getStep(0).setVelocity(0.9f);

		getTrack(1).getStep(8).setActive(true);
		getTrack(1).getStep(8).setVelocity(1.0f);

		getTrack(2).getStep(4).setActive(true);
		getTrack(2).getStep(4).setVelocity(0.6f);
		getTrack(2).getStep(4).setProbability(0.7f);
		getTrack(2).getStep(12).setActive(true);
		getTrack(2).getStep(12).setVelocity(0.65f);
	}

}