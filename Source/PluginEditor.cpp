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
		patternGrid = std::make_unique<PatternGrid>();
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
		patternGrid->setPatternEngine(&processor.getPatternEngine());
		addAndMakeVisible(patternGrid.get());

		slotManager = std::make_unique<SlotManager>(processor.getPatternEngine());

		slotManager->onSlotChanged = [this](int slot) {
			updateStyleComboForCurrentSlot();
			patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
			patternGrid->repaint();
			};

		addAndMakeVisible(slotManager.get());
		intensitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
		intensitySlider.setRange(0.0, 1.0);
		intensitySlider.setValue(processor.intensityParam->get());
		intensitySlider.onValueChange = [this]() {
			processor.intensityParam->setValueNotifyingHost(intensitySlider.getValue());
			updatePatternDisplay();
			};
		addAndMakeVisible(intensitySlider);

		intensityLabel.setText("Intensity", juce::dontSendNotification);
		intensityLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(intensityLabel);

		styleCombo.addItemList(processor.styleParam->choices, 1);

		updateStyleComboForCurrentSlot();

		styleCombo.onChange = [this]() {
			int selectedStyle = styleCombo.getSelectedId() - 1;
			int currentSlot = processor.getPatternEngine().getActiveSlot();
			processor.slotStyleParams[currentSlot]->setValueNotifyingHost(selectedStyle);
			processor.getPatternEngine().setSlotStyle(currentSlot, static_cast<StyleType>(selectedStyle));
			onGenerateClicked();
			};
		addAndMakeVisible(styleCombo);

		styleLabel.setText("Style", juce::dontSendNotification);
		styleLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(styleLabel);

		generateButton.onClick = [this]() { onGenerateClicked(); };
		addAndMakeVisible(generateButton);

		clearButton.onClick = [this]() { onClearClicked(); };
		addAndMakeVisible(clearButton);
	}

	void BeatCrafterEditor::updateStyleComboForCurrentSlot() {
		int currentSlot = processor.getPatternEngine().getActiveSlot();
		StyleType currentStyle = processor.getPatternEngine().getSlotStyle(currentSlot);
		styleCombo.setSelectedId(static_cast<int>(currentStyle) + 1, juce::dontSendNotification);
	}

	void BeatCrafterEditor::updatePatternDisplay() {
		auto& engine = processor.getPatternEngine();
		float currentIntensity = processor.intensityParam->get();
		static float lastIntensity = -1.0f;
		if (currentIntensity != lastIntensity) {
			engine.regenerateSlotSeed(engine.getActiveSlot());
			lastIntensity = currentIntensity;
		}

		engine.setIntensity(currentIntensity);
		patternGrid->updateWithIntensity(*engine.getDisplayPattern());
	}

	void BeatCrafterEditor::paint(juce::Graphics& g) {
		g.fillAll(modernLookAndFeel.backgroundDark);
		g.setColour(modernLookAndFeel.accent);
		g.setFont(24.0f);
		g.drawText("BEATCRAFTER", 20, 10, 200, 30, juce::Justification::left);
		g.setColour(modernLookAndFeel.textDimmed);
		g.setFont(10.0f);
		g.drawText("v0.1.0", 220, 20, 50, 20, juce::Justification::left);
	}

	void BeatCrafterEditor::resized() {
		auto bounds = getLocalBounds();
		auto topArea = bounds.removeFromTop(100);
		topArea.removeFromTop(40);

		auto controlsArea = topArea.reduced(20, 0);

		styleLabel.setBounds(controlsArea.removeFromLeft(40));
		styleCombo.setBounds(controlsArea.removeFromLeft(120).reduced(0, 10));
		controlsArea.removeFromLeft(20);

		generateButton.setBounds(controlsArea.removeFromLeft(80).reduced(0, 10));
		controlsArea.removeFromLeft(10);
		clearButton.setBounds(controlsArea.removeFromLeft(80).reduced(0, 10));

		auto bottomArea = bounds.removeFromBottom(60);
		intensityLabel.setBounds(bottomArea.removeFromLeft(80).reduced(20, 0));
		intensitySlider.setBounds(bottomArea.reduced(20, 15));

		auto slotArea = bounds.removeFromTop(60);
		slotManager->setBounds(slotArea.reduced(20, 10));

		patternGrid->setBounds(bounds.reduced(20, 10));
	}

	void BeatCrafterEditor::timerCallback() {
		patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
		patternGrid->repaint();
		slotManager->updateSlotStates();
	}

	void BeatCrafterEditor::onGenerateClicked() {
		auto style = static_cast<StyleType>(processor.styleParam->getIndex());
		float complexity = processor.intensityParam->get();
		processor.getPatternEngine().generateNewPattern(style, complexity);
		patternGrid->repaint();
	}

	void BeatCrafterEditor::onClearClicked() {
		processor.getPatternEngine().clearCurrentPattern();
		intensitySlider.setValue(0.0f, juce::dontSendNotification);
		processor.intensityParam->setValueNotifyingHost(0.0f);
		patternGrid->repaint();
	}
}