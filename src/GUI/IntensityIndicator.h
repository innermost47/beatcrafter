#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "LookAndFeel.h"

namespace BeatCrafter
{
    class IntensityIndicator : public juce::Component
    {
    public:
        IntensityIndicator(ModernLookAndFeel &laf) : laf(laf) {}

        void setValue(float v)
        {
            value = juce::jlimit(0.0f, 1.0f, v);
            repaint();
        }

        void setSurpriseMeActive(bool active)
        {
            surpriseMeActive = active;
            repaint();
        }

        void paint(juce::Graphics &g) override
        {
            auto bounds = getLocalBounds().toFloat().reduced(1.0f);
            auto cornerSize = 3.0f;

            g.setColour(laf.backgroundDark);
            g.fillRoundedRectangle(bounds, cornerSize);

            float fillHeight = bounds.getHeight() * value;
            auto fillBounds = bounds.removeFromBottom(fillHeight);
            juce::Colour barColour = surpriseMeActive ? laf.accent : laf.textDimmed;
            g.setColour(barColour.withAlpha(0.8f));
            g.fillRoundedRectangle(fillBounds, cornerSize);

            g.setColour(laf.backgroundLight);
            g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), cornerSize, 1.0f);
        }

    private:
        ModernLookAndFeel &laf;
        float value = 0.0f;
        bool surpriseMeActive = false;
    };
}