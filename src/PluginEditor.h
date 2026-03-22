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
		BeatCrafterEditor(BeatCrafterProcessor &);
		~BeatCrafterEditor() override;
		void paint(juce::Graphics &) override;
		void resized() override;
		void timerCallback() override;
		void updateIntensitySlider(float newIntensity);
		void updateSlotButtons(int activeSlot);
		void updateFromProcessorState();
		void updateLiveJamIntensitySlider(float newIntensity);
		void updateMidiLearnButtons();

	private:
		BeatCrafterProcessor &processor;
		ModernLookAndFeel modernLookAndFeel;
		std::unique_ptr<juce::TooltipWindow> tooltipWindow;

		std::unique_ptr<PatternGrid> patternGrid;
		std::unique_ptr<SlotManager> slotManager;

		juce::Slider intensitySlider;
		juce::Label intensityLabel;
		IconButton intensityMidiLearnButton;
		juce::Label intensityMidiLabel;
		IconButton surpriseMeButton;
		IconButton surpriseMeMidiLearnButton;
		juce::Label surpriseMeMidiLabel;
		IconButton tripletModeButton;
		IconButton tripletModeMidiLearnButton;
		juce::Label tripletModeMidiLabel;

		float lastRepaintIntensity = -1.0f;

		juce::Slider liveJamIntensitySlider;
		juce::Label liveJamIntensityLabel;
		IconButton liveJamIntensityMidiLearnButton;
		juce::Label liveJamIntensityMidiLabel;

		std::array<std::unique_ptr<IconButton>, 8> slotMidiLearnButtons;
		std::array<juce::Label, 8> slotMidiLabels;

		void setupComponents();
		void onIntensityMidiLearnClicked();
		void onSlotMidiLearnClicked(int slot);
		void updatePatternDisplay();
		void onLiveJamIntensityMidiLearnClicked();
		void onSurpriseMeMidiLearnClicked();
		void onTripletModeMidiLearnClicked();
		void layoutToggleBlock(juce::Rectangle<int> zone,
							   juce::Component &toggleButton,
							   juce::Component &midiLearnButton,
							   juce::Label &midiLabel,
							   int buttonSize = 32);
		void layoutSliderBlock(juce::Rectangle<int> zone,
							   juce::Label &label,
							   juce::Slider &slider,
							   juce::Component &midiLearnButton,
							   juce::Label &midiLabel,
							   int sliderWidth,
							   int gridBottom,
							   int midiAreaHeight);
		void applyToggleButtonState(IconButton &btn,
									const char *svgData, int svgSize,
									bool isActive);
		void applyMidiLearnButtonState(IconButton &btn,
									   bool isLearning,
									   bool hasMappingResult,
									   const juce::String &mappingDescription,
									   const juce::String &defaultTooltip);
		void updateMidiLearnPair(IconButton &btn,
								 juce::Label &label,
								 bool isLearning,
								 bool hasMapping,
								 const juce::String &mappingDescription,
								 const juce::String &defaultTooltip);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatCrafterEditor)
	};
}