#include "../StyleManager.h"

namespace BeatCrafter
{

    void StyleManager::generateFunkPattern(Pattern &pattern)
    {
        pattern.clear();
        pattern.getTrack(0).getStep(0).setActive(true);
        pattern.getTrack(0).getStep(0).setVelocity(0.9f);
        pattern.getTrack(0).getStep(6).setActive(true);
        pattern.getTrack(0).getStep(6).setVelocity(0.6f);
        pattern.getTrack(0).getStep(6).setProbability(0.75f);
        pattern.getTrack(1).getStep(8).setActive(true);
        pattern.getTrack(1).getStep(8).setVelocity(0.85f);
        pattern.getTrack(2).getStep(4).setActive(true);
        pattern.getTrack(2).getStep(4).setVelocity(0.5f);
        pattern.getTrack(2).getStep(12).setActive(true);
        pattern.getTrack(2).getStep(12).setVelocity(0.4f);
        pattern.getTrack(2).getStep(12).setProbability(0.8f);
    }

    void StyleManager::generateFunkSnare(Pattern &pattern, float intensity)
    {
        auto &snare = pattern.getTrack(1);
        for (int i = 0; i < pattern.getLength(); ++i)
            snare.getStep(i).setActive(false);

        if (intensity <= 0.3f)
        {
            snare.getStep(8).setActive(true);
            snare.getStep(8).setVelocity(0.9f);
            addGhostNotes(pattern, 1, 0.5f);
        }
        else if (intensity <= 0.6f)
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(0.9f);
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(0.85f);
            addGhostNotes(pattern, 1, intensity * 0.8f);
        }
        else
        {
            snare.getStep(4).setActive(true);
            snare.getStep(4).setVelocity(0.9f);
            snare.getStep(6).setActive(true);
            snare.getStep(6).setVelocity(0.3f);
            snare.getStep(12).setActive(true);
            snare.getStep(12).setVelocity(0.85f);
            snare.getStep(15).setActive(true);
            snare.getStep(15).setVelocity(0.4f);
            addGhostNotes(pattern, 1, intensity * 0.9f);
        }
    }

    void StyleManager::generateFunkHiHat(Pattern &pattern, float intensity)
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

        if (intensity <= 0.5f)
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
                    hh.getStep(i).setVelocity(randomFloat(0.4f, 0.7f));
                }
            }
        }
        else if (intensity <= 0.7f)
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
                    hh.getStep(i).setVelocity(randomFloat(0.4f, 0.7f));
                }
            }
            for (int i : {1, 5, 9, 13})
            {
                hhped.getStep(i).setActive(true);
                hhped.getStep(i).setVelocity(0.4f);
            }
        }
        else
        {
            for (int i = 0; i < 16; ++i)
            {
                if (i == 14 && randomChance(0.6f))
                {
                    splash.getStep(i).setActive(true);
                    splash.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
                }
                else if (i == 6)
                {
                    ohh.getStep(i).setActive(true);
                    ohh.getStep(i).setVelocity(randomFloat(0.6f, 0.8f));
                }
                else
                {
                    hh.getStep(i).setActive(true);
                    hh.getStep(i).setVelocity(randomFloat(0.4f, 0.7f));
                }
            }
            for (int i : {1, 5, 9, 13})
            {
                hhped.getStep(i).setActive(true);
                hhped.getStep(i).setVelocity(0.4f);
            }
        }
    }

    void StyleManager::generateFunkBreak(Pattern &pattern, float intensity)
    {
        auto &kick = pattern.getTrack(0);
        auto &tomHi = pattern.getTrack(6);
        auto &tomLo = pattern.getTrack(7);
        auto &splash = pattern.getTrack(10);
        int kicks[] = {0, 3, 7, 10};
        float kvels[] = {0.9f, 0.7f, 0.6f, 0.8f};
        for (int k = 0; k < 4; ++k)
        {
            kick.getStep(kicks[k]).setActive(true);
            kick.getStep(kicks[k]).setVelocity(kvels[k]);
        }
        int thSteps[] = {1, 4, 6, 12, 14};
        for (int s : thSteps)
        {
            tomHi.getStep(s).setActive(true);
            tomHi.getStep(s).setVelocity(randomFloat(0.6f, 0.9f));
        }
        int tlSteps[] = {2, 8, 15};
        for (int s : tlSteps)
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