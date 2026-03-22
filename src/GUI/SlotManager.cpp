#include "SlotManager.h"

namespace BeatCrafter
{

	void SlotButton::paintButton(juce::Graphics &g,
								 bool shouldDrawButtonAsHighlighted,
								 bool shouldDrawButtonAsDown)
	{
		if (!lookAndFeel)
			lookAndFeel = dynamic_cast<ModernLookAndFeel *>(&getLookAndFeel());

		auto bounds = getLocalBounds().toFloat().reduced(0.5f, 0.5f);
		auto cornerSize = 6.0f;

		juce::Colour bgColour = lookAndFeel->backgroundMid;
		if (shouldDrawButtonAsDown)
			bgColour = lookAndFeel->backgroundMid.darker(0.2f);
		else if (shouldDrawButtonAsHighlighted)
			bgColour = lookAndFeel->backgroundMid.brighter(0.1f);

		g.setColour(juce::Colours::black.withAlpha(0.2f));
		g.fillRoundedRectangle(bounds.translated(0, 1), cornerSize);

		g.setColour(bgColour);
		g.fillRoundedRectangle(bounds, cornerSize);

		if (isActive)
		{
			g.setColour(lookAndFeel->accent);
			g.drawRoundedRectangle(bounds, cornerSize, 2.0f);
		}
		else
		{
			g.setColour(lookAndFeel->backgroundLight);
			g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
		}
		juce::Colour textCol = isActive		? lookAndFeel->accent
							   : hasPattern ? lookAndFeel->textColour
											: lookAndFeel->textDimmed;
		g.setColour(textCol);
		g.setFont(lookAndFeel->getPluginFont(ModernLookAndFeel::fontSizeSlotButton));
		g.drawText(getButtonText(), bounds, juce::Justification::centred);
	}

	SlotManager::SlotManager(PatternEngine &engine) : patternEngine(engine)
	{
		for (int i = 0; i < 8; ++i)
		{
			StyleType slotStyle = patternEngine.getSlotStyle(i);
			slotButtons[i] = std::make_unique<SlotButton>(i);
			slotButtons[i]->setButtonText(styleTypeToString(slotStyle));
			slotButtons[i]->onClick = [this, i]()
			{ onSlotClicked(i); };
			slotButtons[i]->setMouseCursor(juce::MouseCursor::PointingHandCursor);
			addAndMakeVisible(slotButtons[i].get());
		}

		updateSlotStates();
	}

	juce::String SlotManager::styleTypeToString(StyleType style)
	{
		switch (style)
		{
		case StyleType::Rock:
			return "Rock";
		case StyleType::Metal:
			return "Metal";
		case StyleType::Jazz:
			return "Jazz";
		case StyleType::Funk:
			return "Funk";
		case StyleType::Electronic:
			return "Electronic";
		case StyleType::HipHop:
			return "Hip Hop";
		case StyleType::Latin:
			return "Latin";
		case StyleType::Punk:
			return "Punk";
		default:
			return "Unknown";
		}
	}

	void SlotManager::resized()
	{
		auto bounds = getLocalBounds();
		int totalWidth = bounds.getWidth();
		int gap = 4;
		int buttonWidth = (totalWidth - (gap * 7)) / 8;
		for (int i = 0; i < 8; ++i)
			slotButtons[i]->setBounds(i * (buttonWidth + gap), 0, buttonWidth, bounds.getHeight());
	}

	void SlotManager::updateSlotStates(int forceActiveSlot)
	{
		int activeSlot = forceActiveSlot >= 0 ? forceActiveSlot : patternEngine.getActiveSlot();
		for (int i = 0; i < 8; ++i)
		{
			bool hasPattern = patternEngine.getSlot(i) != nullptr;
			bool isActive = (i == activeSlot);
			slotButtons[i]->setSlotState(hasPattern, isActive);
			slotButtons[i]->setButtonText(styleTypeToString(patternEngine.getSlotStyle(i)));
		}
	}

	void SlotManager::onSlotClicked(int slot)
	{
		float currentIntensity = getIntensity ? getIntensity() : 0.5f;
		StyleType slotStyle = patternEngine.getSlotStyle(slot);
		if (!patternEngine.getSlot(slot))
			patternEngine.generateNewPatternForSlot(slot, slotStyle, currentIntensity);
		patternEngine.switchToSlot(slot, true, currentIntensity);

		for (int i = 0; i < 8; ++i)
		{
			bool hasPattern = patternEngine.getSlot(i) != nullptr;
			bool isActive = (i == slot);
			slotButtons[i]->setSlotState(hasPattern, isActive);
		}
		repaint();

		if (onSlotChanged)
			onSlotChanged(slot);
	}
}