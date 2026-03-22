#include "../StyleManager.h"

namespace BeatCrafter
{

	void StyleManager::generatePunkPattern(Pattern& pattern)
	{
		pattern.clear();
		pattern.getTrack(0).getStep(0).setActive(true);
		pattern.getTrack(0).getStep(0).setVelocity(0.9f);
		pattern.getTrack(1).getStep(8).setActive(true);
		pattern.getTrack(1).getStep(8).setVelocity(1.0f);
	}

	void StyleManager::generatePunkSnare(Pattern& pattern, float intensity)
	{
		auto& snare = pattern.getTrack(1);
		for (int i = 0; i < pattern.getLength(); ++i)
			snare.getStep(i).setActive(false);

		if (intensity <= 0.4f)
		{
			snare.getStep(4).setActive(true);
			snare.getStep(4).setVelocity(randomFloat(0.8f, 0.95f));
			snare.getStep(12).setActive(true);
			snare.getStep(12).setVelocity(randomFloat(0.8f, 0.95f));
		}
		else if (intensity <= 0.7f)
		{
			for (int i : {2, 6, 10, 14})
			{
				snare.getStep(i).setActive(true);
				snare.getStep(i).setVelocity(randomFloat(0.85f, 0.95f));
			}
		}
		else
		{
			for (int i : {2, 6, 10, 14})
			{
				snare.getStep(i).setActive(true);
				snare.getStep(i).setVelocity(randomFloat(0.9f, 1.0f));
			}
			if (randomChance(intensity * 0.3f))
			{
				snare.getStep(1).setActive(true);
				snare.getStep(1).setVelocity(randomFloat(0.3f, 0.5f));
				snare.getStep(1).setProbability(0.6f);
			}
			if (randomChance(intensity * 0.3f))
			{
				snare.getStep(9).setActive(true);
				snare.getStep(9).setVelocity(randomFloat(0.3f, 0.5f));
				snare.getStep(9).setProbability(0.6f);
			}
		}
	}

	void StyleManager::generatePunkHiHat(Pattern& pattern, float intensity)
	{
		auto& hh = pattern.getTrack(2);
		auto& ohh = pattern.getTrack(3);
		auto& crash = pattern.getTrack(4);
		auto& splash = pattern.getTrack(10);
		for (int i = 0; i < 16; ++i)
		{
			hh.getStep(i).setActive(false);
			ohh.getStep(i).setActive(false);
			crash.getStep(i).setActive(false);
			splash.getStep(i).setActive(false);
		}

		if (intensity <= 0.4f)
		{
			for (int i = 0; i < 16; i += 2)
			{
				hh.getStep(i).setActive(true);
				hh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
			}
		}
		else if (intensity <= 0.5f)
		{
			for (int i = 0; i < 16; ++i)
			{
				hh.getStep(i).setActive(true);
				hh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
			}
			ohh.getStep(6).setActive(true);
			ohh.getStep(6).setVelocity(randomFloat(0.6f, 0.8f));
			ohh.getStep(14).setActive(true);
			ohh.getStep(14).setVelocity(randomFloat(0.6f, 0.8f));
		}
		else if (intensity <= 0.6f)
		{
			for (int i = 0; i < 16; ++i)
			{
				ohh.getStep(i).setActive(true);
				ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.8f));
			}
		}
		else if (intensity <= 0.7f)
		{
			for (int i = 0; i < 16; ++i)
			{
				ohh.getStep(i).setActive(true);
				ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.8f));
			}
			for (int i : {2, 6, 10, 14})
			{
				splash.getStep(i).setActive(true);
				splash.getStep(i).setVelocity(randomFloat(0.85f, 0.95f));
			}
		}
		else
		{
			for (int i : {0, 4, 8, 12})
			{
				crash.getStep(i).setActive(true);
				crash.getStep(i).setVelocity(randomFloat(0.85f, 0.95f));
			}
			for (int i : {2, 6, 10, 14})
			{
				splash.getStep(i).setActive(true);
				splash.getStep(i).setVelocity(randomFloat(0.85f, 0.95f));
			}
		}
	}

	void StyleManager::generatePunkBreak(Pattern& pattern, float /*intensity*/)
	{
		auto& kick = pattern.getTrack(0);
		auto& tomHi = pattern.getTrack(6);
		auto& tomLo = pattern.getTrack(7);
		auto& china = pattern.getTrack(11);
		auto& splash = pattern.getTrack(10);
		for (int i : {0, 4, 8, 12})
		{
			kick.getStep(i).setActive(true);
			kick.getStep(i).setVelocity(i % 8 == 0 ? 0.9f : 0.85f);
		}
		for (int i : {2, 6, 10, 14})
		{
			tomHi.getStep(i).setActive(true);
			tomHi.getStep(i).setVelocity(randomFloat(0.7f, 0.95f));
		}
		for (int i : {1, 5, 9, 13})
		{
			tomLo.getStep(i).setActive(true);
			tomLo.getStep(i).setVelocity(randomFloat(0.6f, 0.9f));
		}
		china.getStep(0).setActive(true);
		china.getStep(0).setVelocity(randomFloat(0.8f, 1.0f));
		splash.getStep(8).setActive(true);
		splash.getStep(8).setVelocity(randomFloat(0.7f, 0.9f));
	}

	void StyleManager::generatePunkIntro(Pattern& pattern, float intensity)
	{
		auto& tomHi = pattern.getTrack(6);
		auto& tomLo = pattern.getTrack(7);

		if (intensity <= 0.05f)
		{
			for (int i = 0; i < 16; ++i)
			{
				tomLo.getStep(i).setActive(true);
				tomLo.getStep(i).setVelocity(0.1f + sin((float)i * 0.8f) * 0.05f + randomFloat(-0.02f, 0.02f));
				tomLo.getStep(i).setProbability(0.7f);
			}
		}
		else if (intensity <= 0.10f)
		{
			for (int i = 0; i < 16; ++i)
			{
				float base = 0.15f + intensity * 0.5f;
				float cresc = (i < 4) ? (float)i / 4.0f * 0.1f : (i >= 8 && i < 12) ? (float)(i - 8) / 4.0f * 0.15f
					: 0.0f;
				float accent = (i % 4 == 0) ? 0.1f : 0.0f;
				tomLo.getStep(i).setActive(true);
				tomLo.getStep(i).setVelocity(std::min(base + cresc + accent + randomFloat(-0.03f, 0.03f), 0.4f));
				if ((i == 3 || i == 7 || i == 11 || i == 15) && randomChance(0.5f))
				{
					tomHi.getStep(i).setActive(true);
					tomHi.getStep(i).setVelocity(tomLo.getStep(i).getVelocity() * 0.8f);
					tomHi.getStep(i).setProbability(0.7f);
				}
			}
		}
		else
		{
			for (int i = 0; i < 16; ++i)
			{
				float base = 0.2f + intensity * 1.5f;
				float cresc = i < 8 ? (float)i / 8.0f * 0.15f : (float)(i - 8) / 8.0f * 0.2f;
				float accent = (i == 0 || i == 8) ? 0.15f : (i % 2 == 0 ? 0.08f : 0.0f);
				float vel = std::min(base + cresc + accent + randomFloat(-0.04f, 0.04f), 0.5f);
				tomLo.getStep(i).setActive(true);
				tomLo.getStep(i).setVelocity(vel);
				bool useHi = (i % 4 == 1 || i % 4 == 3) ? randomChance(0.6f) : (i == 7 || i == 15);
				if (useHi)
				{
					tomHi.getStep(i).setActive(true);
					tomHi.getStep(i).setVelocity(vel * 0.9f);
					tomLo.getStep(i).setVelocity(vel * 0.6f);
				}
			}
			if (randomChance(0.4f))
			{
				int flamPos = randomChance(0.5f) ? 7 : 15;
				tomLo.getStep(flamPos).setVelocity(tomLo.getStep(flamPos).getVelocity() * 1.2f);
				if (flamPos > 0)
				{
					tomHi.getStep(flamPos - 1).setActive(true);
					tomHi.getStep(flamPos - 1).setVelocity(0.25f);
				}
			}
		}
	}

}