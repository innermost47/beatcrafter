#include "../StyleManager.h"

namespace BeatCrafter
{

	void StyleManager::generateLatinPattern(Pattern& pattern)
	{
		pattern.clear();
		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.75f);
		pattern.getTrack(0).getStep(6).setActive(true);
		pattern.getTrack(0).getStep(6).setVelocity(0.5f);
		pattern.getTrack(0).getStep(6).setProbability(0.8f);
		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.8f);
		pattern.getTrack(3).getStep(4).setActive(true);
		pattern.getTrack(3).getStep(4).setVelocity(0.55f);
		pattern.getTrack(3).getStep(12).setActive(true);
		pattern.getTrack(3).getStep(12).setVelocity(0.5f);
		pattern.getTrack(3).getStep(12).setProbability(0.75f);
	}

	void StyleManager::generateLatinSnare(Pattern& pattern, float intensity)
	{
		auto& snare = pattern.getTrack(1);
		for (int i = 0; i < pattern.getLength(); ++i)
			snare.getStep(i).setActive(false);

		if (intensity <= 0.4f)
		{
			snare.getStep(8).setActive(true);
			snare.getStep(8).setVelocity(0.8f);
		}
		else if (intensity <= 0.7f)
		{
			snare.getStep(4).setActive(true);
			snare.getStep(4).setVelocity(0.7f);
			snare.getStep(12).setActive(true);
			snare.getStep(12).setVelocity(0.7f);
		}
		else
		{
			snare.getStep(4).setActive(true);
			snare.getStep(4).setVelocity(0.8f);
			snare.getStep(6).setActive(true);
			snare.getStep(6).setVelocity(0.6f);
			snare.getStep(12).setActive(true);
			snare.getStep(12).setVelocity(0.8f);
			snare.getStep(14).setActive(true);
			snare.getStep(14).setVelocity(0.6f);
		}
	}

	void StyleManager::generateLatinHiHat(Pattern& pattern, float intensity)
	{
		auto& hh = pattern.getTrack(2);
		auto& ohh = pattern.getTrack(3);
		auto& ride = pattern.getTrack(5);
		auto& hhped = pattern.getTrack(9);
		auto& splash = pattern.getTrack(10);
		for (int i = 0; i < 16; ++i)
		{
			hh.getStep(i).setActive(false);
			ohh.getStep(i).setActive(false);
			ride.getStep(i).setActive(false);
			hhped.getStep(i).setActive(false);
			splash.getStep(i).setActive(false);
		}

		if (intensity <= 0.5f)
		{
			for (int i = 0; i < 16; i += 2)
			{
				if (i == 2 || i == 6 || i == 10 || i == 14)
				{
					ohh.getStep(i).setActive(true);
					ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
				else
				{
					hh.getStep(i).setActive(true);
					hh.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
			}
		}
		else if (intensity <= 0.7f)
		{
			for (int i = 0; i < 16; i += 2)
			{
				if (i == 2 || i == 6 || i == 10 || i == 14)
				{
					ohh.getStep(i).setActive(true);
					ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
				else
				{
					hh.getStep(i).setActive(true);
					hh.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
			}
			for (int s : {3, 7, 11})
			{
				hhped.getStep(s).setActive(true);
				hhped.getStep(s).setVelocity(0.4f);
			}
		}
		else if (intensity <= 0.8f)
		{
			for (int i = 0; i < 16; i += 2)
			{
				if (i == 8 && randomChance(0.6f))
				{
					splash.getStep(i).setActive(true);
					splash.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
				}
				else if (i == 2 || i == 6 || i == 10 || i == 14)
				{
					ohh.getStep(i).setActive(true);
					ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
				else
				{
					hh.getStep(i).setActive(true);
					hh.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
			}
			for (int s : {3, 7, 11})
			{
				hhped.getStep(s).setActive(true);
				hhped.getStep(s).setVelocity(0.4f);
			}
		}
		else
		{
			for (int i = 0; i < 16; ++i)
			{
				if (i % 4 == 0)
				{
					ride.getStep(i).setActive(true);
					ride.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
				}
				else if (i == 8)
				{
					splash.getStep(i).setActive(true);
					splash.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
				}
				else if (i == 2 || i == 6 || i == 10 || i == 14)
				{
					ohh.getStep(i).setActive(true);
					ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
			}
			for (int s : {3, 7, 11})
			{
				hhped.getStep(s).setActive(true);
				hhped.getStep(s).setVelocity(0.4f);
			}
		}
	}

	void StyleManager::generateLatinBreak(Pattern& pattern, float /*intensity*/)
	{
		auto& kick = pattern.getTrack(0);
		auto& tomHi = pattern.getTrack(6);
		auto& tomLo = pattern.getTrack(7);
		auto& splash = pattern.getTrack(10);
		for (int s : {0, 3, 6, 10})
		{
			kick.getStep(s).setActive(true);
			kick.getStep(s).setVelocity(randomFloat(0.7f, 0.85f));
		}
		for (int s : {1, 4, 8, 12, 14})
		{
			tomHi.getStep(s).setActive(true);
			tomHi.getStep(s).setVelocity(randomFloat(0.6f, 0.9f));
		}
		for (int s : {2, 5, 9, 15})
		{
			tomLo.getStep(s).setActive(true);
			tomLo.getStep(s).setVelocity(randomFloat(0.6f, 0.9f));
		}
		splash.getStep(0).setActive(true);
		splash.getStep(0).setVelocity(randomFloat(0.7f, 0.9f));
		splash.getStep(8).setActive(true);
		splash.getStep(8).setVelocity(randomFloat(0.6f, 0.8f));
	}

}