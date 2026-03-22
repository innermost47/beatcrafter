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
			setColour(juce::DrawableButton::backgroundColourId,
				juce::Colours::transparentBlack);
			setColour(juce::DrawableButton::backgroundOnColourId,
				juce::Colours::transparentBlack);
		}

		void setIconFromSVG(const char* svgData, int svgSize,
			juce::Colour iconColour,
			juce::Colour bgColour = juce::Colours::transparentBlack)
		{
			auto normal = laf.loadSVGWithColour(svgData, svgSize, iconColour);
			auto hover = laf.loadSVGWithColour(svgData, svgSize, iconColour.brighter(0.3f));
			auto pressed = laf.loadSVGWithColour(svgData, svgSize, laf.accent);
			if (normal && hover && pressed)
				setImages(normal.get(), hover.get(), pressed.get());
			setColour(juce::DrawableButton::backgroundColourId, bgColour);
			setColour(juce::DrawableButton::backgroundOnColourId, bgColour);
			repaint();
		}

		void paintButton(juce::Graphics& g,
			bool shouldDrawButtonAsHighlighted,
			bool shouldDrawButtonAsDown) override
		{
			auto bounds = getLocalBounds().toFloat().reduced(0.5f, 0.5f);
			auto cornerSize = 6.0f;

			auto bgColour = laf.backgroundMid;
			if (shouldDrawButtonAsDown)
				bgColour = laf.backgroundMid.darker(0.2f);
			else if (shouldDrawButtonAsHighlighted)
				bgColour = laf.backgroundMid.brighter(0.1f);

			g.setColour(juce::Colours::black.withAlpha(0.2f));
			g.fillRoundedRectangle(bounds.translated(0, 1), cornerSize);

			g.setColour(bgColour);
			g.fillRoundedRectangle(bounds, cornerSize);

			g.setColour(getToggleState() ? laf.accentLight : laf.backgroundLight);
			g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

			if (auto* drawable = getCurrentImage())
			{
				auto iconBounds = getLocalBounds().toFloat().reduced(iconPadding);
				drawable->drawWithin(g, iconBounds,
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