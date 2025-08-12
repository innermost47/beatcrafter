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

		for (int i = 0; i < 8; ++i) {
			slotMidiLearnButtons[i].setButtonText("LEARN");
			slotMidiLearnButtons[i].onClick = [this, i]() { onSlotMidiLearnClicked(i); };
			addAndMakeVisible(slotMidiLearnButtons[i]);

			slotMidiLabels[i].setText("--", juce::dontSendNotification);
			slotMidiLabels[i].setFont(juce::Font(8.0f));
			slotMidiLabels[i].setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
			addAndMakeVisible(slotMidiLabels[i]);
		}

		updateMidiLearnButtons();

		intensitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
		intensitySlider.setRange(0.0, 1.0, 0.01);
		intensitySlider.setValue(processor.intensityParam->get(), juce::dontSendNotification);
		intensitySlider.onValueChange = [this]() {
			processor.intensityParam->setValueNotifyingHost(intensitySlider.getValue());
			updatePatternDisplay();
			};
		addAndMakeVisible(intensitySlider);

		intensityLabel.setText("Intensity", juce::dontSendNotification);
		intensityLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(intensityLabel);

		intensityLabel.setText("Intensity", juce::dontSendNotification);
		intensityLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(intensityLabel);

		intensityMidiLearnButton.onClick = [this]() { onIntensityMidiLearnClicked(); };
		addAndMakeVisible(intensityMidiLearnButton);

		intensityMidiLabel.setText("Not mapped", juce::dontSendNotification);
		intensityMidiLabel.setFont(juce::Font(10.0f));
		intensityMidiLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
		addAndMakeVisible(intensityMidiLabel);

		styleCombo.addItem("Rock", 1);
		styleCombo.addItem("Metal", 2);
		styleCombo.addItem("Jazz", 3);
		styleCombo.addItem("Funk", 4);
		styleCombo.addItem("Electronic", 5);
		styleCombo.addItem("HipHop", 6);
		styleCombo.addItem("Latin", 7);
		styleCombo.addItem("Punk", 8);

		updateStyleComboForCurrentSlot();

		styleCombo.onChange = [this]() {
			int selectedStyle = styleCombo.getSelectedId() - 1;
			int currentSlot = processor.getPatternEngine().getActiveSlot();
			float normalized = selectedStyle / (float)(processor.slotStyleParams[currentSlot]->choices.size() - 1);
			processor.slotStyleParams[currentSlot]->setValueNotifyingHost(normalized);
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
		int styleFromParam = processor.slotStyleParams[currentSlot]->getIndex();
		StyleType styleFromEngine = processor.getPatternEngine().getSlotStyle(currentSlot);
		if (static_cast<int>(styleFromEngine) != styleFromParam) {
			processor.getPatternEngine().setSlotStyle(currentSlot, static_cast<StyleType>(styleFromParam));
		}
		styleCombo.setSelectedId(styleFromParam + 1, juce::dontSendNotification);
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

		auto slotAreaWithMidi = bounds.removeFromTop(100);
		auto slotArea = slotAreaWithMidi.removeFromTop(60);
		slotManager->setBounds(slotArea.reduced(20, 10));

		auto slotMidiArea = slotAreaWithMidi.removeFromTop(40).reduced(20, 0);
		int slotWidth = slotMidiArea.getWidth() / 8;
		for (int i = 0; i < 8; ++i) {
			auto slotBounds = slotMidiArea.removeFromLeft(slotWidth).reduced(1, 0);
			auto buttonArea = slotBounds.removeFromTop(20);
			slotMidiLearnButtons[i].setBounds(buttonArea);
			slotMidiLabels[i].setBounds(slotBounds);
		}

		auto bottomAreaWithMidi = bounds.removeFromBottom(80);

		auto intensityArea = bottomAreaWithMidi.removeFromTop(50);
		intensityLabel.setBounds(intensityArea.removeFromLeft(80).reduced(20, 0));
		intensityMidiLearnButton.setBounds(intensityArea.removeFromRight(60).reduced(5, 5));
		intensitySlider.setBounds(intensityArea.reduced(20, 15));

		auto intensityMidiArea = bottomAreaWithMidi.removeFromTop(20);
		intensityMidiLabel.setBounds(intensityMidiArea.reduced(100, 0));

		patternGrid->setBounds(bounds.reduced(20, 10));
	}

	void BeatCrafterEditor::timerCallback() {
		patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
		patternGrid->repaint();
		slotManager->updateSlotStates();
		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::onGenerateClicked() {
		int currentSlot = processor.getPatternEngine().getActiveSlot();
		auto style = static_cast<StyleType>(processor.slotStyleParams[currentSlot]->getIndex());
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

	void BeatCrafterEditor::updateMidiLearnButtons() {
		if (processor.isMidiLearning() && processor.getMidiLearnTarget() == 0) {
			intensityMidiLearnButton.setButtonText("LISTENING...");
			intensityMidiLearnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
		}
		else {
			intensityMidiLearnButton.setButtonText("LEARN");
			intensityMidiLearnButton.setColour(juce::TextButton::buttonColourId, modernLookAndFeel.backgroundMid);
		}

		intensityMidiLabel.setText(processor.getMidiMappingDescription(0, -1), juce::dontSendNotification);

		for (int i = 0; i < 8; ++i) {
			if (processor.isMidiLearning() && processor.getMidiLearnTarget() >= 1 && processor.getMidiLearnSlot() == i) {
				slotMidiLearnButtons[i].setButtonText("!");
				slotMidiLearnButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::red);
			}
			else {
				slotMidiLearnButtons[i].setButtonText("L");
				slotMidiLearnButtons[i].setColour(juce::TextButton::buttonColourId, modernLookAndFeel.backgroundMid);
			}

			juce::String mappingText = processor.getMidiMappingDescription(1, i);
			slotMidiLabels[i].setText(mappingText == "Not mapped" ? "--" : mappingText, juce::dontSendNotification);
		}
	}

	void BeatCrafterEditor::onIntensityMidiLearnClicked() {
		if (processor.isMidiLearning()) {
			processor.stopMidiLearn();
		}
		else if (processor.hasMidiMapping(0, -1)) {
			processor.clearMidiMapping(0, -1);
		}
		else {
			processor.startMidiLearn(0, -1);
		}
		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::onSlotMidiLearnClicked(int slot) {
		if (processor.isMidiLearning()) {
			processor.stopMidiLearn();
		}
		else if (processor.hasMidiMapping(1, slot)) {
			processor.clearMidiMapping(1, slot);
		}
		else {
			processor.startMidiLearn(1, slot);
		}
		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::updateFromProcessorState() {
		intensitySlider.setValue(processor.intensityParam->get(), juce::dontSendNotification);
		int currentSlot = processor.getPatternEngine().getActiveSlot();
		int currentStyleIndex = processor.slotStyleParams[currentSlot]->getIndex();
		styleCombo.setSelectedId(currentStyleIndex + 1, juce::dontSendNotification);
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
		patternGrid->repaint();
		slotManager->updateSlotStates();
		updateMidiLearnButtons();
	}
}