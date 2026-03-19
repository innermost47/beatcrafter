#include "PluginEditor.h"

namespace BeatCrafter
{
	BeatCrafterEditor::BeatCrafterEditor(BeatCrafterProcessor& p)
		: AudioProcessorEditor(&p),
		processor(p),
		intensityMidiLearnButton("IntensityMidi",
			BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
			modernLookAndFeel),
		liveJamIntensityMidiLearnButton("LiveJamMidi",
			BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
			modernLookAndFeel)
	{
		tooltipWindow = std::make_unique<juce::TooltipWindow>(this);
		setLookAndFeel(&modernLookAndFeel);
		setupComponents();
		setSize(900, 600);
		startTimerHz(30);
	}

	BeatCrafterEditor::~BeatCrafterEditor()
	{
		tooltipWindow = nullptr;
		setLookAndFeel(nullptr);
		stopTimer();
	}

	void BeatCrafterEditor::setupComponents()
	{
		processor.setLiveJamMode(true);
		patternGrid = std::make_unique<PatternGrid>();
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
		patternGrid->setPatternEngine(&processor.getPatternEngine());
		addAndMakeVisible(patternGrid.get());

		slotManager = std::make_unique<SlotManager>(processor.getPatternEngine());
		slotManager->onSlotChanged = [this](int slot)
			{
				int styleFromParam = processor.slotStyleParams[slot]->getIndex();
				styleCombo.setSelectedId(styleFromParam + 1, juce::dontSendNotification);
				patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
				patternGrid->repaint();
			};
		slotManager->getIntensity = [this]()
			{
				return processor.intensityParam->get();
			};
		addAndMakeVisible(slotManager.get());

		for (int i = 0; i < 8; ++i)
		{
			slotMidiLearnButtons[i] = std::make_unique<IconButton>(
				"SlotMidi" + juce::String(i),
				BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
				modernLookAndFeel);
			slotMidiLearnButtons[i]->onClick = [this, i]()
				{ onSlotMidiLearnClicked(i); };
			slotMidiLearnButtons[i]->setTooltip("Assign a MIDI CC to slot " + juce::String(i + 1));
			addAndMakeVisible(slotMidiLearnButtons[i].get());

			slotMidiLabels[i].setText("--", juce::dontSendNotification);
			slotMidiLabels[i].getProperties().set("customFontHeight",
				ModernLookAndFeel::fontSizeMidiLabel);
			slotMidiLabels[i].setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
			slotMidiLearnButtons[i]->setMouseCursor(juce::MouseCursor::PointingHandCursor);
			slotMidiLearnButtons[i]->setIconPadding(ModernLookAndFeel::iconPadding);
			addAndMakeVisible(slotMidiLabels[i]);
		}
		updateMidiLearnButtons();

		styleCombo.addItem("Rock", 1);
		styleCombo.addItem("Metal", 2);
		styleCombo.addItem("Jazz", 3);
		styleCombo.addItem("Funk", 4);
		styleCombo.addItem("Electronic", 5);
		styleCombo.addItem("HipHop", 6);
		styleCombo.addItem("Latin", 7);
		styleCombo.addItem("Punk", 8);
		updateStyleComboForCurrentSlot();
		styleCombo.onChange = [this]()
			{
				int selectedStyle = styleCombo.getSelectedId() - 1;
				int currentSlot = processor.getPatternEngine().getActiveSlot();
				float normalized = selectedStyle / (float)(processor.slotStyleParams[currentSlot]->choices.size() - 1);
				processor.slotStyleParams[currentSlot]->setValueNotifyingHost(normalized);
				processor.getPatternEngine().setSlotStyle(currentSlot, static_cast<StyleType>(selectedStyle));

				processor.getPatternEngine().generateNewPatternForSlot(
					currentSlot,
					static_cast<StyleType>(selectedStyle),
					processor.intensityParam->get());
				processor.getPatternEngine().invalidateCache();
			};
		addAndMakeVisible(styleCombo);

		styleLabel.setText("Style", juce::dontSendNotification);
		styleLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(styleLabel);

		intensitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
		intensitySlider.setRange(0.0, 1.0, 0.01);
		intensitySlider.setValue(processor.intensityParam->get(), juce::dontSendNotification);
		intensitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		intensitySlider.onValueChange = [this]()
			{
				processor.intensityParam->setValueNotifyingHost(intensitySlider.getValue());
				updatePatternDisplay();
			};
		addAndMakeVisible(intensitySlider);

		intensityLabel.setText("Intensity", juce::dontSendNotification);
		intensityLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(intensityLabel);

		intensityMidiLearnButton.onClick = [this]()
			{ onIntensityMidiLearnClicked(); };
		intensityMidiLearnButton.setTooltip("Assign a MIDI CC to the intensity");
		addAndMakeVisible(intensityMidiLearnButton);

		intensityMidiLabel.setText("Not mapped", juce::dontSendNotification);
		intensityMidiLabel.setFont(juce::Font(10.0f));
		intensityMidiLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
		addAndMakeVisible(intensityMidiLabel);

		liveJamIntensitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
		liveJamIntensitySlider.setRange(0.0, 1.0, 0.01);
		liveJamIntensitySlider.setValue(processor.liveJamIntensityParam->get(), juce::dontSendNotification);
		liveJamIntensitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		liveJamIntensitySlider.onValueChange = [this]()
			{
				processor.liveJamIntensityParam->setValueNotifyingHost(liveJamIntensitySlider.getValue());
			};
		addAndMakeVisible(liveJamIntensitySlider);

		liveJamIntensityLabel.setText("Chaos", juce::dontSendNotification);
		liveJamIntensityLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		addAndMakeVisible(liveJamIntensityLabel);

		liveJamIntensityMidiLearnButton.onClick = [this]()
			{ onLiveJamIntensityMidiLearnClicked(); };
		liveJamIntensityMidiLearnButton.setTooltip("Assign a MIDI CC to Chaos");
		addAndMakeVisible(liveJamIntensityMidiLearnButton);

		liveJamIntensityMidiLabel.setText("Not mapped", juce::dontSendNotification);
		liveJamIntensityMidiLabel.setFont(juce::Font(10.0f));
		liveJamIntensityMidiLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
		addAndMakeVisible(liveJamIntensityMidiLabel);

		intensitySlider.setMouseCursor(juce::MouseCursor::PointingHandCursor);
		liveJamIntensitySlider.setMouseCursor(juce::MouseCursor::PointingHandCursor);
		styleCombo.setMouseCursor(juce::MouseCursor::PointingHandCursor);
	}

	void BeatCrafterEditor::paint(juce::Graphics& g)
	{
		g.fillAll(modernLookAndFeel.backgroundDark);

		auto typeface = juce::Typeface::createSystemTypefaceFor(
			BinaryData::AudiowideRegular_ttf,
			BinaryData::AudiowideRegular_ttfSize);
		juce::Font titleFont(typeface);
		titleFont = titleFont.withHeight(31.0f);

		g.setColour(modernLookAndFeel.accent);
		g.setFont(titleFont);
		g.drawText("BEATCRAFTER", 20, 8, 250, 35, juce::Justification::left);

		g.setColour(modernLookAndFeel.textDimmed);
		g.setFont(juce::Font(10.0f));
		g.drawText("v0.1.1", 240, 18, 50, 20, juce::Justification::left);
	}

	void BeatCrafterEditor::resized()
	{
		auto bounds = getLocalBounds();

		auto topArea = bounds.removeFromTop(130);
		topArea.removeFromTop(40);

		auto controlsArea = topArea.reduced(20, 0);

		auto line1 = controlsArea.removeFromTop(40);
		styleLabel.setBounds(line1.removeFromLeft(40));
		styleCombo.setBounds(line1.removeFromLeft(180).reduced(0, 8));
		line1.removeFromLeft(20);
		intensityLabel.setBounds(line1.removeFromLeft(80));
		intensityMidiLearnButton.setBounds(line1.removeFromRight(30).reduced(2, 5));
		intensitySlider.setBounds(line1.reduced(0, 10));

		auto line2 = controlsArea.removeFromTop(40);
		line2.removeFromLeft(240);
		liveJamIntensityLabel.setBounds(line2.removeFromLeft(80));
		liveJamIntensityMidiLearnButton.setBounds(line2.removeFromRight(30).reduced(2, 5));
		liveJamIntensitySlider.setBounds(line2.reduced(0, 10));

		auto slotAreaWithMidi = bounds.removeFromTop(100);
		auto slotArea = slotAreaWithMidi.removeFromTop(60);
		slotManager->setBounds(slotArea.reduced(20, 10));

		auto slotMidiArea = slotAreaWithMidi.removeFromTop(40).reduced(20, 0);
		int slotWidth = slotMidiArea.getWidth() / 8;
		for (int i = 0; i < 8; ++i)
		{
			auto slotBounds = slotMidiArea.removeFromLeft(slotWidth).reduced(1, 0);
			auto buttonArea = slotBounds.removeFromTop(20);
			slotMidiLearnButtons[i]->setBounds(buttonArea);
			slotMidiLabels[i].setBounds(slotBounds);
		}

		patternGrid->setBounds(bounds.reduced(20, 10));
	}

	void BeatCrafterEditor::timerCallback()
	{
		patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
		patternGrid->repaint();
		slotManager->updateSlotStates();
		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::updatePatternDisplay()
	{
		auto& engine = processor.getPatternEngine();
		engine.setIntensity(processor.intensityParam->get());
		engine.invalidateCache();
		patternGrid->setPattern(engine.getDisplayPattern());
		patternGrid->repaint();
	}

	void BeatCrafterEditor::updateStyleComboForCurrentSlot(int slotIndex)
	{
		int currentSlot = slotIndex >= 0
			? slotIndex
			: processor.getPatternEngine().getActiveSlot();

		int styleFromParam = processor.slotStyleParams[currentSlot]->getIndex();
		StyleType styleFromEngine = processor.getPatternEngine().getSlotStyle(currentSlot);

		if (static_cast<int>(styleFromEngine) != styleFromParam)
			processor.getPatternEngine().setSlotStyle(
				currentSlot, static_cast<StyleType>(styleFromParam));

		styleCombo.setSelectedId(styleFromParam + 1, juce::dontSendNotification);
	}

	void BeatCrafterEditor::updateMidiLearnButtons()
	{
		if (processor.isMidiLearning() && processor.getMidiLearnTarget() == 0)
		{
			intensityMidiLearnButton.setIconFromSVG(
				BinaryData::broadcast_svg, BinaryData::broadcast_svgSize,
				juce::Colours::white, juce::Colours::red.darker(0.3f));
			intensityMidiLearnButton.setTooltip("Click to cancel listening");
		}
		else if (processor.hasMidiMapping(0, -1))
		{
			intensityMidiLearnButton.setIconFromSVG(
				BinaryData::linksimplebreak_svg, BinaryData::linksimplebreak_svgSize,
				juce::Colours::white, juce::Colours::green.darker(0.4f));
			intensityMidiLearnButton.setTooltip(processor.getMidiMappingDescription(0, -1));
		}
		else
		{
			intensityMidiLearnButton.setIconFromSVG(
				BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
				modernLookAndFeel.textDimmed, modernLookAndFeel.backgroundMid);
			intensityMidiLearnButton.setTooltip("Assign a MIDI CC to the intensity");
		}
		intensityMidiLabel.setText(
			processor.getMidiMappingDescription(0, -1), juce::dontSendNotification);

		for (int i = 0; i < 8; ++i)
		{
			auto& btn = *slotMidiLearnButtons[i];
			if (processor.isMidiLearning() && processor.getMidiLearnTarget() >= 1 && processor.getMidiLearnSlot() == i)
			{
				btn.setIconFromSVG(
					BinaryData::broadcast_svg, BinaryData::broadcast_svgSize,
					juce::Colours::white, juce::Colours::red.darker(0.3f));
				btn.setTooltip("Click to cancel listening");
			}
			else if (processor.hasMidiMapping(1, i))
			{
				btn.setIconFromSVG(
					BinaryData::linksimplebreak_svg, BinaryData::linksimplebreak_svgSize,
					juce::Colours::white, juce::Colours::green.darker(0.4f));
				btn.setTooltip(processor.getMidiMappingDescription(1, i));
			}
			else
			{
				btn.setIconFromSVG(
					BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
					modernLookAndFeel.textDimmed, modernLookAndFeel.backgroundMid);
				btn.setTooltip("Assign a MIDI CC to slot " + juce::String(i + 1));
			}
			slotMidiLabels[i].setText(
				processor.hasMidiMapping(1, i)
				? processor.getMidiMappingDescription(1, i)
				: "--",
				juce::dontSendNotification);
		}

		if (processor.isMidiLearning() && processor.getMidiLearnTarget() == 2)
		{
			liveJamIntensityMidiLearnButton.setIconFromSVG(
				BinaryData::broadcast_svg, BinaryData::broadcast_svgSize,
				juce::Colours::white, juce::Colours::red.darker(0.3f));
			liveJamIntensityMidiLearnButton.setTooltip("Click to cancel listening");
		}
		else if (processor.hasMidiMapping(2, -1))
		{
			liveJamIntensityMidiLearnButton.setIconFromSVG(
				BinaryData::linksimplebreak_svg, BinaryData::linksimplebreak_svgSize,
				juce::Colours::white, juce::Colours::green.darker(0.4f));
			liveJamIntensityMidiLearnButton.setTooltip(
				processor.getMidiMappingDescription(2, -1));
		}
		else
		{
			liveJamIntensityMidiLearnButton.setIconFromSVG(
				BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
				modernLookAndFeel.textDimmed, modernLookAndFeel.backgroundMid);
			liveJamIntensityMidiLearnButton.setTooltip(
				"Assign a MIDI CC to Chaos");
		}
		liveJamIntensityMidiLabel.setText(
			processor.getMidiMappingDescription(2, -1), juce::dontSendNotification);
		intensityMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);
		liveJamIntensityMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);
	}

	void BeatCrafterEditor::onIntensityMidiLearnClicked()
	{
		if (processor.isMidiLearning())
			processor.stopMidiLearn();
		else if (processor.hasMidiMapping(0, -1))
			processor.clearMidiMapping(0, -1);
		else
			processor.startMidiLearn(0, -1);
		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::onSlotMidiLearnClicked(int slot)
	{
		if (processor.isMidiLearning())
			processor.stopMidiLearn();
		else if (processor.hasMidiMapping(1, slot))
			processor.clearMidiMapping(1, slot);
		else
			processor.startMidiLearn(1, slot);
		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::onLiveJamIntensityMidiLearnClicked()
	{
		if (processor.isMidiLearning())
			processor.stopMidiLearn();
		else if (processor.hasMidiMapping(2, -1))
			processor.clearMidiMapping(2, -1);
		else
			processor.startMidiLearn(2, -1);
		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::updateFromProcessorState()
	{
		intensitySlider.setValue(processor.intensityParam->get(), juce::dontSendNotification);
		int currentSlot = processor.getPatternEngine().getActiveSlot();
		int currentStyleIndex = processor.slotStyleParams[currentSlot]->getIndex();
		styleCombo.setSelectedId(currentStyleIndex + 1, juce::dontSendNotification);
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
		patternGrid->repaint();
		slotManager->updateSlotStates();
		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::updateIntensitySlider(float newIntensity)
	{
		intensitySlider.setValue(newIntensity, juce::dontSendNotification);
		updatePatternDisplay();
	}

	void BeatCrafterEditor::updateLiveJamIntensitySlider(float newIntensity)
	{
		liveJamIntensitySlider.setValue(newIntensity, juce::dontSendNotification);
	}

	void BeatCrafterEditor::updateSlotButtons(int activeSlot)
	{
		slotManager->updateSlotStates();
		int styleFromParam = processor.slotStyleParams[activeSlot]->getIndex();
		styleCombo.setSelectedId(styleFromParam + 1, juce::dontSendNotification);
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
		patternGrid->repaint();
	}
}