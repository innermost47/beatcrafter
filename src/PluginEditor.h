#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "GUI/PatternGrid.h"
#include "GUI/SlotManager.h"
#include "GUI/LookAndFeel.h"

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

		int lastPlayheadPosition = -1;

		std::unique_ptr<PatternGrid> patternGrid;
		std::unique_ptr<SlotManager> slotManager;

		juce::Slider intensitySlider;
		juce::Label intensityLabel;

		juce::ComboBox styleCombo;
		juce::Label styleLabel;

		juce::TextButton generateButton{ "Generate" };
		juce::TextButton clearButton{ "Clear" };
		juce::TextButton resetMidiMappingsButton{ "Reset" };
		juce::ToggleButton liveJamButton{ "Live Jam" };
		bool isLiveJamActive = false;

		std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intensityAttachment;
		std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> styleAttachment;

		juce::TextButton intensityMidiLearnButton{ "LEARN" };
		juce::Label intensityMidiLabel;
		std::array<juce::TextButton, 8> slotMidiLearnButtons;
		std::array<juce::Label, 8> slotMidiLabels;

		juce::Slider liveJamIntensitySlider;
		juce::Label liveJamIntensityLabel;
		juce::TextButton liveJamIntensityMidiLearnButton{ "LEARN" };
		juce::Label liveJamIntensityMidiLabel;

		void updateMidiLearnButtons();
		void onIntensityMidiLearnClicked();
		void onSlotMidiLearnClicked(int slot);

		void setupComponents();
		void onGenerateClicked();
		void onClearClicked();
		void updatePatternDisplay();
		void updateStyleComboForCurrentSlot();
		void onResetMidiMappingsClicked();
		void onLiveJamToggled();
		void onLiveJamIntensityMidiLearnClicked();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatCrafterEditor)
	};

}