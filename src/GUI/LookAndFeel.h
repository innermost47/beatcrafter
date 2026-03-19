#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

namespace BeatCrafter {

	class ModernLookAndFeel : public juce::LookAndFeel_V4 {
	public:
		ModernLookAndFeel();
		~ModernLookAndFeel() override = default;

		static constexpr float fontSizeTitle = 28.0f;
		static constexpr float fontSizeLabel = 18.0f;
		static constexpr float fontSizeSmall = 11.0f;
		static constexpr float fontSizeTiny = 10.0f;
		static constexpr float fontSizeSlotButton = 18.0f;
		static constexpr float fontSizeGridLabel = 16.0f;
		static constexpr float fontSizeStepNumberLarge = 14.0f;
		static constexpr float fontSizeStepNumberSmall = 12.0f;
		static constexpr float fontSizeMidiLabel = 12.0f;

		static constexpr int iconSizeMain = 36;
		static constexpr int iconSizeMidi = 30;
		static constexpr int iconPadding = 4;
		static constexpr int iconPaddingHard = 6;

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

		std::unique_ptr<juce::Drawable> loadSVGWithColour(
			const char* svgData, int svgSize, juce::Colour colour)
		{
			auto svgText = juce::String::fromUTF8(svgData, svgSize);

			auto hexColour = colour.toDisplayString(false);
			svgText = svgText.replace("stroke=\"currentColor\"",
				"stroke=\"#" + hexColour + "\"");
			svgText = svgText.replace("fill=\"currentColor\"",
				"fill=\"#" + hexColour + "\"");

			auto xml = juce::XmlDocument::parse(svgText);
			if (xml)
				return juce::Drawable::createFromSVG(*xml);
			return nullptr;
		}

		void drawDrawableButton(juce::Graphics& g, juce::DrawableButton& button,
			bool shouldDrawButtonAsHighlighted,
			bool shouldDrawButtonAsDown) override
		{
			auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
			auto cornerSize = 6.0f;

			juce::Colour bg = backgroundMid;
			if (button.getToggleState())        bg = accent;
			else if (shouldDrawButtonAsDown)    bg = accentDark;
			else if (shouldDrawButtonAsHighlighted) bg = backgroundLight.brighter(0.1f);

			g.setColour(juce::Colours::black.withAlpha(0.2f));
			g.fillRoundedRectangle(bounds.translated(0, 1), cornerSize);

			g.setColour(bg);
			g.fillRoundedRectangle(bounds, cornerSize);

			g.setColour(button.getToggleState() ? accentLight : backgroundLight);
			g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
		}

		juce::Label* createSliderTextBox(juce::Slider& slider) override
		{
			auto* label = juce::LookAndFeel_V4::createSliderTextBox(slider);
			label->setColour(juce::Label::backgroundColourId, backgroundMid);
			label->setColour(juce::Label::outlineColourId, backgroundLight);
			return label;
		}

		juce::Font getPluginFont(float height = 14.0f)
		{
			if (pluginTypeface == nullptr)
				pluginTypeface = juce::Typeface::createSystemTypefaceFor(
					BinaryData::Exo2Regular_ttf,
					BinaryData::Exo2Regular_ttfSize);
			return juce::Font(pluginTypeface).withHeight(height);
		}

		juce::Font getBodyFont(float height = 14.0f)
		{
			if (bodyTypeface == nullptr)
				bodyTypeface = juce::Typeface::createSystemTypefaceFor(
					BinaryData::Exo2Regular_ttf,
					BinaryData::Exo2Regular_ttfSize);
			return juce::Font(bodyTypeface).withHeight(height);
		}

		juce::Font getLabelFont(juce::Label& label) override
		{
			auto customHeight = label.getProperties()["customFontHeight"];
			if (!customHeight.isVoid())
				return getBodyFont(static_cast<float>(customHeight));

			return getBodyFont(fontSizeLabel);
		}

		juce::Font getComboBoxFont(juce::ComboBox&) override
		{
			return getBodyFont();
		}

		juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
		{
			return getBodyFont().withHeight(juce::jmin(14.0f, buttonHeight * 0.6f));
		}

		juce::Font getPopupMenuFont() override
		{
			return getBodyFont();
		}

	private:
		juce::Typeface::Ptr pluginTypeface = nullptr;
		juce::Typeface::Ptr bodyTypeface = nullptr;
	};
}