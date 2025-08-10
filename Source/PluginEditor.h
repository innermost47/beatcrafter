#pragma once

#include <juce_gui_basics/juce_gui_basics.h>      
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "GUI/PatternGrid.h"
#include "GUI/SlotManager.h"
#include "GUI/LookAndFeel.h"

namespace BeatCrafter {

	class BeatCrafterEditor : public juce::AudioProcessorEditor,
		public juce::Timer {
	public:
		BeatCrafterEditor(BeatCrafterProcessor&);
		~BeatCrafterEditor() override;

		void paint(juce::Graphics&) override;
		void resized() override;
		void timerCallback() override;

	private:
		BeatCrafterProcessor& processor;
		ModernLookAndFeel modernLookAndFeel;

		int lastPlayheadPosition = -1;

		// GUI Components
		std::unique_ptr<PatternGrid> patternGrid;
		std::unique_ptr<SlotManager> slotManager;

		juce::Slider intensitySlider;
		juce::Label intensityLabel;

		juce::ComboBox styleCombo;
		juce::Label styleLabel;

		juce::TextButton generateButton{ "Generate" };
		juce::TextButton clearButton{ "Clear" };

		// Parameter attachments
		std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intensityAttachment;
		std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> styleAttachment;

		void setupComponents();
		void onGenerateClicked();
		void onClearClicked();
		void updatePatternDisplay();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatCrafterEditor)
	};

} // namespace BeatCrafter