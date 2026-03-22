#include "StyleManager.h"
#include <algorithm>

namespace BeatCrafter
{

	void StyleManager::generateBasicPattern(Pattern &pattern, StyleType style)
	{
		pattern.clear();
		switch (style)
		{
		case StyleType::Rock:
			generateRockPattern(pattern);
			break;
		case StyleType::Metal:
			generateMetalPattern(pattern);
			break;
		case StyleType::Jazz:
			generateJazzPattern(pattern);
			break;
		case StyleType::Funk:
			generateFunkPattern(pattern);
			break;
		case StyleType::Electronic:
			generateElectronicPattern(pattern);
			break;
		case StyleType::HipHop:
			generateHipHopPattern(pattern);
			break;
		case StyleType::Latin:
			generateLatinPattern(pattern);
			break;
		case StyleType::Punk:
			generatePunkPattern(pattern);
			break;
		default:
			generateRockPattern(pattern);
			break;
		}
	}

	Pattern StyleManager::applyIntensity(const Pattern &basePattern, float intensity,
										 StyleType style, uint32_t seed)
	{
		getRNG().seed(seed);
		Pattern result = basePattern;

		if (intensity == 0.0f)
		{
			for (int t = 0; t < result.getNumTracks(); ++t)
				for (int i = 0; i < result.getLength(); ++i)
					result.getTrack(t).getStep(i).setActive(false);
			return result;
		}

		if (intensity <= 0.15f)
		{
			for (int t = 0; t < result.getNumTracks(); ++t)
				for (int i = 0; i < result.getLength(); ++i)
					result.getTrack(t).getStep(i).setActive(false);
			addSoftIntro(result, intensity, style);
			return result;
		}

		if (intensity <= 0.4f && style == StyleType::Punk)
		{
			for (int t = 0; t < result.getNumTracks(); ++t)
				for (int i = 0; i < result.getLength(); ++i)
					result.getTrack(t).getStep(i).setActive(false);
			generatePunkIntro(result, intensity);
			return result;
		}

		applyBaseIntensityScaling(result, intensity);

		if (intensity > 0.9f)
		{
			addBreakMode(result, intensity, style);
		}
		else
		{
			if (intensity > 0.1f)
				addSubtleVariations(result, intensity, style);
			if (intensity > 0.3f)
				changeSnarePattern(result, intensity, style);
			if (intensity > 0.4f)
				addHiHatRideVariations(result, intensity, style);
			if (intensity > 0.5f)
				addGhostNotes(result, 1, intensity * 0.7f);
			if (intensity > 0.7f)
				addRandomFills(result, intensity);
		}

		return result;
	}

	void StyleManager::changeSnarePattern(Pattern &pattern, float intensity, StyleType style)
	{
		switch (style)
		{
		case StyleType::Rock:
			generateRockSnare(pattern, intensity);
			break;
		case StyleType::Metal:
			generateMetalSnare(pattern, intensity);
			break;
		case StyleType::Jazz:
			generateJazzSnare(pattern, intensity);
			break;
		case StyleType::Funk:
			generateFunkSnare(pattern, intensity);
			break;
		case StyleType::Electronic:
			generateElectronicSnare(pattern, intensity);
			break;
		case StyleType::HipHop:
			generateHipHopSnare(pattern, intensity);
			break;
		case StyleType::Latin:
			generateLatinSnare(pattern, intensity);
			break;
		case StyleType::Punk:
			generatePunkSnare(pattern, intensity);
			break;
		}
	}

	void StyleManager::addHiHatRideVariations(Pattern &pattern, float intensity, StyleType style)
	{
		switch (style)
		{
		case StyleType::Rock:
			generateRockHiHat(pattern, intensity);
			break;
		case StyleType::Metal:
			generateMetalHiHat(pattern, intensity);
			break;
		case StyleType::Jazz:
			generateJazzHiHat(pattern, intensity);
			break;
		case StyleType::Funk:
			generateFunkHiHat(pattern, intensity);
			break;
		case StyleType::Electronic:
			generateElectronicHiHat(pattern, intensity);
			break;
		case StyleType::HipHop:
			generateHipHopHiHat(pattern, intensity);
			break;
		case StyleType::Latin:
			generateLatinHiHat(pattern, intensity);
			break;
		case StyleType::Punk:
			generatePunkHiHat(pattern, intensity);
			break;
		}
	}

	void StyleManager::addBreakMode(Pattern &pattern, float intensity, StyleType style)
	{
		if (!randomChance(0.6f))
			return;
		clearCymbalsAndToms(pattern);
		switch (style)
		{
		case StyleType::Rock:
			generateRockBreak(pattern, intensity);
			break;
		case StyleType::Metal:
			generateMetalBreak(pattern, intensity);
			break;
		case StyleType::Jazz:
			generateJazzBreak(pattern, intensity);
			break;
		case StyleType::Funk:
			generateFunkBreak(pattern, intensity);
			break;
		case StyleType::Electronic:
			generateElectronicBreak(pattern, intensity);
			break;
		case StyleType::HipHop:
			generateHipHopBreak(pattern, intensity);
			break;
		case StyleType::Latin:
			generateLatinBreak(pattern, intensity);
			break;
		case StyleType::Punk:
			generatePunkBreak(pattern, intensity);
			break;
		}
	}

	void StyleManager::clearCymbalsAndToms(Pattern &pattern)
	{
		int tracks[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
		for (int t : tracks)
			for (int i = 0; i < pattern.getLength(); ++i)
				pattern.getTrack(t).getStep(i).setActive(false);
	}

	void StyleManager::addSoftIntro(Pattern &pattern, float intensity, StyleType style)
	{
		for (int t = 0; t < pattern.getNumTracks(); ++t)
			for (int i = 0; i < pattern.getLength(); ++i)
				pattern.getTrack(t).getStep(i).setActive(false);

		switch (style)
		{
		case StyleType::Metal:
		case StyleType::Jazz:
		{
			auto &rideTrack = pattern.getTrack(5);
			for (int i = 0; i < 16; i += 4)
			{
				if (randomChance(intensity * 4.0f))
				{
					rideTrack.getStep(i).setActive(true);
					rideTrack.getStep(i).setVelocity(randomFloat(0.2f, 0.3f + intensity));
				}
			}
			break;
		}
		case StyleType::Punk:
			generatePunkIntro(pattern, intensity);
			break;
		default:
		{
			auto &hihatTrack = pattern.getTrack(2);
			for (int i = 2; i < 16; i += 4)
			{
				if (randomChance(intensity * 4.0f))
				{
					hihatTrack.getStep(i).setActive(true);
					hihatTrack.getStep(i).setVelocity(randomFloat(0.2f, 0.3f + intensity));
				}
			}
			break;
		}
		}
	}

	void StyleManager::applyBaseIntensityScaling(Pattern &pattern, float intensity)
	{
		for (int t = 0; t < pattern.getNumTracks(); ++t)
		{
			auto &track = pattern.getTrack(t);
			for (int i = 0; i < pattern.getLength(); ++i)
			{
				auto &step = track.getStep(i);
				if (step.isActive())
				{
					step.setVelocity(step.getVelocity() * (0.3f + intensity * 0.7f));
					if (step.getProbability() < 1.0f)
						step.setProbability(step.getProbability() * (0.7f + intensity * 0.3f));
				}
			}
		}
	}

	void StyleManager::addGhostNotes(Pattern &pattern, int track, float probability)
	{
		auto &t = pattern.getTrack(track);
		for (int i = 0; i < pattern.getLength(); ++i)
		{
			if (!t.getStep(i).isActive() && randomChance(probability * 0.3f))
			{
				t.getStep(i).setActive(true);
				t.getStep(i).setVelocity(randomFloat(0.2f, 0.4f));
				t.getStep(i).setProbability(randomFloat(0.6f, 0.9f));
			}
		}
	}

	void StyleManager::addRandomFills(Pattern &pattern, float intensity)
	{
		if (!randomChance(intensity * 0.6f))
			return;
		auto &tomHi = pattern.getTrack(6);
		auto &tomLo = pattern.getTrack(7);
		int fillStart = 12 + static_cast<int>(randomFloat(0.f, 4.f));
		for (int i = fillStart; i < 16; ++i)
		{
			if (randomChance(0.6f))
			{
				if (randomChance(0.5f))
				{
					tomHi.getStep(i).setActive(true);
					tomHi.getStep(i).setVelocity(randomFloat(0.6f, 0.9f));
				}
				else
				{
					tomLo.getStep(i).setActive(true);
					tomLo.getStep(i).setVelocity(randomFloat(0.6f, 0.9f));
				}
			}
		}
	}

	void StyleManager::applyComplexityToPattern(Pattern &pattern, StyleType style,
												float complexity, uint32_t seed)
	{
		switch (style)
		{
		case StyleType::Rock:
			if (complexity > 0.5f)
			{
				pattern.getTrack(0).getStep(2).setActive(true);
				pattern.getTrack(0).getStep(2).setVelocity(0.7f);
				pattern.getTrack(0).getStep(10).setActive(true);
				pattern.getTrack(0).getStep(10).setVelocity(0.7f);
			}
			if (complexity > 0.7f)
			{
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.8f);
			}
			break;
		case StyleType::Metal:
			if (complexity > 0.6f)
			{
				pattern.getTrack(0).getStep(1).setActive(true);
				pattern.getTrack(0).getStep(1).setVelocity(0.7f);
				pattern.getTrack(0).getStep(9).setActive(true);
				pattern.getTrack(0).getStep(9).setVelocity(0.7f);
			}
			if (complexity > 0.7f)
			{
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.9f);
			}
			break;
		case StyleType::Jazz:
			if (complexity > 0.6f)
			{
				pattern.getTrack(1).getStep(7).setActive(true);
				pattern.getTrack(1).getStep(7).setVelocity(0.25f);
				pattern.getTrack(1).getStep(15).setActive(true);
				pattern.getTrack(1).getStep(15).setVelocity(0.3f);
			}
			break;
		case StyleType::Electronic:
			if (complexity > 0.6f)
			{
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.8f);
			}
			if (complexity > 0.7f)
			{
				pattern.getTrack(0).getStep(6).setActive(true);
				pattern.getTrack(0).getStep(6).setVelocity(0.7f);
			}
			break;
		case StyleType::HipHop:
			if (complexity > 0.6f)
			{
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.9f);
			}
			if (complexity > 0.7f)
			{
				pattern.getTrack(0).getStep(3).setActive(true);
				pattern.getTrack(0).getStep(3).setVelocity(0.8f);
			}
			break;
		case StyleType::Funk:
			if (complexity > 0.6f)
			{
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.9f);
			}
			if (complexity > 0.7f)
			{
				pattern.getTrack(1).getStep(10).setActive(true);
				pattern.getTrack(1).getStep(10).setVelocity(0.35f);
			}
			break;
		case StyleType::Latin:
			if (complexity > 0.6f)
			{
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(0.8f);
			}
			if (complexity > 0.7f)
			{
				pattern.getTrack(0).getStep(13).setActive(true);
				pattern.getTrack(0).getStep(13).setVelocity(0.7f);
			}
			break;
		case StyleType::Punk:
			if (complexity > 0.6f)
			{
				pattern.getTrack(1).getStep(4).setActive(true);
				pattern.getTrack(1).getStep(4).setVelocity(1.0f);
			}
			if (complexity > 0.7f)
			{
				for (int i = 1; i < 16; i += 2)
				{
					pattern.getTrack(2).getStep(i).setActive(true);
					pattern.getTrack(2).getStep(i).setVelocity(0.6f);
				}
			}
			break;
		}

		std::mt19937 rng(seed);
		std::uniform_real_distribution<float> velVar(-0.1f, 0.1f);
		for (int t = 0; t < pattern.getNumTracks(); ++t)
			for (int i = 0; i < pattern.getLength(); ++i)
			{
				auto &step = pattern.getTrack(t).getStep(i);
				if (step.isActive())
					step.setVelocity(juce::jlimit(0.3f, 1.0f, step.getVelocity() + velVar(rng)));
			}
	}

	void StyleManager::addSubtleVariations(Pattern &pattern, float intensity, StyleType style)
	{
		auto &kickTrack = pattern.getTrack(0);
		auto &snareTrack = pattern.getTrack(1);
		auto &hihatTrack = pattern.getTrack(2);

		for (int i = 0; i < pattern.getLength(); ++i)
		{
			if (kickTrack.getStep(i).isActive() && randomChance(intensity * 0.15f))
				kickTrack.getStep(i).setVelocity(kickTrack.getStep(i).getVelocity() + randomFloat(-0.05f, 0.08f));
			if (snareTrack.getStep(i).isActive() && randomChance(intensity * 0.12f))
				snareTrack.getStep(i).setVelocity(snareTrack.getStep(i).getVelocity() + randomFloat(-0.04f, 0.1f));
		}

		if (intensity > 0.1f)
			for (int i = 0; i < 16; ++i)
				if (!hihatTrack.getStep(i).isActive() && randomChance(intensity * 0.25f))
				{
					hihatTrack.getStep(i).setActive(true);
					hihatTrack.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
					hihatTrack.getStep(i).setProbability(0.5f + intensity * 0.3f);
				}

		for (int i = 1; i < 16; ++i)
			if (!kickTrack.getStep(i).isActive() && randomChance(intensity * 0.1f))
			{
				kickTrack.getStep(i).setActive(true);
				kickTrack.getStep(i).setVelocity(randomFloat(0.35f, 0.55f));
				kickTrack.getStep(i).setProbability(0.5f + intensity * 0.3f);
			}

		if (style == StyleType::Metal && intensity > 0.3f)
		{
			float dkChance = (intensity - 0.3f) * 1.2f;
			for (int i = 0; i < 16; ++i)
				if (kickTrack.getStep(i).isActive() && randomChance(dkChance))
				{
					int next = (i + 1) % 16;
					if (!kickTrack.getStep(next).isActive())
					{
						kickTrack.getStep(next).setActive(true);
						kickTrack.getStep(next).setVelocity(randomFloat(0.7f, 0.9f));
						kickTrack.getStep(next).setProbability(0.6f + intensity * 0.3f);
					}
				}
			if (intensity > 0.7f)
				for (int i = 0; i < 16; i += 2)
					if (randomChance(dkChance * 0.8f))
					{
						kickTrack.getStep(i).setActive(true);
						kickTrack.getStep(i).setVelocity(randomFloat(0.8f, 0.95f));
						if (i + 1 < 16)
						{
							kickTrack.getStep(i + 1).setActive(true);
							kickTrack.getStep(i + 1).setVelocity(randomFloat(0.75f, 0.9f));
						}
					}
		}

		if (style == StyleType::Electronic && intensity > 0.3f)
		{
			kickTrack.getStep(0).setActive(true);
			kickTrack.getStep(4).setActive(true);
			kickTrack.getStep(8).setActive(true);
			kickTrack.getStep(12).setActive(true);
			if (intensity > 0.6f)
				for (int i = 2; i < 16; i += 4)
					if (randomChance(intensity * 0.4f))
					{
						kickTrack.getStep(i).setActive(true);
						kickTrack.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
						kickTrack.getStep(i).setProbability(0.6f + intensity * 0.3f);
					}
		}

		if (style == StyleType::Punk && intensity > 0.2f)
		{
			for (int i = 0; i < 16; ++i)
				kickTrack.getStep(i).setActive(false);
			if (intensity <= 0.3f)
			{
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(randomFloat(0.5f, 0.65f));
			}
			else if (intensity <= 0.4f)
			{
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(randomFloat(0.6f, 0.75f));
				kickTrack.getStep(8).setActive(true);
				kickTrack.getStep(8).setVelocity(randomFloat(0.55f, 0.7f));
			}
			else if (intensity <= 0.5f)
			{
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(randomFloat(0.7f, 0.8f));
				kickTrack.getStep(4).setActive(true);
				kickTrack.getStep(4).setVelocity(randomFloat(0.6f, 0.7f));
				kickTrack.getStep(8).setActive(true);
				kickTrack.getStep(8).setVelocity(randomFloat(0.7f, 0.8f));
				kickTrack.getStep(12).setActive(true);
				kickTrack.getStep(12).setVelocity(randomFloat(0.6f, 0.7f));
			}
			else if (intensity <= 0.7f)
			{
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(randomFloat(0.75f, 0.85f));
				kickTrack.getStep(4).setActive(true);
				kickTrack.getStep(4).setVelocity(randomFloat(0.7f, 0.8f));
				if (randomChance(0.6f))
				{
					kickTrack.getStep(5).setActive(true);
					kickTrack.getStep(5).setVelocity(randomFloat(0.65f, 0.75f));
				}
				kickTrack.getStep(8).setActive(true);
				kickTrack.getStep(8).setVelocity(randomFloat(0.75f, 0.85f));
				kickTrack.getStep(12).setActive(true);
				kickTrack.getStep(12).setVelocity(randomFloat(0.7f, 0.8f));
				if (randomChance(0.6f))
				{
					kickTrack.getStep(13).setActive(true);
					kickTrack.getStep(13).setVelocity(randomFloat(0.65f, 0.75f));
				}
			}
			else
			{
				kickTrack.getStep(0).setActive(true);
				kickTrack.getStep(0).setVelocity(randomFloat(0.85f, 0.95f));
				kickTrack.getStep(4).setActive(true);
				kickTrack.getStep(4).setVelocity(randomFloat(0.8f, 0.9f));
				kickTrack.getStep(5).setActive(true);
				kickTrack.getStep(5).setVelocity(randomFloat(0.75f, 0.85f));
				kickTrack.getStep(8).setActive(true);
				kickTrack.getStep(8).setVelocity(randomFloat(0.85f, 0.95f));
				kickTrack.getStep(12).setActive(true);
				kickTrack.getStep(12).setVelocity(randomFloat(0.8f, 0.9f));
				kickTrack.getStep(13).setActive(true);
				kickTrack.getStep(13).setVelocity(randomFloat(0.75f, 0.85f));
			}
		}
	}

	std::mt19937 &StyleManager::getRNG()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return gen;
	}

	float StyleManager::randomFloat(float min, float max)
	{
		std::uniform_real_distribution<float> dis(min, max);
		return dis(getRNG());
	}

	bool StyleManager::randomChance(float probability)
	{
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		return dis(getRNG()) < probability;
	}

}