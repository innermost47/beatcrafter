#include "PatternEngine.h"
#include <random>

namespace BeatCrafter
{

	PatternEngine::PatternEngine()
	{
		auto pattern = std::make_unique<Pattern>("Rock Basic");
		StyleManager::generateBasicPattern(*pattern, StyleType::Rock);
		slots[0] = std::move(pattern);

		std::random_device rd;
		for (int i = 0; i < 8; ++i)
		{
			slotStyles[i] = StyleType::Rock;
			slotRandomSeeds[i] = rd();
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

		intensityCacheValid = false;
		intensifiedPatternCache.clear();

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
		{
			queuedSlot = slot;
		}
	}

	void PatternEngine::processBlock(juce::MidiBuffer& midiMessages,
		int numSamples,
		double sampleRate,
		const juce::AudioPlayHead::PositionInfo& posInfo)
	{
		midiMessages.clear();
		if (!slots[activeSlot])
			return;
		double bpm = posInfo.getBpm().orFallback(120.0);
		double ppqPosition = posInfo.getPpqPosition().orFallback(0.0);
		double beatsPerSecond = bpm / 60.0;
		double sixteenthsPerSecond = beatsPerSecond * 4.0;
		samplesPerStep = static_cast<int>(sampleRate / sixteenthsPerSecond);
		auto& pattern = *slots[activeSlot];
		int patternLength = pattern.getLength();
		double ppqPerStep = 0.25;
		int currentStepFromPPQ = static_cast<int>(ppqPosition / ppqPerStep) % patternLength;
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

	void PatternEngine::sendAllNotesOff(juce::MidiBuffer& midiMessages)
	{
		for (int ch = 1; ch <= 16; ++ch)
		{
			midiMessages.addEvent(juce::MidiMessage::allNotesOff(ch), 0);
			midiMessages.addEvent(juce::MidiMessage::allSoundOff(ch), 0);
		}
	}

	void PatternEngine::addLiveJamElements(Pattern& pattern, int stepIndex, float intensity)
	{
		stepsSinceLastJam++;

		float jamChance = currentLiveJamIntensity * 0.15f;

		if (liveJamRandom.nextFloat() < jamChance)
		{
			stepsSinceLastJam = 0;

			int elementType = liveJamRandom.nextInt(6);

			switch (elementType)
			{
			case 0:
				if (!pattern.getTrack(0).getStep(stepIndex).isActive())
				{
					pattern.getTrack(0).getStep(stepIndex).setActive(true);
					pattern.getTrack(0).getStep(stepIndex).setVelocity(0.4f + currentLiveJamIntensity * 0.5f);
					pattern.getTrack(0).getStep(stepIndex).setProbability(0.6f + currentLiveJamIntensity * 0.3f);
				}
				break;

			case 1:
				if (!pattern.getTrack(1).getStep(stepIndex).isActive())
				{
					pattern.getTrack(1).getStep(stepIndex).setActive(true);
					pattern.getTrack(1).getStep(stepIndex).setVelocity(0.2f + liveJamRandom.nextFloat() * currentLiveJamIntensity * 0.4f);
					pattern.getTrack(1).getStep(stepIndex).setProbability(0.5f + currentLiveJamIntensity * 0.4f);
				}
				break;

			case 2:
				if (stepIndex % 4 == 0 || stepIndex % 8 == 0)
				{
					pattern.getTrack(4).getStep(stepIndex).setActive(true);
					pattern.getTrack(4).getStep(stepIndex).setVelocity(0.5f + currentLiveJamIntensity * 0.4f);
					pattern.getTrack(4).getStep(stepIndex).setProbability(0.4f + currentLiveJamIntensity * 0.4f);
				}
				break;

			case 3:
				pattern.getTrack(3).getStep(stepIndex).setActive(true);
				pattern.getTrack(3).getStep(stepIndex).setVelocity(0.3f + currentLiveJamIntensity * 0.4f);
				pattern.getTrack(3).getStep(stepIndex).setProbability(0.6f + currentLiveJamIntensity * 0.3f);
				break;

			case 4:
				if (currentLiveJamIntensity > 0.4f && stepIndex % 4 == 0)
				{
					pattern.getTrack(10).getStep(stepIndex).setActive(true);
					pattern.getTrack(10).getStep(stepIndex).setVelocity(0.5f + currentLiveJamIntensity * 0.4f);
					pattern.getTrack(10).getStep(stepIndex).setProbability(0.3f + currentLiveJamIntensity * 0.5f);
				}
				break;

			case 5:
				if (currentLiveJamIntensity > 0.3f)
				{
					pattern.getTrack(5).getStep(stepIndex).setActive(true);
					pattern.getTrack(5).getStep(stepIndex).setVelocity(0.3f + currentLiveJamIntensity * 0.5f);
					pattern.getTrack(5).getStep(stepIndex).setProbability(0.5f + currentLiveJamIntensity * 0.4f);
				}
				break;
			}
		}

		if (stepIndex >= 14 && currentLiveJamIntensity > 0.5f &&
			liveJamRandom.nextFloat() < currentLiveJamIntensity * 0.4f)
		{
			int tomTrack = liveJamRandom.nextBool() ? 6 : 7;
			pattern.getTrack(tomTrack).getStep(stepIndex).setActive(true);
			pattern.getTrack(tomTrack).getStep(stepIndex).setVelocity(0.4f + liveJamRandom.nextFloat() * currentLiveJamIntensity * 0.5f);
			pattern.getTrack(tomTrack).getStep(stepIndex).setProbability(0.6f + currentLiveJamIntensity * 0.3f);
		}

		if (currentLiveJamIntensity > 0.7f && liveJamRandom.nextFloat() < currentLiveJamIntensity * 0.2f)
		{
			if (!pattern.getTrack(2).getStep(stepIndex).isActive())
			{
				pattern.getTrack(2).getStep(stepIndex).setActive(true);
				pattern.getTrack(2).getStep(stepIndex).setVelocity(0.2f + currentLiveJamIntensity * 0.3f);
				pattern.getTrack(2).getStep(stepIndex).setProbability(0.7f + currentLiveJamIntensity * 0.2f);
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
}