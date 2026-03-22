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
			modernLookAndFeel),
		surpriseMeButton("SurpriseMe",
			BinaryData::wavesine_svg, BinaryData::wavesine_svgSize,
			modernLookAndFeel, true),
		surpriseMeMidiLearnButton("SurpriseMeMidi",
			BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
			modernLookAndFeel)
	{
		tooltipWindow = std::make_unique<juce::TooltipWindow>(this);
		setLookAndFeel(&modernLookAndFeel);
		setupComponents();
		setSize(960, 600);
		setResizeLimits(960, 600, 1920, 1200);
		juce::Desktop::getInstance().setGlobalScaleFactor(1.0f);
		startTimerHz(30);
		juce::Timer::callAfterDelay(100, [this]()
			{
				intensitySlider.setValue(processor.intensityParam->get(),
					juce::dontSendNotification);
				liveJamIntensitySlider.setValue(processor.liveJamIntensityParam->get(),
					juce::dontSendNotification);
				processor.getPatternEngine().perfParams.surpriseMeEnabled =
					processor.surpriseMeParam->get();
				updateMidiLearnButtons();
			});
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
		patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
		patternGrid->setPatternEngine(&processor.getPatternEngine());
		addAndMakeVisible(patternGrid.get());

		slotManager = std::make_unique<SlotManager>(processor.getPatternEngine());
		slotManager->onSlotChanged = [this](int /*slot*/)
			{
				processor.getPatternEngine().setIntensity(processor.intensityParam->get());
				processor.getPatternEngine().invalidateCache();
				patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
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
			slotMidiLearnButtons[i]->setMouseCursor(juce::MouseCursor::PointingHandCursor);
			slotMidiLearnButtons[i]->setIconPadding(ModernLookAndFeel::iconPadding);
			addAndMakeVisible(slotMidiLearnButtons[i].get());

			slotMidiLabels[i].setText("--", juce::dontSendNotification);
			slotMidiLabels[i].getProperties().set("customFontHeight",
				ModernLookAndFeel::fontSizeMidiLabel);
			slotMidiLabels[i].setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
			addAndMakeVisible(slotMidiLabels[i]);
		}

		surpriseMeButton.onClick = [this]()
			{
				if (processor.isMidiLearning() && processor.getMidiLearnTarget() == 3)
				{
					processor.stopMidiLearn();
				}
				else
				{
					bool active = !processor.surpriseMeParam->get();
					processor.surpriseMeParam->setValueNotifyingHost(active ? 1.0f : 0.0f);
					processor.getPatternEngine().perfParams.surpriseMeEnabled = active;
				}
				updateMidiLearnButtons();
			};
		addAndMakeVisible(surpriseMeButton);

		surpriseMeMidiLearnButton.onClick = [this]()
			{ onSurpriseMeMidiLearnClicked(); };
		surpriseMeMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);
		addAndMakeVisible(surpriseMeMidiLearnButton);

		surpriseMeMidiLabel.setText("--", juce::dontSendNotification);
		surpriseMeMidiLabel.getProperties().set("customFontHeight", ModernLookAndFeel::fontSizeMidiLabel);
		surpriseMeMidiLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
		surpriseMeMidiLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(surpriseMeMidiLabel);

		updateMidiLearnButtons();

		intensitySlider.setSliderStyle(juce::Slider::LinearVertical);
		intensitySlider.setRange(0.0, 1.0, 0.01);
		intensitySlider.setValue(processor.intensityParam->get(), juce::dontSendNotification);
		intensitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		intensitySlider.onValueChange = [this]()
			{
				processor.intensityParam->setValueNotifyingHost(intensitySlider.getValue());
				updatePatternDisplay();
			};
		intensitySlider.setMouseCursor(juce::MouseCursor::PointingHandCursor);
		addAndMakeVisible(intensitySlider);

		processor.getPatternEngine().onIntensityChanged = [this](float newIntensity)
			{
				juce::MessageManager::callAsync([this, newIntensity]()
					{
						if (std::abs(intensitySlider.getValue() - newIntensity) > 0.005f)
						{
							*processor.intensityParam = newIntensity;
							intensitySlider.setValue(newIntensity, juce::dontSendNotification);
						}
					});
			};

		intensityLabel.setText("Intensity", juce::dontSendNotification);
		intensityLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		intensityLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(intensityLabel);

		intensityMidiLearnButton.onClick = [this]()
			{ onIntensityMidiLearnClicked(); };
		intensityMidiLearnButton.setTooltip("Assign a MIDI CC to the intensity");
		addAndMakeVisible(intensityMidiLearnButton);

		intensityMidiLabel.setText("--", juce::dontSendNotification);
		intensityMidiLabel.getProperties().set("customFontHeight",
			ModernLookAndFeel::fontSizeMidiLabel);
		intensityMidiLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
		intensityMidiLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(intensityMidiLabel);

		liveJamIntensitySlider.setSliderStyle(juce::Slider::LinearVertical);
		liveJamIntensitySlider.setRange(0.0, 1.0, 0.01);
		liveJamIntensitySlider.setValue(processor.liveJamIntensityParam->get(), juce::dontSendNotification);
		liveJamIntensitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		liveJamIntensitySlider.onValueChange = [this]()
			{
				processor.liveJamIntensityParam->setValueNotifyingHost(liveJamIntensitySlider.getValue());
			};
		liveJamIntensitySlider.setMouseCursor(juce::MouseCursor::PointingHandCursor);
		addAndMakeVisible(liveJamIntensitySlider);

		liveJamIntensityLabel.setText("Chaos", juce::dontSendNotification);
		liveJamIntensityLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textColour);
		liveJamIntensityLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(liveJamIntensityLabel);

		liveJamIntensityMidiLearnButton.onClick = [this]()
			{ onLiveJamIntensityMidiLearnClicked(); };
		liveJamIntensityMidiLearnButton.setTooltip("Assign a MIDI CC to Chaos");
		addAndMakeVisible(liveJamIntensityMidiLearnButton);

		liveJamIntensityMidiLabel.setText("--", juce::dontSendNotification);
		liveJamIntensityMidiLabel.getProperties().set("customFontHeight",
			ModernLookAndFeel::fontSizeMidiLabel);
		liveJamIntensityMidiLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
		liveJamIntensityMidiLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(liveJamIntensityMidiLabel);
	}

	void BeatCrafterEditor::onSurpriseMeMidiLearnClicked()
	{
		if (processor.isMidiLearning())
			processor.stopMidiLearn();
		else if (processor.hasMidiMapping(3, -1))
			processor.clearMidiMapping(3, -1);
		else
			processor.startMidiLearn(3, -1);
		updateMidiLearnButtons();
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
		bounds.removeFromTop(50);

		auto slotAreaWithMidi = bounds.removeFromTop(100);
		auto slotArea = slotAreaWithMidi.removeFromTop(60);
		slotManager->setBounds(slotArea.reduced(20, 10));
		auto slotMidiArea = slotAreaWithMidi.removeFromTop(40).reduced(20, 0);
		int slotWidth = slotMidiArea.getWidth() / 8;
		for (int i = 0; i < 8; ++i)
		{
			auto slotBounds = slotMidiArea.removeFromLeft(slotWidth).reduced(1, 0);
			slotMidiLearnButtons[i]->setBounds(slotBounds.removeFromTop(20));
			slotMidiLabels[i].setBounds(slotBounds);
		}

		auto contentArea = bounds.reduced(20, 10);
		auto rightColumn = contentArea.removeFromRight(180);
		contentArea.removeFromRight(10);

		int gridBottom = contentArea.getBottom();
		int midiAreaHeight = 40;
		int sliderWidth = 20;
		int colWidth = 60;
		int buttonSize = 32;

		auto intensityCol = rightColumn.removeFromLeft(colWidth);
		intensityLabel.setBounds(intensityCol.removeFromTop(20).reduced(2, 0));
		auto intensityMidiArea = intensityCol.withTop(gridBottom - midiAreaHeight);
		intensityMidiLearnButton.setBounds(intensityMidiArea.removeFromTop(24).reduced(4, 2));
		intensityMidiLabel.setBounds(intensityMidiArea.removeFromTop(16).reduced(2, 0));
		auto intensitySliderArea = intensityCol.withBottom(gridBottom - midiAreaHeight);
		intensitySlider.setBounds(
			intensitySliderArea.getCentreX() - sliderWidth / 2,
			intensitySliderArea.getY() + 10,
			sliderWidth,
			intensitySliderArea.getHeight() - 10);

		auto chaosCol = rightColumn.removeFromLeft(colWidth);
		liveJamIntensityLabel.setBounds(chaosCol.removeFromTop(20).reduced(2, 0));
		auto chaosMidiArea = chaosCol.withTop(gridBottom - midiAreaHeight);
		liveJamIntensityMidiLearnButton.setBounds(chaosMidiArea.removeFromTop(24).reduced(4, 2));
		liveJamIntensityMidiLabel.setBounds(chaosMidiArea.removeFromTop(16).reduced(2, 0));
		auto chaosSliderArea = chaosCol.withBottom(gridBottom - midiAreaHeight);
		liveJamIntensitySlider.setBounds(
			chaosSliderArea.getCentreX() - sliderWidth / 2,
			chaosSliderArea.getY() + 10,
			sliderWidth,
			chaosSliderArea.getHeight() - 10);

		auto surpriseCol = rightColumn;

		int blockHeight = 120;
		auto surpriseBlock = surpriseCol.removeFromBottom(blockHeight);

		surpriseMeMidiLabel.setBounds(surpriseBlock.removeFromBottom(16).reduced(2, 0));
		surpriseMeMidiLearnButton.setBounds(surpriseBlock.removeFromBottom(28).reduced(4, 2));
		surpriseMeButton.setBounds(surpriseBlock.removeFromBottom(buttonSize + 8).withSizeKeepingCentre(buttonSize, buttonSize));

		patternGrid->setBounds(contentArea);
	}

	void BeatCrafterEditor::timerCallback()
	{
		float engineIntensity = processor.getPatternEngine().getIntensity();

		if (std::abs(intensitySlider.getValue() - engineIntensity) > 0.005f)
			intensitySlider.setValue(engineIntensity, juce::dontSendNotification);

		processor.getPatternEngine().setLiveJamIntensity(
			processor.liveJamIntensityParam->get());

		if (std::abs(engineIntensity - lastRepaintIntensity) > 0.02f)
		{
			processor.getPatternEngine().setIntensity(engineIntensity);
			patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
			patternGrid->repaint();
			lastRepaintIntensity = engineIntensity;
		}

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

		if (processor.surpriseMeParam->get())
			surpriseMeButton.setIconFromSVG(
				BinaryData::wavesine_svg, BinaryData::wavesine_svgSize,
				modernLookAndFeel.accent,
				modernLookAndFeel.backgroundMid);
		else
			surpriseMeButton.setIconFromSVG(
				BinaryData::wavesine_svg, BinaryData::wavesine_svgSize,
				modernLookAndFeel.textDimmed,
				modernLookAndFeel.backgroundMid);
		surpriseMeButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);

		if (processor.isMidiLearning() && processor.getMidiLearnTarget() == 3)
		{
			surpriseMeMidiLearnButton.setIconFromSVG(
				BinaryData::broadcast_svg, BinaryData::broadcast_svgSize,
				juce::Colours::white, juce::Colours::red.darker(0.3f));
			surpriseMeMidiLearnButton.setTooltip("Click to cancel listening");
		}
		else if (processor.hasMidiMapping(3, -1))
		{
			surpriseMeMidiLearnButton.setIconFromSVG(
				BinaryData::linksimplebreak_svg, BinaryData::linksimplebreak_svgSize,
				juce::Colours::white, juce::Colours::green.darker(0.4f));
			surpriseMeMidiLearnButton.setTooltip(processor.getMidiMappingDescription(3, -1));
		}
		else
		{
			surpriseMeMidiLearnButton.setIconFromSVG(
				BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
				modernLookAndFeel.textDimmed, modernLookAndFeel.backgroundMid);
			surpriseMeMidiLearnButton.setTooltip("Assign a MIDI CC to Surprise Me");
		}
		surpriseMeMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);
		surpriseMeMidiLabel.setText(
			processor.getMidiMappingDescription(3, -1), juce::dontSendNotification);
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

	void BeatCrafterEditor::updateSlotButtons(int /*activeSlot*/)
	{
		slotManager->updateSlotStates();
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
		patternGrid->repaint();
	}
}