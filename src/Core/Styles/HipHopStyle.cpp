#include "../StyleManager.h"

namespace BeatCrafter
{

    void StyleManager::generateHipHopPattern(Pattern &pattern)
    {
        pattern.clear();
        pattern.getTrack(0).getStep(0).setActive(true);
        pattern.getTrack(0).getStep(0).setVelocity(0.95f);
        pattern.getTrack(1).getStep(8).setActive(true);
        pattern.getTrack(1).getStep(8).setVelocity(0.9f);
        pattern.getTrack(2).getStep(4).setActive(true);
        pattern.getTrack(2).getStep(4).setVelocity(0.45f);
        pattern.getTrack(2).getStep(12).setActive(true);
        pattern.getTrack(2).getStep(12).setVelocity(0.35f);
        pattern.getTrack(2).getStep(12).setProbability(0.6f);
    }

    void StyleManager::generateHipHopSnare(Pattern &pattern, float intensity)
    {
        auto &snare = pattern.getTrack(1);
        for (int i = 0; i < pattern.getLength(); ++i)
            snare.getStep(i).setActive(false);

        if (intensity <= 0.4f)
        {
            snare.getStep(8).setActive(true);
            snare.getStep(8).setVelocity(0.9f);
        }
        else if (intensity <= 0.6f)
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(0.9f);
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(0.9f);
        }
        else if (intensity <= 0.8f)
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(0.9f);
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(0.9f);
            addGhostNotes(pattern, 1, intensity * 0.7f);
        }
        else
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(1.0f);
            snare.getStep(6).setActive(true);
            snare.getStep(6).setVelocity(0.8f);
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(1.0f);
            snare.getStep(14).setActive(true);
            snare.getStep(14).setVelocity(0.8f);
        }
    }

    void StyleManager::generateHipHopHiHat(Pattern &pattern, float intensity)
    {
        auto &hh = pattern.getTrack(2);
        auto &ohh = pattern.getTrack(3);
        auto &hhped = pattern.getTrack(9);
        auto &splash = pattern.getTrack(10);
        for (int i = 0; i < 16; ++i)
        {
            hh.getStep(i).setActive(false);
            ohh.getStep(i).setActive(false);
            hhped.getStep(i).setActive(false);
            splash.getStep(i).setActive(false);
        }

        if (intensity <= 0.4f)
        {
            for (int i = 2; i < 16; i += 4)
            {
                hh.getStep(i).setActive(true);
                hh.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
            }
        }
        else if (intensity <= 0.6f)
        {
            for (int i = 2; i < 16; i += 4)
            {
                hh.getStep(i).setActive(true);
                hh.getStep(i).setVelocity(randomFloat(0.4f, 0.6f));
            }
            for (int i = 1; i < 16; i += 4)
            {
                hh.getStep(i).setActive(true);
                hh.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
            }
        }
        else if (intensity <= 0.8f)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (i == 3 || i == 11)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
                }
                else if (i % 2 == 1 || i % 4 == 2)
                {
                    hh.getStep(i).setActive(true);
                    hh.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
                }
            }
            hhped.getStep(6).setActive(true);
            hhped.getStep(6).setVelocity(0.3f);
            hhped.getStep(14).setActive(true);
            hhped.getStep(14).setVelocity(0.3f);
        }
        else
        {
            for (int i = 0; i < 16; ++i)
            {
                if (i == 0 && randomChance(0.5f))
                {
                    splash.getStep(i).setActive(true);
                    splash.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
                }
                else if (i == 3 || i == 11)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.5f, 0.7f));
                }
                else if (i % 2 == 1 || i % 4 == 2)
                {
                    hh.getStep(i).setActive(true);
                    hh.getStep(i).setVelocity(randomFloat(0.3f, 0.5f));
                }
            }
            hhped.getStep(6).setActive(true);
            hhped.getStep(6).setVelocity(0.3f);
            hhped.getStep(14).setActive(true);
            hhped.getStep(14).setVelocity(0.3f);
        }
    }

    void StyleManager::generateHipHopBreak(Pattern &pattern, float intensity)
    {
        auto &kick = pattern.getTrack(0);
        auto &tomHi = pattern.getTrack(6);
        auto &tomLo = pattern.getTrack(7);
        auto &splash = pattern.getTrack(10);
        kick.getStep(0).setActive(true);
        kick.getStep(0).setVelocity(0.95f);
        kick.getStep(7).setActive(true);
        kick.getStep(7).setVelocity(0.6f);
        kick.getStep(11).setActive(true);
        kick.getStep(11).setVelocity(0.7f);
        for (int s : {2, 4, 6, 12, 14})
        {
            tomHi.getStep(s).setActive(true);
            tomHi.getStep(s).setVelocity(randomFloat(0.6f, 0.9f));
        }
        for (int s : {1, 8, 15})
        {
            tomLo.getStep(s).setActive(true);
            tomLo.getStep(s).setVelocity(randomFloat(0.6f, 0.9f));
        }
        splash.getStep(0).setActive(true);
        splash.getStep(0).setVelocity(randomFloat(0.7f, 0.9f));
    }

}