#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace BeatCrafter {

	class ModernLookAndFeel : public juce::LookAndFeel_V4 {
	public:
		ModernLookAndFeel();
		~ModernLookAndFeel() override = default;

		void drawButtonBackground(juce::Graphics& g,
			juce::Button& button,
			const juce::Colour& backgroundColour,
			bool shouldDrawButtonAsHighlighted,
			bool shouldDrawButtonAsDown) override;

		void drawLinearSlider(juce::Graphics& g,
			int x, int y, int width, int height,
			float sliderPos, float minSliderPos, float maxSliderPos,
			const juce::Slider::SliderStyle style,
			juce::Slider& slider) override;

		void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
			float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
			juce::Slider& slider) override;

		void drawComboBox(juce::Graphics& g, int width, int height, bool,
			int, int, int, int, juce::ComboBox& box) override;

		void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
			const bool isSeparator, const bool isActive,
			const bool isHighlighted, const bool isTicked,
			const bool hasSubMenu, const juce::String& text,
			const juce::String& shortcutKeyText,
			const juce::Drawable* icon, const juce::Colour* textColour) override;

		juce::Font getPopupMenuFont();
		juce::Font getComboBoxFont(juce::ComboBox&);
		juce::Font getLabelFont(juce::Label&);
		juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight);

		juce::Colour backgroundDark = juce::Colour(0xff1a1a1a);
		juce::Colour backgroundMid = juce::Colour(0xff2d2d2d);
		juce::Colour backgroundLight = juce::Colour(0xff404040);
		juce::Colour accent = juce::Colour(0xff00bcd4);
		juce::Colour accentDark = juce::Colour(0xff0097a7);
		juce::Colour accentLight = juce::Colour(0xff4dd0e1);
		juce::Colour textColour = juce::Colour(0xfff0f0f0);
		juce::Colour textDimmed = juce::Colour(0xff888888);

		juce::Colour stepActive = juce::Colour(0xff00bcd4);
		juce::Colour stepInactive = juce::Colour(0xff404040);
		juce::Colour stepHover = juce::Colour(0xff4dd0e1);
		juce::Colour stepPlaying = juce::Colour(0xffff6b35);

		juce::Colour slotEmpty = juce::Colour(0xff2d2d2d);
		juce::Colour slotLoaded = juce::Colour(0xff404040);
		juce::Colour slotActive = juce::Colour(0xff00bcd4);
	};

}