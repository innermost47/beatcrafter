#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "LookAndFeel.h"

namespace BeatCrafter
{
	class IconButton : public juce::DrawableButton
	{
	public:
		IconButton(const juce::String& name,
			const char* svgData, int svgSize,
			ModernLookAndFeel& laf,
			bool isToggle = false)
			: juce::DrawableButton(name, juce::DrawableButton::ImageRaw),
			laf(laf)
		{
			setClickingTogglesState(isToggle);
			setIconFromSVG(svgData, svgSize, laf.textColour);
			setMouseCursor(juce::MouseCursor::PointingHandCursor);
		}

		void setIconFromSVG(const char* svgData, int svgSize,
			juce::Colour iconColour,
			juce::Colour bgColour = juce::Colours::transparentBlack)
		{
			auto normal = laf.loadSVGWithColour(svgData, svgSize, iconColour);
			auto hover = laf.loadSVGWithColour(svgData, svgSize,
				iconColour.brighter(0.3f));
			auto pressed = laf.loadSVGWithColour(svgData, svgSize,
				laf.accent);

			if (normal && hover && pressed)
				setImages(normal.get(), hover.get(), pressed.get());

			setColour(juce::DrawableButton::backgroundColourId, bgColour);
			repaint();
		}

		void paintButton(juce::Graphics& g,
			bool shouldDrawButtonAsHighlighted,
			bool shouldDrawButtonAsDown) override
		{
			getLookAndFeel().drawDrawableButton(g, *this,
				shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

			if (auto* drawable = getCurrentImage())
			{
				auto bounds = getLocalBounds().toFloat().reduced(iconPadding);
				drawable->drawWithin(g, bounds,
					juce::RectanglePlacement::centred, 1.0f);
			}
		}


		void setIconPadding(float padding)
		{
			iconPadding = padding;
			repaint();
		}

	private:
		ModernLookAndFeel& laf;
		float iconPadding = 4.0f;
	};
}