#include "../StyleManager.h"

namespace BeatCrafter
{

	void StyleManager::generateJazzPattern(Pattern& pattern)
	{
		pattern.clear();
		pattern.setSwing(0.67f);
		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.4f + (rand() % 20) / 100.0f);
		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.55f);
		pattern.getTrack(5).getStep(2).setActive(true);
		pattern.getTrack(5).getStep(2).setVelocity(0.3f);
		pattern.getTrack(5).getStep(10).setActive(true);
		pattern.getTrack(5).getStep(10).setVelocity(0.35f);
		pattern.getTrack(5).getStep(10).setProbability(0.7f);
	}

	void StyleManager::generateJazzSnare(Pattern& pattern, float intensity)
	{
		auto& snare = pattern.getTrack(1);
		for (int i = 0; i < pattern.getLength(); ++i)
			snare.getStep(i).setActive(false);

		if (intensity <= 0.4f)
		{
			snare.getStep(8).setActive(true);
			snare.getStep(8).setVelocity(randomFloat(0.5f, 0.7f));
			addGhostNotes(pattern, 1, intensity * 0.6f);
		}
		else if (intensity <= 0.7f)
		{
			snare.getStep(4).setActive(true);
			snare.getStep(4).setVelocity(randomFloat(0.5f, 0.7f));
			snare.getStep(12).setActive(true);
			snare.getStep(12).setVelocity(randomFloat(0.5f, 0.7f));
			addGhostNotes(pattern, 1, intensity * 0.8f);
		}
		else
		{
			for (int i = 1; i < 16; ++i)
				if (randomChance(intensity * 0.4f))
				{
					snare.getStep(i).setActive(true);
					snare.getStep(i).setVelocity(randomFloat(0.3f, 0.8f));
				}
		}
	}

	void StyleManager::generateJazzHiHat(Pattern& pattern, float intensity)
	{
		auto& ride = pattern.getTrack(5);
		auto& rideBell = pattern.getTrack(8);
		auto& hhped = pattern.getTrack(9);
		auto& ohh = pattern.getTrack(3);
		auto& splash = pattern.getTrack(10);
		for (int i = 0; i < 16; ++i)
		{
			ride.getStep(i).setActive(false);
			rideBell.getStep(i).setActive(false);
			hhped.getStep(i).setActive(false);
			ohh.getStep(i).setActive(false);
			splash.getStep(i).setActive(false);
		}

		if (intensity <= 0.5f)
		{
			for (int i = 0; i < 16; i += 2)
			{
				ride.getStep(i).setActive(true);
				ride.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
			}
			hhped.getStep(2).setActive(true);
			hhped.getStep(2).setVelocity(0.3f);
			hhped.getStep(10).setActive(true);
			hhped.getStep(10).setVelocity(0.3f);
		}
		else if (intensity <= 0.7f)
		{
			for (int i = 0; i < 16; i += 2)
			{
				if (i == 4 || i == 12)
				{
					rideBell.getStep(i).setActive(true);
					rideBell.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
				}
				else
				{
					ride.getStep(i).setActive(true);
					ride.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
			}
			for (int i = 2; i < 16; i += 4)
			{
				hhped.getStep(i).setActive(true);
				hhped.getStep(i).setVelocity(randomFloat(0.3f, 0.45f));
			}
		}
		else
		{
			for (int i = 0; i < 16; ++i)
			{
				if (i == 8 && randomChance(0.5f))
				{
					splash.getStep(i).setActive(true);
					splash.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
				else if (i % 4 == 0 && randomChance(0.4f))
				{
					rideBell.getStep(i).setActive(true);
					rideBell.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
				}
				else if (i == 6 && randomChance(0.3f))
				{
					ohh.getStep(i).setActive(true);
					ohh.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
				else if (randomChance(0.8f))
				{
					ride.getStep(i).setActive(true);
					ride.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
			}
			for (int i = 2; i < 16; i += 4)
			{
				hhped.getStep(i).setActive(true);
				hhped.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
			}
		}
	}

	void StyleManager::generateJazzBreak(Pattern& pattern, float /*intensity*/)
	{
		auto& kick = pattern.getTrack(0);
		auto& tomHi = pattern.getTrack(6);
		auto& tomLo = pattern.getTrack(7);
		auto& splash = pattern.getTrack(10);
		kick.getStep(0).setActive(true);
		kick.getStep(0).setVelocity(0.6f);
		kick.getStep(6).setActive(true);
		kick.getStep(6).setVelocity(0.5f);
		kick.getStep(10).setActive(true);
		kick.getStep(10).setVelocity(0.55f);
		int tomHiSteps[] = { 1, 3, 7, 11, 13 };
		float tomHiVels[] = { 0.6f, 0.5f, 0.6f, 0.7f, 0.6f };
		for (int k = 0; k < 5; ++k)
		{
			tomHi.getStep(tomHiSteps[k]).setActive(true);
			tomHi.getStep(tomHiSteps[k]).setVelocity(randomFloat(tomHiVels[k] - 0.1f, tomHiVels[k] + 0.1f));
		}
		int tomLoSteps[] = { 5, 9, 15 };
		float tomLoVels[] = { 0.7f, 0.6f, 0.7f };
		for (int k = 0; k < 3; ++k)
		{
			tomLo.getStep(tomLoSteps[k]).setActive(true);
			tomLo.getStep(tomLoSteps[k]).setVelocity(randomFloat(tomLoVels[k] - 0.1f, tomLoVels[k] + 0.1f));
		}
		splash.getStep(0).setActive(true);
		splash.getStep(0).setVelocity(randomFloat(0.6f, 0.8f));
	}

}