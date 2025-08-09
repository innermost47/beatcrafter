#include "PluginEditor.h"

namespace BeatCrafter {

	BeatCrafterEditor::BeatCrafterEditor(BeatCrafterProcessor& p)
		: AudioProcessorEditor(&p), processor(p) {

		setLookAndFeel(&modernLookAndFeel);
		setupComponents();
		setSize(900, 600);
		startTimerHz(30);
	}

	BeatCrafterEditor::~BeatCrafterEditor() {
		setLookAndFeel(nullptr);
		stopTimer();
	}

	void BeatCrafterEditor::setupComponents() {
		// Pattern Grid
		patternGrid = std::make_unique<PatternGrid>();
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
		addAndMakeVisible(patternGrid.get());

		// Slot Manager
		slotManager = std::make_unique<SlotManager>(processor.getPatternEngine());
		addAndMakeVisible(slotManager.get());

		// Intensity Slider
		intensitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
		intensitySlider.setRange(0.0, 1.0);
		intensitySlider.setValue(processor.intensityParam->get());
		intensitySlider.onValueChange = [this]() {
			processor.intensityParam->setValueNotifyingHost(
				intensitySlider.getValue());
			};
		addAndMakeVisible(intensitySlider);

		intensityLabel.setText("Intensity", juce::dontSendNotification);
		intensityLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(intensityLabel);

		// Style Combo
		styleCombo.addItemList(processor.styleParam->choices, 1);
		styleCombo.setSelectedId(processor.styleParam->getIndex() + 1);
		styleCombo.onChange = [this]() {
			processor.styleParam->setValueNotifyingHost(
				styleCombo.getSelectedId() - 1);
			};
		addAndMakeVisible(styleCombo);

		styleLabel.setText("Style", juce::dontSendNotification);
		styleLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(styleLabel);

		// Buttons
		generateButton.onClick = [this]() { onGenerateClicked(); };
		addAndMakeVisible(generateButton);

		clearButton.onClick = [this]() { onClearClicked(); };
		addAndMakeVisible(clearButton);

		playButton.onClick = [this]() { onPlayClicked(); };
		playButton.setToggleState(processor.playParam->get(), juce::dontSendNotification);
		addAndMakeVisible(playButton);
	}

	void BeatCrafterEditor::paint(juce::Graphics& g) {
		g.fillAll(modernLookAndFeel.backgroundDark);

		// Title
		g.setColour(modernLookAndFeel.accent);
		g.setFont(24.0f);
		g.drawText("BEATCRAFTER", 20, 10, 200, 30, juce::Justification::left);

		// Version
		g.setColour(modernLookAndFeel.textDimmed);
		g.setFont(10.0f);
		g.drawText("v0.1.0", 220, 20, 50, 20, juce::Justification::left);
	}

	void BeatCrafterEditor::resized() {
		auto bounds = getLocalBounds();

		// Top controls area
		auto topArea = bounds.removeFromTop(100);
		topArea.removeFromTop(40); // Title space

		auto controlsArea = topArea.reduced(20, 0);

		// Style selector
		styleLabel.setBounds(controlsArea.removeFromLeft(40));
		styleCombo.setBounds(controlsArea.removeFromLeft(120).reduced(0, 10));
		controlsArea.removeFromLeft(20);

		// Buttons
		generateButton.setBounds(controlsArea.removeFromLeft(80).reduced(0, 10));
		controlsArea.removeFromLeft(10);
		clearButton.setBounds(controlsArea.removeFromLeft(80).reduced(0, 10));
		controlsArea.removeFromLeft(10);
		playButton.setBounds(controlsArea.removeFromLeft(80).reduced(0, 10));

		// Intensity slider at bottom
		auto bottomArea = bounds.removeFromBottom(60);
		intensityLabel.setBounds(bottomArea.removeFromLeft(80).reduced(20, 0));
		intensitySlider.setBounds(bottomArea.reduced(20, 15));

		jassert(slotManager != nullptr);
		jassert(patternGrid != nullptr);
		// Slot manager
		auto slotArea = bounds.removeFromTop(60);
		slotManager->setBounds(slotArea.reduced(20, 10));

		// Pattern grid takes remaining space
		patternGrid->setBounds(bounds.reduced(20, 10));
	}

	void BeatCrafterEditor::timerCallback() {
		// Update play button state
		bool isPlaying = processor.getPatternEngine().getIsPlaying();
		if (playButton.getToggleState() != isPlaying) {
			playButton.setToggleState(isPlaying, juce::dontSendNotification);
		}

		// Update slot states
		slotManager->updateSlotStates();
	}

	void BeatCrafterEditor::onGenerateClicked() {
		auto style = static_cast<StyleType>(processor.styleParam->getIndex());
		float complexity = processor.intensityParam->get();
		processor.getPatternEngine().generateNewPattern(style, complexity);
		patternGrid->repaint();
	}

	void BeatCrafterEditor::onClearClicked() {
		processor.getPatternEngine().getCurrentPattern().clear();
		patternGrid->repaint();
	}

	void BeatCrafterEditor::onPlayClicked() {
		bool newState = !processor.playParam->get();
		processor.playParam->setValueNotifyingHost(newState);

		if (newState) {
			processor.getPatternEngine().start();
		}
		else {
			processor.getPatternEngine().stop();
		}
	}

} // namespace BeatCrafter