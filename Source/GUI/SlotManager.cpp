#include "SlotManager.h"

namespace BeatCrafter {

	void SlotButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
		bool shouldDrawButtonAsDown) {
		if (!lookAndFeel) {
			lookAndFeel = dynamic_cast<ModernLookAndFeel*>(&getLookAndFeel());
		}

		auto bounds = getLocalBounds().toFloat().reduced(2);

		// Background color based on state
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
			bgColour = bgColour.darker(0.2f);
		}
		else if (shouldDrawButtonAsHighlighted) {
			bgColour = bgColour.brighter(0.2f);
		}

		// Draw background
		g.setColour(bgColour);
		g.fillRoundedRectangle(bounds, 6.0f);

		// Draw border for active slot
		if (isActive) {
			g.setColour(lookAndFeel->accent);
			g.drawRoundedRectangle(bounds, 6.0f, 2.0f);
		}

		// Draw text
		g.setColour(isActive || hasPattern ? juce::Colours::white : lookAndFeel->textDimmed);
		g.setFont(16.0f);
		g.drawText(getButtonText(), bounds, juce::Justification::centred);

		// LED indicator
		float ledSize = 8.0f;
		auto ledBounds = juce::Rectangle<float>(bounds.getRight() - ledSize - 4,
			bounds.getY() + 4,
			ledSize, ledSize);

		if (isActive) {
			// Glowing effect
			g.setColour(lookAndFeel->accent.withAlpha(0.3f));
			g.fillEllipse(ledBounds.expanded(2));

			g.setColour(lookAndFeel->accent);
			g.fillEllipse(ledBounds);
		}
		else if (hasPattern) {
			g.setColour(lookAndFeel->slotLoaded.withAlpha(0.5f));
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
		patternEngine.switchToSlot(slot, false);
		updateSlotStates();
	}

} // namespace BeatCrafter