#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace BeatCrafter {

	class ModernLookAndFeel : public juce::LookAndFeel_V4 {
	public:
		ModernLookAndFeel();
		~ModernLookAndFeel() override = default;

		// Override pour boutons custom
		void drawButtonBackground(juce::Graphics& g,
			juce::Button& button,
			const juce::Colour& backgroundColour,
			bool shouldDrawButtonAsHighlighted,
			bool shouldDrawButtonAsDown) override;

		// Override pour sliders modernes
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

		// Couleurs publiques pour usage dans les components
		juce::Colour backgroundDark;
		juce::Colour backgroundMid;
		juce::Colour backgroundLight;
		juce::Colour accent;
		juce::Colour accentDark;
		juce::Colour accentLight;
		juce::Colour textColour;
		juce::Colour textDimmed;

		juce::Colour stepActive;
		juce::Colour stepInactive;
		juce::Colour stepHover;
		juce::Colour stepPlaying;

		juce::Colour slotEmpty;
		juce::Colour slotLoaded;
		juce::Colour slotActive;
	};

} // namespace BeatCrafter
