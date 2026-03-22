#include "PatternEngine.h"
#include <random>

namespace BeatCrafter
{

	PatternEngine::PatternEngine()
	{
		std::random_device rd;

		StyleType defaultStyles[8] = {
			StyleType::Rock,
			StyleType::Metal,
			StyleType::Jazz,
			StyleType::Funk,
			StyleType::Electronic,
			StyleType::HipHop,
			StyleType::Latin,
			StyleType::Punk };

		for (int i = 0; i < 8; ++i)
		{
			slotStyles[i] = defaultStyles[i];
			slotRandomSeeds[i] = rd();

			auto pattern = std::make_unique<Pattern>("Slot " + juce::String(i + 1));
			StyleManager::generateBasicPattern(*pattern, defaultStyles[i]);
			StyleManager::applyComplexityToPattern(*pattern, defaultStyles[i],
				0.5f, slotRandomSeeds[i]);
			slots[i] = std::move(pattern);
		}
	}

	void PatternEngine::resetToStart()
	{
		for (auto& slot : slots)
		{
			if (slot)
			{
				slot->setCurrentStep(0);
			}
		}
		isPlaying = true;
	}

	void PatternEngine::start()
	{
		if (!isPlaying)
		{
			resetToStart();
		}
	}

	void PatternEngine::stop()
	{
		isPlaying = false;
		for (auto& slot : slots)
		{
			if (slot)
				slot->setCurrentStep(0);
		}
	}

	void PatternEngine::generateNewPatternForSlot(int slot, StyleType style, float complexity)
	{
		if (slot < 0 || slot >= 8)
			return;

		auto newPattern = std::make_unique<Pattern>("Generated " + juce::String(slot + 1));
		StyleManager::generateBasicPattern(*newPattern, style);
		StyleManager::applyComplexityToPattern(*newPattern, style,
			juce::jmax(0.1f, complexity),
			slotRandomSeeds[slot]);

		loadPatternToSlot(std::move(newPattern), slot);
	}

	void PatternEngine::loadPatternToSlot(std::unique_ptr<Pattern> pattern, int slot)
	{
		if (slot >= 0 && slot < 8)
		{
			slots[slot] = std::move(pattern);
		}
	}

	void PatternEngine::switchToSlot(int slot, bool immediate, float intensity)
	{
		if (slot < 0 || slot >= 8 || !slots[slot])
			return;

		if (intensity >= 0.0f)
			currentIntensity = intensity;

		intensifiedPatternCache = applyIntensity(*slots[slot], currentIntensity);
		intensityCacheValid = true;

		if (immediate)
		{
			if (isPlaying)
				pendingImmediateSlot.store(slot);
			else
			{
				activeSlot = slot;
				queuedSlot = -1;
			}
		}
		else
			queuedSlot = slot;
	}

	void PatternEngine::processBlock(juce::MidiBuffer& midiMessages,
		int /*numSamples*/,
		double sampleRate,
		const juce::AudioPlayHead::PositionInfo& posInfo)
	{
		midiMessages.clear();
		if (!slots[activeSlot])
			return;
		bool isPlayingDAW = posInfo.getIsPlaying();
		double bpm = posInfo.getBpm().orFallback(120.0);
		double ppqPosition = posInfo.getPpqPosition().orFallback(0.0);
		int currentMeasure = static_cast<int>(ppqPosition / 4.0);
		double beatsPerSecond = bpm / 60.0;
		double stepsPerBeat = perfParams.tripletMode ? 3.0 : 4.0;
		double stepsPerSecond = beatsPerSecond * stepsPerBeat;
		samplesPerStep = static_cast<int>(sampleRate / stepsPerSecond);
		auto& pattern = *slots[activeSlot];
		int patternLength = pattern.getLength();
		double ppqPerStep = perfParams.tripletMode ? (1.0 / 3.0) : 0.25;
		int effectiveLength = perfParams.tripletMode ? 12 : patternLength;
		int currentStepFromPPQ = static_cast<int>(ppqPosition / ppqPerStep) % effectiveLength;
		updateSurpriseMe(currentMeasure, ppqPosition);
		if (currentStepFromPPQ != pattern.getCurrentStep() ||
			(currentStepFromPPQ == 0 && ppqPosition < 0.1))
		{
			pattern.setCurrentStep(currentStepFromPPQ);
			int pending = pendingImmediateSlot.exchange(-1);
			if (pending >= 0)
			{
				sendAllNotesOff(midiMessages);
				activeSlot = pending;
				queuedSlot = -1;
				intensityCacheValid = false;
			}
			else if (queuedSlot >= 0 && currentStepFromPPQ == 0)
			{
				sendAllNotesOff(midiMessages);
				activeSlot = queuedSlot;
				queuedSlot = -1;
				intensityCacheValid = false;
			}
			if (isPlayingDAW)
				generateMidiForStep(midiMessages, 0, *slots[activeSlot], currentStepFromPPQ);
		}
		lastPpqPosition = ppqPosition;
		int displaySlot = pendingImmediateSlot.load() >= 0
			? pendingImmediateSlot.load()
			: activeSlot;
		if (!intensityCacheValid && slots[displaySlot])
		{
			intensifiedPatternCache = applyIntensity(*slots[displaySlot], currentIntensity);
			intensityCacheValid = true;
		}
	}

	void PatternEngine::applyHumanization(Pattern& pattern, int stepIndex)
	{
		static std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> velDist(
			-perfParams.humanizeAmount, perfParams.humanizeAmount);
		std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);

		for (int trackIdx = 0; trackIdx < pattern.getNumTracks(); ++trackIdx)
		{
			auto& step = pattern.getTrack(trackIdx).getStep(stepIndex);
			if (!step.isActive())
				continue;

			float newVel = juce::jlimit(0.15f, 1.0f,
				step.getVelocity() + velDist(rng));
			step.setVelocity(newVel);

			float omitProb = perfParams.omitChance;

			if (trackIdx == 0)
			{
				if (stepIndex == 0 || stepIndex == 8)
					omitProb *= 0.1f;
				else
					omitProb *= 0.5f;
			}
			else if (trackIdx == 1)
			{
				if (stepIndex == 4 || stepIndex == 12)
					omitProb *= 0.15f;
				else
					omitProb *= 0.6f;
			}
			else if (trackIdx == 2)
			{
				omitProb *= 1.5f;
			}
			else
			{
				omitProb *= 2.5f;
			}

			if (chanceDist(rng) < omitProb)
				step.setActive(false);
		}
	}

	void PatternEngine::sendAllNotesOff(juce::MidiBuffer& midiMessages)
	{
		for (int ch = 1; ch <= 16; ++ch)
		{
			midiMessages.addEvent(juce::MidiMessage::allNotesOff(ch), 0);
			midiMessages.addEvent(juce::MidiMessage::allSoundOff(ch), 0);
		}
	}

	void PatternEngine::addLiveJamElements(Pattern& pattern, int stepIndex, float /*intensity*/)
	{
		stepsSinceLastJam++;

		float jamChance = currentLiveJamIntensity * currentLiveJamIntensity * 0.4f;

		if (liveJamRandom.nextFloat() < jamChance)
		{
			stepsSinceLastJam = 0;

			int maxElement = currentLiveJamIntensity > 0.7f ? 10 : currentLiveJamIntensity > 0.4f ? 8
				: 6;
			int elementType = liveJamRandom.nextInt(maxElement);

			switch (elementType)
			{
			case 0:
				if (!pattern.getTrack(0).getStep(stepIndex).isActive())
				{
					pattern.getTrack(0).getStep(stepIndex).setActive(true);
					pattern.getTrack(0).getStep(stepIndex).setVelocity(
						0.4f + currentLiveJamIntensity * 0.5f);
					pattern.getTrack(0).getStep(stepIndex).setProbability(
						0.6f + currentLiveJamIntensity * 0.3f);
				}
				break;

			case 1:
				if (!pattern.getTrack(1).getStep(stepIndex).isActive())
				{
					pattern.getTrack(1).getStep(stepIndex).setActive(true);
					pattern.getTrack(1).getStep(stepIndex).setVelocity(
						0.2f + liveJamRandom.nextFloat() * currentLiveJamIntensity * 0.4f);
					pattern.getTrack(1).getStep(stepIndex).setProbability(
						0.5f + currentLiveJamIntensity * 0.4f);
				}
				break;

			case 2:
				if (stepIndex % 4 == 0 || stepIndex % 8 == 0)
				{
					pattern.getTrack(4).getStep(stepIndex).setActive(true);
					pattern.getTrack(4).getStep(stepIndex).setVelocity(
						0.5f + currentLiveJamIntensity * 0.4f);
					pattern.getTrack(4).getStep(stepIndex).setProbability(
						0.4f + currentLiveJamIntensity * 0.4f);
				}
				break;

			case 3:
				pattern.getTrack(3).getStep(stepIndex).setActive(true);
				pattern.getTrack(3).getStep(stepIndex).setVelocity(
					0.3f + currentLiveJamIntensity * 0.4f);
				pattern.getTrack(3).getStep(stepIndex).setProbability(
					0.6f + currentLiveJamIntensity * 0.3f);
				break;

			case 4:
				if (currentLiveJamIntensity > 0.4f && stepIndex % 4 == 0)
				{
					pattern.getTrack(10).getStep(stepIndex).setActive(true);
					pattern.getTrack(10).getStep(stepIndex).setVelocity(
						0.5f + currentLiveJamIntensity * 0.4f);
					pattern.getTrack(10).getStep(stepIndex).setProbability(
						0.3f + currentLiveJamIntensity * 0.5f);
				}
				break;

			case 5:
				if (currentLiveJamIntensity > 0.3f)
				{
					pattern.getTrack(5).getStep(stepIndex).setActive(true);
					pattern.getTrack(5).getStep(stepIndex).setVelocity(
						0.3f + currentLiveJamIntensity * 0.5f);
					pattern.getTrack(5).getStep(stepIndex).setProbability(
						0.5f + currentLiveJamIntensity * 0.4f);
				}
				break;

			case 6:
				if (currentLiveJamIntensity > 0.4f && stepIndex % 2 == 0)
				{
					pattern.getTrack(0).getStep(stepIndex).setActive(true);
					pattern.getTrack(0).getStep(stepIndex).setVelocity(
						0.6f + liveJamRandom.nextFloat() * currentLiveJamIntensity * 0.3f);
					pattern.getTrack(0).getStep(stepIndex).setProbability(
						0.5f + currentLiveJamIntensity * 0.4f);
				}
				break;

			case 7:
				if (currentLiveJamIntensity > 0.4f)
				{
					pattern.getTrack(1).getStep(stepIndex).setActive(true);
					pattern.getTrack(1).getStep(stepIndex).setVelocity(
						0.3f + liveJamRandom.nextFloat() * 0.4f);
					pattern.getTrack(1).getStep(stepIndex).setProbability(
						0.4f + currentLiveJamIntensity * 0.5f);
					if (stepIndex < 15)
					{
						pattern.getTrack(1).getStep(stepIndex + 1).setActive(true);
						pattern.getTrack(1).getStep(stepIndex + 1).setVelocity(0.15f + liveJamRandom.nextFloat() * 0.2f);
						pattern.getTrack(1).getStep(stepIndex + 1).setProbability(0.4f + currentLiveJamIntensity * 0.3f);
					}
				}
				break;

			case 8:
				if (currentLiveJamIntensity > 0.7f)
				{
					pattern.getTrack(9).getStep(stepIndex).setActive(true);
					pattern.getTrack(9).getStep(stepIndex).setVelocity(
						0.4f + liveJamRandom.nextFloat() * currentLiveJamIntensity * 0.4f);
					pattern.getTrack(9).getStep(stepIndex).setProbability(
						0.6f + currentLiveJamIntensity * 0.3f);
				}
				break;

			case 9:
				if (currentLiveJamIntensity > 0.7f)
				{
					pattern.getTrack(11).getStep(stepIndex).setActive(true);
					pattern.getTrack(11).getStep(stepIndex).setVelocity(
						0.5f + liveJamRandom.nextFloat() * currentLiveJamIntensity * 0.4f);
					pattern.getTrack(11).getStep(stepIndex).setProbability(
						0.3f + currentLiveJamIntensity * 0.5f);
				}
				break;
			}
		}

		if (stepIndex >= 12 && currentLiveJamIntensity > 0.3f &&
			liveJamRandom.nextFloat() < currentLiveJamIntensity * 0.5f)
		{
			int tomTrack = liveJamRandom.nextBool() ? 6 : 7;
			pattern.getTrack(tomTrack).getStep(stepIndex).setActive(true);
			pattern.getTrack(tomTrack).getStep(stepIndex).setVelocity(
				0.4f + liveJamRandom.nextFloat() * currentLiveJamIntensity * 0.5f);
			pattern.getTrack(tomTrack).getStep(stepIndex).setProbability(
				0.6f + currentLiveJamIntensity * 0.3f);

			if (currentLiveJamIntensity > 0.8f && stepIndex < 15)
			{
				int otherTom = (tomTrack == 6) ? 7 : 6;
				pattern.getTrack(otherTom).getStep(stepIndex + 1).setActive(true);
				pattern.getTrack(otherTom).getStep(stepIndex + 1).setVelocity(0.5f + liveJamRandom.nextFloat() * 0.3f);
				pattern.getTrack(otherTom).getStep(stepIndex + 1).setProbability(0.7f + currentLiveJamIntensity * 0.2f);
			}
		}

		if (currentLiveJamIntensity > 0.6f &&
			liveJamRandom.nextFloat() < currentLiveJamIntensity * 0.25f)
		{
			if (!pattern.getTrack(2).getStep(stepIndex).isActive())
			{
				pattern.getTrack(2).getStep(stepIndex).setActive(true);
				pattern.getTrack(2).getStep(stepIndex).setVelocity(
					0.2f + currentLiveJamIntensity * 0.3f);
				pattern.getTrack(2).getStep(stepIndex).setProbability(
					0.7f + currentLiveJamIntensity * 0.2f);
			}
		}

		if (currentLiveJamIntensity > 0.9f &&
			liveJamRandom.nextFloat() < 0.3f)
		{
			int randomTrack = liveJamRandom.nextInt(pattern.getNumTracks());
			if (!pattern.getTrack(randomTrack).getStep(stepIndex).isActive())
			{
				pattern.getTrack(randomTrack).getStep(stepIndex).setActive(true);
				pattern.getTrack(randomTrack).getStep(stepIndex).setVelocity(0.3f + liveJamRandom.nextFloat() * 0.5f);
				pattern.getTrack(randomTrack).getStep(stepIndex).setProbability(0.5f + liveJamRandom.nextFloat() * 0.4f);
			}
		}
	}

	void PatternEngine::generateMidiForStep(juce::MidiBuffer& midiMessages,
		int samplePosition,
		const Pattern& pattern,
		int stepIndex)
	{
		cachedIntensifiedPattern = applyIntensity(pattern, currentIntensity);

		if (liveJamMode && currentLiveJamIntensity > 0.1f)
			addLiveJamElements(cachedIntensifiedPattern, stepIndex, currentIntensity);

		if (perfParams.humanizeEnabled)
			applyHumanization(cachedIntensifiedPattern, stepIndex);

		static std::mt19937 gen(std::random_device{}());
		static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

		for (int trackIdx = 0; trackIdx < cachedIntensifiedPattern.getNumTracks(); ++trackIdx)
		{
			const auto& track = cachedIntensifiedPattern.getTrack(trackIdx);
			const auto& step = track.getStep(stepIndex);

			if (step.isActive() && dis(gen) <= step.getProbability())
			{
				int midiNote = track.getMidiNote();
				int velocity = static_cast<int>(step.getVelocity() * 127.0f);
				int timingOffset = static_cast<int>(step.getMicroTiming() * samplesPerStep * 0.1f);
				int finalSamplePos = juce::jmax(0, samplePosition + timingOffset);

				midiMessages.addEvent(
					juce::MidiMessage::noteOn(10, midiNote, (juce::uint8)velocity),
					finalSamplePos);
				midiMessages.addEvent(
					juce::MidiMessage::noteOff(10, midiNote),
					juce::jmin(finalSamplePos + (int)(0.1 * 44100),
						samplePosition + samplesPerStep - 1));
			}
		}
	}

	Pattern PatternEngine::applyIntensity(const Pattern& basePattern, float intensity) const
	{
		StyleType currentStyle = getSlotStyle(activeSlot);
		uint32_t currentSeed = slotRandomSeeds[activeSlot];
		return StyleManager::applyIntensity(basePattern, intensity, currentStyle, currentSeed);
	}

	void PatternEngine::generateNewPattern(StyleType style, float complexity)
	{
		if (!slots[activeSlot])
			slots[activeSlot] = std::make_unique<Pattern>("Generated");

		auto& pattern = *slots[activeSlot];
		StyleManager::generateBasicPattern(pattern, style);

		StyleManager::applyComplexityToPattern(pattern, style,
			juce::jmax(0.1f, complexity),
			slotRandomSeeds[activeSlot]);

		intensityCacheValid = false;
	}

	void PatternEngine::updateSurpriseMe(int currentMeasure, double ppqPosition)
	{
		if (!perfParams.surpriseMeEnabled)
			return;

		if (std::abs(currentIntensity - surpriseMeCenter) > 0.2f)
			surpriseMeCenter = currentIntensity;

		if (currentMeasure != lastSurpriseMeMeasure)
		{
			lastSurpriseMeMeasure = currentMeasure;

			if (currentMeasure % 8 == 0)
			{
				static std::mt19937 rng(std::random_device{}());
				std::uniform_real_distribution<float> freqDist(0.4f, 1.2f);
				std::uniform_real_distribution<float> ampDist(0.03f, 0.10f);

				surpriseMeTargetFreq = freqDist(rng);
				surpriseMeTargetAmplitude = ampDist(rng);

				if (surpriseMeCenter < 0.2f)
					surpriseMeCenter += 0.1f;
				else if (surpriseMeCenter > 0.8f)
					surpriseMeCenter -= 0.1f;
			}
		}

		surpriseMeFreq += (surpriseMeTargetFreq - surpriseMeFreq) * 0.15f;
		surpriseMeAmplitude += (surpriseMeTargetAmplitude - surpriseMeAmplitude) * 0.15f;

		float phaseIncrement = (surpriseMeFreq * juce::MathConstants<float>::twoPi) / 4.0f;
		surpriseMePhase = std::fmod(
			(float)ppqPosition * phaseIncrement,
			juce::MathConstants<float>::twoPi);

		float lfo = std::sin(surpriseMePhase) + 0.5f * std::sin(surpriseMePhase * 1.7f + 1.3f) + 0.3f * std::sin(surpriseMePhase * 3.1f + 0.5f);
		lfo /= 1.8f;

		float newIntensity = juce::jlimit(0.05f, 0.95f,
			surpriseMeCenter + lfo * surpriseMeAmplitude);

		setIntensity(newIntensity);
		if (onIntensityChanged)
			onIntensityChanged(newIntensity);
	}
}