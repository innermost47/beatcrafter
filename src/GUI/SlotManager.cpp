#include "SlotManager.h"

namespace BeatCrafter {

	void SlotButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
		bool shouldDrawButtonAsDown) {
		if (!lookAndFeel) {
			lookAndFeel = dynamic_cast<ModernLookAndFeel*>(&getLookAndFeel());
		}

		auto bounds = getLocalBounds().toFloat().reduced(2);

		juce::Colour bgColour;
		if (isActive) {
			bgColour = lookAndFeel->slotActive;
		}
		else if (hasPattern) {
			bgColour = lookAndFeel->slotLoaded;
		}
		else {
			bgColour = lookAndFeel->slotEmpty;
		}

		if (shouldDrawButtonAsDown) {
			bgColour = bgColour.darker(0.3f);
		}
		else if (shouldDrawButtonAsHighlighted) {
			bgColour = bgColour.brighter(0.3f);
		}

		g.setColour(bgColour);
		g.fillRoundedRectangle(bounds, 6.0f);

		if (isActive) {
			g.setColour(lookAndFeel->accent);
			g.drawRoundedRectangle(bounds, 6.0f, 3.0f);

			g.setColour(lookAndFeel->accent.withAlpha(0.3f));
			g.drawRoundedRectangle(bounds.expanded(2), 8.0f, 2.0f);
		}

		g.setColour(isActive ? juce::Colours::black :
			hasPattern ? juce::Colours::white :
			lookAndFeel->textDimmed);
		g.setFont(16.0f);
		g.drawText(getButtonText(), bounds, juce::Justification::centred);

		float ledSize = 10.0f;
		auto ledBounds = juce::Rectangle<float>(bounds.getRight() - ledSize - 4,
			bounds.getY() + 4,
			ledSize, ledSize);

		if (isActive) {
			g.setColour(lookAndFeel->accent.withAlpha(0.5f));
			g.fillEllipse(ledBounds.expanded(3));

			g.setColour(lookAndFeel->accent);
			g.fillEllipse(ledBounds);

			g.setColour(juce::Colours::white);
			g.fillEllipse(ledBounds.reduced(3));
		}
		else if (hasPattern) {
			g.setColour(lookAndFeel->slotLoaded.brighter());
			g.fillEllipse(ledBounds);
		}
	}

	SlotManager::SlotManager(PatternEngine& engine) : patternEngine(engine) {
		for (int i = 0; i < 8; ++i) {
			slotButtons[i] = std::make_unique<SlotButton>(i);
			slotButtons[i]->onClick = [this, i]() { onSlotClicked(i); };
			addAndMakeVisible(slotButtons[i].get());
		}

		updateSlotStates();
	}

	void SlotManager::resized() {
		auto bounds = getLocalBounds();
		int buttonSize = bounds.getWidth() / 8;

		for (int i = 0; i < 8; ++i) {
			slotButtons[i]->setBounds(i * buttonSize, 0, buttonSize, bounds.getHeight());
		}
	}

	void SlotManager::updateSlotStates() {
		int activeSlot = patternEngine.getActiveSlot();

		for (int i = 0; i < 8; ++i) {
			bool hasPattern = patternEngine.getSlot(i) != nullptr;
			bool isActive = (i == activeSlot);
			slotButtons[i]->setSlotState(hasPattern, isActive);
		}
	}

	void SlotManager::onSlotClicked(int slot) {
		if (!patternEngine.getSlot(slot)) {
			StyleType slotStyle = patternEngine.getSlotStyle(slot);
			patternEngine.generateNewPatternForSlot(slot, slotStyle, 0.5f);
		}

		patternEngine.switchToSlot(slot, true);
		updateSlotStates();

		if (onSlotChanged) {
			onSlotChanged(slot);
		}
	}

}