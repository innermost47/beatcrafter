#include "../StyleManager.h"

namespace BeatCrafter
{

    void StyleManager::generateElectronicPattern(Pattern &pattern)
    {
        pattern.clear();
        for (int i : {0, 4, 8, 12})
        {
            pattern.getTrack(0).getStep(i).setActive(true);
            pattern.getTrack(0).getStep(i).setVelocity(0.9f);
        }
        pattern.getTrack(1).getStep(4).setActive(true);
        pattern.getTrack(1).getStep(4).setVelocity(0.7f);
        pattern.getTrack(1).getStep(12).setActive(true);
        pattern.getTrack(1).getStep(12).setVelocity(0.7f);
        for (int i : {2, 6, 10, 14})
        {
            pattern.getTrack(2).getStep(i).setActive(true);
            pattern.getTrack(2).getStep(i).setVelocity(0.4f);
        }
    }

    void StyleManager::generateElectronicSnare(Pattern &pattern, float intensity)
    {
        auto &snare = pattern.getTrack(1);
        auto &kick = pattern.getTrack(0);
        for (int i = 0; i < pattern.getLength(); ++i)
            snare.getStep(i).setActive(false);

        if (intensity <= 0.4f)
        {
            snare.getStep(8).setActive(true);
            snare.getStep(8).setVelocity(0.8f);
        }
        else if (intensity <= 0.6f)
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(0.8f);
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(0.8f);
        }
        else if (intensity <= 0.8f)
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(0.9f);
            snare.getStep(10).setActive(true);
            snare.getStep(10).setVelocity(0.7f);
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(0.8f);
        }
        else
        {
            for (int i = 0; i < 16; i += 2)
            {
                snare.getStep(i).setActive(true);
                snare.getStep(i).setVelocity(randomFloat(0.85f, 0.95f));
                kick.getStep(i).setActive(true);
                kick.getStep(i).setVelocity(randomFloat(0.9f, 1.0f));
            }
        }
    }

    void StyleManager::generateElectronicHiHat(Pattern &pattern, float intensity)
    {
        auto &hh = pattern.getTrack(2);
        auto &ohh = pattern.getTrack(3);
        auto &crash = pattern.getTrack(4);
        auto &splash = pattern.getTrack(10);
        for (int i = 0; i < 16; ++i)
        {
            hh.getStep(i).setActive(false);
            ohh.getStep(i).setActive(false);
            crash.getStep(i).setActive(false);
            splash.getStep(i).setActive(false);
        }

        if (intensity <= 0.5f)
        {
            for (int i = 0; i < 16; ++i)
            {
                hh.getStep(i).setActive(true);
                hh.getStep(i).setVelocity(0.3f + (i % 4 == 0 ? 0.2f : 0.0f));
            }
        }
        else if (intensity <= 0.7f)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (i == 2 || i == 10)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
                }
                else
                {
                    hh.getStep(i).setActive(true);
                    hh.getStep(i).setVelocity(0.3f + (i % 4 == 0 ? 0.2f : 0.0f));
                }
            }
        }
        else if (intensity <= 0.8f)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (i == 0 || i == 8)
                {
                    splash.getStep(i).setActive(true);
                    splash.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
                }
                else if (i == 2 || i == 10)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
                }
                else
                {
                    hh.getStep(i).setActive(true);
                    hh.getStep(i).setVelocity(0.3f + (i % 4 == 0 ? 0.2f : 0.0f));
                }
            }
        }
        else
        {
            if (randomChance(0.6f))
            {
                for (int i = 0; i < 16; i += 2)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.7f, 0.9f));
                }
            }
            else
            {
                for (int i = 0; i < 16; i += 2)
                {
                    crash.getStep(i).setActive(true);
                    crash.getStep(i).setVelocity(randomFloat(0.8f, 1.0f));
                }
            }
            if (randomChance(0.4f))
            {
                splash.getStep(4).setActive(true);
                splash.getStep(4).setVelocity(randomFloat(0.8f, 0.95f));
                splash.getStep(12).setActive(true);
                splash.getStep(12).setVelocity(randomFloat(0.8f, 0.95f));
            }
        }
    }

    void StyleManager::generateElectronicBreak(Pattern &pattern, float intensity)
    {
        auto &kick = pattern.getTrack(0);
        auto &tomHi = pattern.getTrack(6);
        auto &tomLo = pattern.getTrack(7);
        auto &splash = pattern.getTrack(10);
        for (int i : {0, 4, 8, 12})
        {
            kick.getStep(i).setActive(true);
            kick.getStep(i).setVelocity(i % 8 == 0 ? 0.9f : 0.85f);
        }
        for (int i : {2, 6, 10, 14})
        {
            tomHi.getStep(i).setActive(true);
            tomHi.getStep(i).setVelocity(0.6f + (i == 14 ? 0.2f : 0.0f));
        }
        for (int i : {1, 5, 9, 13})
        {
            tomLo.getStep(i).setActive(true);
            tomLo.getStep(i).setVelocity(0.6f + (i % 8 == 5 ? 0.1f : 0.0f));
        }
        splash.getStep(0).setActive(true);
        splash.getStep(0).setVelocity(0.8f);
        splash.getStep(8).setActive(true);
        splash.getStep(8).setVelocity(0.7f);
    }

}