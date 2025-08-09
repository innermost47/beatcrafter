#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Core/PatternEngine.h"
#include "LookAndFeel.h"

namespace BeatCrafter {

	class SlotButton : public juce::TextButton {
	public:
		SlotButton(int slotNumber) : slotNumber(slotNumber) {
			setButtonText(juce::String(slotNumber + 1));
		}

		void setSlotState(bool hasPattern, bool isActive) {
			this->hasPattern = hasPattern;
			this->isActive = isActive;
			repaint();
		}

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
			bool shouldDrawButtonAsDown) override;

	private:
		int slotNumber;
		bool hasPattern = false;
		bool isActive = false;
		ModernLookAndFeel* lookAndFeel = nullptr;
	};

	class SlotManager : public juce::Component {
	public:
		SlotManager(PatternEngine& engine);
		~SlotManager() override = default;

		void resized() override;
		void updateSlotStates();

	private:
		PatternEngine& patternEngine;
		std::array<std::unique_ptr<SlotButton>, 8> slotButtons;

		void onSlotClicked(int slot);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SlotManager)
	};

} // namespace BeatCrafter