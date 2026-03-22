#include "../StyleManager.h"

namespace BeatCrafter
{

	void StyleManager::generateMetalPattern(Pattern& pattern)
	{
		pattern.clear();
		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.95f);
		pattern.getTrack(0).getStep(2).setActive(true);
		pattern.getTrack(0).getStep(2).setVelocity(0.7f);
		pattern.getTrack(0).getStep(2).setProbability(0.8f);
		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(0.95f);
		pattern.getTrack(5).getStep(6).setActive(true);
		pattern.getTrack(5).getStep(6).setVelocity(0.5f);
		pattern.getTrack(5).getStep(6).setProbability(0.7f);
	}

	void StyleManager::generateMetalSnare(Pattern& pattern, float intensity)
	{
		auto& snare = pattern.getTrack(1);
		for (int i = 0; i < pattern.getLength(); ++i)
			snare.getStep(i).setActive(false);

		if (intensity <= 0.3f)
		{
			snare.getStep(8).setActive(true);
			snare.getStep(8).setVelocity(randomFloat(0.9f, 1.0f));
		}
		else if (intensity <= 0.5f)
		{
			snare.getStep(4).setActive(true);
			snare.getStep(4).setVelocity(randomFloat(0.85f, 0.95f));
			snare.getStep(12).setActive(true);
			snare.getStep(12).setVelocity(randomFloat(0.85f, 0.95f));
		}
		else if (intensity <= 0.7f)
		{
			snare.getStep(4).setActive(true);
			snare.getStep(4).setVelocity(randomFloat(0.9f, 1.0f));
			snare.getStep(5).setActive(true);
			snare.getStep(5).setVelocity(randomFloat(0.7f, 0.8f));
			snare.getStep(12).setActive(true);
			snare.getStep(12).setVelocity(randomFloat(0.9f, 1.0f));
			snare.getStep(13).setActive(true);
			snare.getStep(13).setVelocity(randomFloat(0.7f, 0.8f));
		}
		else if (intensity <= 0.85f)
		{
			for (int i = 2; i < 16; i += 2)
			{
				snare.getStep(i).setActive(true);
				snare.getStep(i).setVelocity(randomFloat(0.8f, 0.95f));
			}
		}
		else
		{
			for (int i = 1; i < 16; i += 2)
			{
				snare.getStep(i).setActive(true);
				snare.getStep(i).setVelocity(randomFloat(0.85f, 1.0f));
			}
		}
	}

	void StyleManager::generateMetalHiHat(Pattern& pattern, float intensity)
	{
		auto& ride = pattern.getTrack(5);
		auto& rideBell = pattern.getTrack(8);
		auto& hhped = pattern.getTrack(9);
		auto& splash = pattern.getTrack(10);
		auto& china = pattern.getTrack(11);
		for (int i = 0; i < 16; ++i)
		{
			ride.getStep(i).setActive(false);
			rideBell.getStep(i).setActive(false);
			hhped.getStep(i).setActive(false);
			splash.getStep(i).setActive(false);
			china.getStep(i).setActive(false);
		}

		if (intensity <= 0.4f)
		{
			for (int i = 0; i < 16; i += 2)
			{
				ride.getStep(i).setActive(true);
				ride.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
			}
		}
		else if (intensity <= 0.6f)
		{
			for (int i = 0; i < 16; i += 2)
			{
				if (i % 8 == 0 && randomChance(0.5f))
				{
					rideBell.getStep(i).setActive(true);
					rideBell.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
				}
				else
				{
					ride.getStep(i).setActive(true);
					ride.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
			}
		}
		else if (intensity <= 0.8f)
		{
			for (int i = 0; i < 16; ++i)
			{
				if (i == 0 && randomChance(0.5f))
				{
					china.getStep(i).setActive(true);
					china.getStep(i).setVelocity(randomFloat(0.8f, 1.0f));
				}
				else if (i % 4 == 0 && randomChance(0.4f))
				{
					rideBell.getStep(i).setActive(true);
					rideBell.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
				}
				else
				{
					ride.getStep(i).setActive(true);
					ride.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
				}
			}
			hhped.getStep(4).setActive(true);
			hhped.getStep(4).setVelocity(0.3f);
			hhped.getStep(12).setActive(true);
			hhped.getStep(12).setVelocity(0.3f);
		}
		else
		{
			for (int i = 0; i < 16; ++i)
				if (randomChance(0.7f))
				{
					if (randomChance(0.6f))
					{
						china.getStep(i).setActive(true);
						china.getStep(i).setVelocity(randomFloat(0.8f, 1.0f));
					}
					else
					{
						splash.getStep(i).setActive(true);
						splash.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
					}
				}
		}
	}

	void StyleManager::generateMetalBreak(Pattern& pattern, float /*intensity*/)
	{
		auto& kick = pattern.getTrack(0);
		auto& tomHi = pattern.getTrack(6);
		auto& tomLo = pattern.getTrack(7);
		auto& china = pattern.getTrack(11);
		auto& splash = pattern.getTrack(10);
		int kicks[] = { 0, 1, 2, 4, 5, 8, 9, 12, 13 };
		float vels[] = { 0.95f, 0.9f, 0.85f, 0.9f, 0.85f, 0.95f, 0.9f, 0.9f, 0.85f };
		for (int k = 0; k < 9; ++k)
		{
			kick.getStep(kicks[k]).setActive(true);
			kick.getStep(kicks[k]).setVelocity(vels[k]);
		}
		tomHi.getStep(3).setActive(true);
		tomHi.getStep(3).setVelocity(randomFloat(0.8f, 0.95f));
		tomHi.getStep(6).setActive(true);
		tomHi.getStep(6).setVelocity(randomFloat(0.7f, 0.9f));
		tomHi.getStep(10).setActive(true);
		tomHi.getStep(10).setVelocity(randomFloat(0.8f, 0.95f));
		tomHi.getStep(14).setActive(true);
		tomHi.getStep(14).setVelocity(randomFloat(0.8f, 0.95f));
		tomLo.getStep(7).setActive(true);
		tomLo.getStep(7).setVelocity(randomFloat(0.8f, 0.95f));
		tomLo.getStep(11).setActive(true);
		tomLo.getStep(11).setVelocity(randomFloat(0.7f, 0.9f));
		tomLo.getStep(15).setActive(true);
		tomLo.getStep(15).setVelocity(randomFloat(0.8f, 0.95f));
		china.getStep(0).setActive(true);
		china.getStep(0).setVelocity(randomFloat(0.9f, 1.0f));
		splash.getStep(8).setActive(true);
		splash.getStep(8).setVelocity(randomFloat(0.8f, 0.95f));
	}

}