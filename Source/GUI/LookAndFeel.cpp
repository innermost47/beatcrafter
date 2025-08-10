#include "LookAndFeel.h"

namespace BeatCrafter {

	ModernLookAndFeel::ModernLookAndFeel() {
		// Set all the colours for the modern dark theme

		// Window background
		setColour(juce::ResizableWindow::backgroundColourId, backgroundDark);
		setColour(juce::DocumentWindow::textColourId, textColour);

		// Default component colors
		setColour(juce::TextEditor::backgroundColourId, backgroundMid);
		setColour(juce::TextEditor::textColourId, textColour);
		setColour(juce::TextEditor::outlineColourId, backgroundLight);

		// Labels
		setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
		setColour(juce::Label::textColourId, textColour);
		setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);

		// Buttons
		setColour(juce::TextButton::buttonColourId, backgroundMid);
		setColour(juce::TextButton::buttonOnColourId, accent);
		setColour(juce::TextButton::textColourOffId, textColour);
		setColour(juce::TextButton::textColourOnId, backgroundDark);

		// ComboBox
		setColour(juce::ComboBox::backgroundColourId, backgroundMid);
		setColour(juce::ComboBox::textColourId, textColour);
		setColour(juce::ComboBox::outlineColourId, backgroundLight);
		setColour(juce::ComboBox::buttonColourId, backgroundLight);
		setColour(juce::ComboBox::arrowColourId, textDimmed);
		setColour(juce::ComboBox::focusedOutlineColourId, accent);

		// PopupMenu
		setColour(juce::PopupMenu::backgroundColourId, backgroundMid);
		setColour(juce::PopupMenu::textColourId, textColour);
		setColour(juce::PopupMenu::highlightedBackgroundColourId, accent);
		setColour(juce::PopupMenu::highlightedTextColourId, backgroundDark);
		setColour(juce::PopupMenu::headerTextColourId, textDimmed);

		// Slider
		setColour(juce::Slider::thumbColourId, accent);
		setColour(juce::Slider::trackColourId, backgroundLight);
		setColour(juce::Slider::backgroundColourId, backgroundDark);
		setColour(juce::Slider::rotarySliderFillColourId, accent);
		setColour(juce::Slider::rotarySliderOutlineColourId, backgroundLight);
		setColour(juce::Slider::textBoxTextColourId, textColour);
		setColour(juce::Slider::textBoxBackgroundColourId, backgroundMid);
		setColour(juce::Slider::textBoxHighlightColourId, accent);
		setColour(juce::Slider::textBoxOutlineColourId, backgroundLight);

		// ScrollBar
		setColour(juce::ScrollBar::backgroundColourId, backgroundDark);
		setColour(juce::ScrollBar::thumbColourId, backgroundLight);

		// TreeView
		setColour(juce::TreeView::backgroundColourId, backgroundDark);
		setColour(juce::TreeView::linesColourId, backgroundLight);
		setColour(juce::TreeView::dragAndDropIndicatorColourId, accent);
		setColour(juce::TreeView::selectedItemBackgroundColourId, accent.withAlpha(0.3f));

		// AlertWindow
		setColour(juce::AlertWindow::backgroundColourId, backgroundMid);
		setColour(juce::AlertWindow::textColourId, textColour);
		setColour(juce::AlertWindow::outlineColourId, backgroundLight);

		// ProgressBar
		setColour(juce::ProgressBar::backgroundColourId, backgroundLight);
		setColour(juce::ProgressBar::foregroundColourId, accent);

		// TooltipWindow
		setColour(juce::TooltipWindow::backgroundColourId, backgroundMid);
		setColour(juce::TooltipWindow::textColourId, textColour);
		setColour(juce::TooltipWindow::outlineColourId, backgroundLight);

		// CaretComponent
		setColour(juce::CaretComponent::caretColourId, accent);

		// GroupComponent
		setColour(juce::GroupComponent::outlineColourId, backgroundLight);
		setColour(juce::GroupComponent::textColourId, textDimmed);

		// BubbleComponent
		setColour(juce::BubbleComponent::backgroundColourId, backgroundMid);
		setColour(juce::BubbleComponent::outlineColourId, backgroundLight);

		// DirectoryContentsDisplayComponent
		setColour(juce::DirectoryContentsDisplayComponent::highlightColourId, accent.withAlpha(0.3f));
		setColour(juce::DirectoryContentsDisplayComponent::textColourId, textColour);
		setColour(juce::DirectoryContentsDisplayComponent::highlightedTextColourId, textColour);

	}

	void ModernLookAndFeel::drawButtonBackground(juce::Graphics& g,
		juce::Button& button,
		const juce::Colour& backgroundColour,
		bool shouldDrawButtonAsHighlighted,
		bool shouldDrawButtonAsDown) {

		auto cornerSize = 6.0f;
		auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

		auto baseColour = backgroundColour;

		if (button.getToggleState()) {
			baseColour = accent;
		}
		else if (shouldDrawButtonAsDown) {
			baseColour = accentDark;
		}
		else if (shouldDrawButtonAsHighlighted) {
			baseColour = backgroundColour.brighter(0.2f);
		}

		// Draw shadow
		g.setColour(juce::Colours::black.withAlpha(0.2f));
		g.fillRoundedRectangle(bounds.translated(0, 1), cornerSize);

		// Draw main button
		g.setColour(baseColour);
		g.fillRoundedRectangle(bounds, cornerSize);

		// Draw subtle gradient overlay
		juce::ColourGradient gradient(baseColour.brighter(0.1f), 0, bounds.getY(),
			baseColour.darker(0.1f), 0, bounds.getBottom(),
			false);
		g.setGradientFill(gradient);
		g.fillRoundedRectangle(bounds, cornerSize);

		// Draw border
		g.setColour(button.getToggleState() ? accentLight : backgroundLight);
		g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
	}

	void ModernLookAndFeel::drawLinearSlider(juce::Graphics& g,
		int x, int y, int width, int height,
		float sliderPos, float minSliderPos, float maxSliderPos,
		const juce::Slider::SliderStyle style,
		juce::Slider& slider) {

		if (style == juce::Slider::LinearHorizontal || style == juce::Slider::LinearVertical) {
			auto isHorizontal = style == juce::Slider::LinearHorizontal;

			// Calculate track bounds
			auto trackWidth = isHorizontal ? width : 6.0f;
			auto trackHeight = isHorizontal ? 6.0f : height;
			auto trackX = isHorizontal ? x : x + (width - trackWidth) * 0.5f;
			auto trackY = isHorizontal ? y + (height - trackHeight) * 0.5f : y;

			// Draw track background
			g.setColour(backgroundLight);
			g.fillRoundedRectangle(trackX, trackY, trackWidth, trackHeight, 3.0f);

			// Draw filled portion
			g.setColour(accent);
			if (isHorizontal) {
				auto filledWidth = sliderPos - x;
				g.fillRoundedRectangle(x, trackY, filledWidth, trackHeight, 3.0f);
			}
			else {
				auto filledHeight = y + height - sliderPos;
				g.fillRoundedRectangle(trackX, sliderPos, trackWidth, filledHeight, 3.0f);
			}

			// Draw thumb
			auto thumbWidth = 20.0f;
			auto thumbX = isHorizontal ? sliderPos - thumbWidth * 0.5f : x + width * 0.5f - thumbWidth * 0.5f;
			auto thumbY = isHorizontal ? y + height * 0.5f - thumbWidth * 0.5f : sliderPos - thumbWidth * 0.5f;

			// Thumb shadow
			g.setColour(juce::Colours::black.withAlpha(0.2f));
			g.fillEllipse(thumbX + 1, thumbY + 1, thumbWidth, thumbWidth);

			// Thumb outer ring
			g.setColour(accent);
			g.fillEllipse(thumbX, thumbY, thumbWidth, thumbWidth);

			// Thumb inner circle
			g.setColour(backgroundDark);
			g.fillEllipse(thumbX + thumbWidth * 0.25f, thumbY + thumbWidth * 0.25f,
				thumbWidth * 0.5f, thumbWidth * 0.5f);

			// Thumb highlight
			g.setColour(accentLight.withAlpha(0.3f));
			g.fillEllipse(thumbX + thumbWidth * 0.15f, thumbY + thumbWidth * 0.15f,
				thumbWidth * 0.4f, thumbWidth * 0.4f);
		}
	}

	void ModernLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
		float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
		juce::Slider& slider) {

		auto radius = juce::jmin(width / 2, height / 2) - 4.0f;
		auto centreX = x + width * 0.5f;
		auto centreY = y + height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

		// Background circle
		g.setColour(backgroundLight);
		g.fillEllipse(rx, ry, rw, rw);

		// Value arc
		juce::Path arcPath;
		arcPath.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, angle, 0.8f);
		g.setColour(accent);
		g.fillPath(arcPath);

		// Center knob
		auto knobRadius = radius * 0.6f;
		g.setColour(backgroundMid);
		g.fillEllipse(centreX - knobRadius, centreY - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

		// Pointer
		juce::Path pointer;
		auto pointerLength = radius * 0.4f;
		auto pointerThickness = 3.0f;
		pointer.addRectangle(-pointerThickness * 0.5f, -radius * 0.5f, pointerThickness, pointerLength);
		pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
		g.setColour(accent);
		g.fillPath(pointer);
	}

	void ModernLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
		int, int, int, int, juce::ComboBox& box) {

		auto cornerSize = 4.0f;
		auto bounds = juce::Rectangle<float>(0, 0, width, height).reduced(0.5f, 0.5f);

		// Background
		g.setColour(backgroundMid);
		g.fillRoundedRectangle(bounds, cornerSize);

		// Border
		g.setColour(box.hasKeyboardFocus(false) ? accent : backgroundLight);
		g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

		// Arrow
		juce::Path arrow;
		arrow.addTriangle(width - 20.0f, height * 0.5f - 3.0f,
			width - 10.0f, height * 0.5f - 3.0f,
			width - 15.0f, height * 0.5f + 3.0f);

		g.setColour(textDimmed);
		g.fillPath(arrow);
	}

	void ModernLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
		const bool isSeparator, const bool isActive,
		const bool isHighlighted, const bool isTicked,
		const bool hasSubMenu, const juce::String& text,
		const juce::String& shortcutKeyText,
		const juce::Drawable* icon, const juce::Colour* textColour) {

		if (isSeparator) {
			auto r = area.reduced(5, 0);
			r.removeFromTop(r.getHeight() / 2 - 1);
			g.setColour(backgroundLight);
			g.fillRect(r.removeFromTop(1));
			return;
		}

		// CORRECTION ICI - gestion sécurisée du pointeur textColour
		juce::Colour textColourToUse;
		if (textColour != nullptr) {
			textColourToUse = *textColour;
		}
		else if (isHighlighted) {
			textColourToUse = backgroundDark;
		}
		else {
			textColourToUse = this->textColour; // Utiliser la couleur de la classe
		}

		auto r = area.reduced(1);

		if (isHighlighted && isActive) {
			g.setColour(accent);
			g.fillRect(r);
			g.setColour(backgroundDark);
		}
		else {
			g.setColour(textColourToUse);
		}

		g.setFont(getPopupMenuFont());

		auto iconArea = r.removeFromLeft(20);

		if (icon != nullptr) {
			icon->drawWithin(g, iconArea.toFloat(), juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
		}
		else if (isTicked) {
			g.setColour(isHighlighted ? backgroundDark : accent);
			g.fillEllipse(iconArea.reduced(6).toFloat());
		}

		r.removeFromLeft(5);
		g.drawFittedText(text, r, juce::Justification::centredLeft, 1);

		if (shortcutKeyText.isNotEmpty()) {
			g.setFont(getPopupMenuFont().withHeight(14.0f));
			g.setColour(isHighlighted ? backgroundDark : textDimmed);
			g.drawText(shortcutKeyText, r, juce::Justification::centredRight, true);
		}
	}

	juce::Font ModernLookAndFeel::getPopupMenuFont() {
		return juce::Font(15.0f);
	}

	juce::Font ModernLookAndFeel::getComboBoxFont(juce::ComboBox&) {
		return juce::Font(14.0f);
	}

	juce::Font ModernLookAndFeel::getLabelFont(juce::Label&) {
		return juce::Font(14.0f);
	}

	juce::Font ModernLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight) {
		return juce::Font(juce::jmin(15.0f, buttonHeight * 0.6f));
	}

} // namespace BeatCrafter