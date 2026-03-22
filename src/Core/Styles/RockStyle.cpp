#include "../StyleManager.h"

namespace BeatCrafter
{

    void StyleManager::generateRockPattern(Pattern &pattern)
    {
        pattern.clear();
        pattern.getTrack(0).getStep(0).setActive(true);
        pattern.getTrack(0).getStep(0).setVelocity(0.85f + (rand() % 15) / 100.0f);
        pattern.getTrack(1).getStep(8).setActive(true);
        pattern.getTrack(1).getStep(8).setVelocity(0.9f);
        pattern.getTrack(2).getStep(4).setActive(true);
        pattern.getTrack(2).getStep(4).setVelocity(0.4f);
        pattern.getTrack(2).getStep(4).setProbability(0.7f);
    }

    void StyleManager::generateRockSnare(Pattern &pattern, float intensity)
    {
        auto &snare = pattern.getTrack(1);
        for (int i = 0; i < pattern.getLength(); ++i)
            snare.getStep(i).setActive(false);

        if (intensity <= 0.3f)
        {
            snare.getStep(8).setActive(true);
            snare.getStep(8).setVelocity(randomFloat(0.8f, 0.9f));
        }
        else if (intensity <= 0.5f)
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(randomFloat(0.7f, 0.85f));
            snare.getStep(10).setActive(true);
            snare.getStep(10).setVelocity(randomFloat(0.7f, 0.85f));
        }
        else if (intensity <= 0.7f)
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(randomFloat(0.8f, 0.9f));
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(randomFloat(0.8f, 0.9f));
        }
        else if (intensity <= 0.85f)
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(randomFloat(0.8f, 0.9f));
            snare.getStep(6).setActive(true);
            snare.getStep(6).setVelocity(randomFloat(0.6f, 0.8f));
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(randomFloat(0.8f, 0.9f));
            snare.getStep(14).setActive(true);
            snare.getStep(14).setVelocity(randomFloat(0.6f, 0.8f));
        }
        else
        {
            for (int i = 2; i < 16; i += 2)
                if (randomChance(0.8f))
                {
                    snare.getStep(i).setActive(true);
                    snare.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
                }
        }
    }

    void StyleManager::generateRockHiHat(Pattern &pattern, float intensity)
    {
        auto &hh = pattern.getTrack(2);
        auto &ohh = pattern.getTrack(3);
        auto &hhped = pattern.getTrack(9);
        auto &splash = pattern.getTrack(10);
        auto &china = pattern.getTrack(11);
        for (int i = 0; i < 16; ++i)
        {
            hh.getStep(i).setActive(false);
            ohh.getStep(i).setActive(false);
            hhped.getStep(i).setActive(false);
            splash.getStep(i).setActive(false);
            china.getStep(i).setActive(false);
        }

        if (intensity <= 0.4f)
        {
            for (int i = 0; i < 16; i += 2)
            {
                hh.getStep(i).setActive(true);
                hh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
            }
        }
        else if (intensity <= 0.6f)
        {
            for (int i = 0; i < 16; i += 2)
            {
                if (i == 6 || i == 14)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
                }
                else
                {
                    hh.getStep(i).setActive(true);
                    hh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
                }
            }
        }
        else if (intensity <= 0.8f)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (i == 6 || i == 14)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
                }
                else
                {
                    hh.getStep(i).setActive(true);
                    hh.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
                }
            }
            hhped.getStep(2).setActive(true);
            hhped.getStep(2).setVelocity(0.4f);
            hhped.getStep(10).setActive(true);
            hhped.getStep(10).setVelocity(0.4f);
        }
        else
        {
            for (int i = 0; i < 16; ++i)
            {
                if (i == 0 && randomChance(0.6f))
                {
                    splash.getStep(i).setActive(true);
                    splash.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
                }
                else if (i == 8 && randomChance(0.4f))
                {
                    china.getStep(i).setActive(true);
                    china.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
                }
                else if (i == 6 || i == 14)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
                }
                else
                {
                    hh.getStep(i).setActive(true);
                    hh.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
                }
            }
            hhped.getStep(2).setActive(true);
            hhped.getStep(2).setVelocity(0.4f);
            hhped.getStep(10).setActive(true);
            hhped.getStep(10).setVelocity(0.4f);
        }
    }

    void StyleManager::generateRockBreak(Pattern &pattern, float intensity)
    {
        auto &kick = pattern.getTrack(0);
        auto &tomHi = pattern.getTrack(6);
        auto &tomLo = pattern.getTrack(7);
        auto &splash = pattern.getTrack(10);
        kick.getStep(0).setActive(true);
        kick.getStep(0).setVelocity(0.9f);
        kick.getStep(4).setActive(true);
        kick.getStep(4).setVelocity(0.85f);
        kick.getStep(8).setActive(true);
        kick.getStep(8).setVelocity(0.9f);
        kick.getStep(12).setActive(true);
        kick.getStep(12).setVelocity(0.8f);
        tomHi.getStep(2).setActive(true);
        tomHi.getStep(2).setVelocity(randomFloat(0.7f, 0.9f));
        tomHi.getStep(6).setActive(true);
        tomHi.getStep(6).setVelocity(randomFloat(0.6f, 0.8f));
        tomHi.getStep(10).setActive(true);
        tomHi.getStep(10).setVelocity(randomFloat(0.7f, 0.9f));
        tomHi.getStep(14).setActive(true);
        tomHi.getStep(14).setVelocity(randomFloat(0.8f, 0.95f));
        tomLo.getStep(1).setActive(true);
        tomLo.getStep(1).setVelocity(randomFloat(0.6f, 0.8f));
        tomLo.getStep(5).setActive(true);
        tomLo.getStep(5).setVelocity(randomFloat(0.7f, 0.9f));
        tomLo.getStep(13).setActive(true);
        tomLo.getStep(13).setVelocity(randomFloat(0.7f, 0.9f));
        splash.getStep(0).setActive(true);
        splash.getStep(0).setVelocity(randomFloat(0.8f, 1.0f));
        splash.getStep(8).setActive(true);
        splash.getStep(8).setVelocity(randomFloat(0.7f, 0.9f));
    }

}