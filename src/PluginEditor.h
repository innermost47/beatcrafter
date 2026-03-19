#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "GUI/PatternGrid.h"
#include "GUI/SlotManager.h"
#include "GUI/LookAndFeel.h"
#include "GUI/IconButton.h"
#include "BinaryData.h"

namespace BeatCrafter
{
	class BeatCrafterEditor : public juce::AudioProcessorEditor,
		public juce::Timer
	{
	public:
		BeatCrafterEditor(BeatCrafterProcessor&);
		~BeatCrafterEditor() override;
		void paint(juce::Graphics&) override;
		void resized() override;
		void timerCallback() override;
		void updateIntensitySlider(float newIntensity);
		void updateSlotButtons(int activeSlot);
		void updateFromProcessorState();
		void updateLiveJamIntensitySlider(float newIntensity);

	private:
		BeatCrafterProcessor& processor;
		ModernLookAndFeel modernLookAndFeel;
		std::unique_ptr<juce::TooltipWindow> tooltipWindow;

		std::unique_ptr<PatternGrid> patternGrid;
		std::unique_ptr<SlotManager> slotManager;

		juce::Slider   intensitySlider;
		juce::Label    intensityLabel;
		juce::ComboBox styleCombo;
		juce::Label    styleLabel;

		IconButton intensityMidiLearnButton;
		juce::Label intensityMidiLabel;

		std::array<std::unique_ptr<IconButton>, 8> slotMidiLearnButtons;
		std::array<juce::Label, 8>                 slotMidiLabels;

		juce::Slider liveJamIntensitySlider;
		juce::Label  liveJamIntensityLabel;
		IconButton   liveJamIntensityMidiLearnButton;
		juce::Label  liveJamIntensityMidiLabel;

		std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   intensityAttachment;
		std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> styleAttachment;

		void setupComponents();
		void updateMidiLearnButtons();
		void onIntensityMidiLearnClicked();
		void onSlotMidiLearnClicked(int slot);
		void updatePatternDisplay();
		void updateStyleComboForCurrentSlot(int slotIndex = -1);
		void onLiveJamIntensityMidiLearnClicked();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatCrafterEditor)
	};
}