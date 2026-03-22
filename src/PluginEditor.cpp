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
			modernLookAndFeel),
		tripletModeButton("TripletMode",
			BinaryData::numberthree_svg, BinaryData::numberthree_svgSize,
			modernLookAndFeel, true),
		tripletModeMidiLearnButton("TripletModeMidi",
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
				processor.getPatternEngine().perfParams.tripletMode =
					processor.tripletModeParam->get();
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
		patternGrid->markDirty();
		patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
		patternGrid->setPatternEngine(&processor.getPatternEngine());
		addAndMakeVisible(patternGrid.get());

		slotManager = std::make_unique<SlotManager>(processor.getPatternEngine());
		slotManager->onSlotChanged = [this](int /*slot*/)
			{
				processor.getPatternEngine().setIntensity(processor.intensityParam->get());
				processor.getPatternEngine().invalidateCache();
				patternGrid->markDirty();
				patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
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

		tripletModeButton.onClick = [this]()
			{
				if (processor.isMidiLearning() && processor.getMidiLearnTarget() == 4)
					processor.stopMidiLearn();
				else {
					bool active = !processor.tripletModeParam->get();
					processor.tripletModeParam->setValueNotifyingHost(active ? 1.0f : 0.0f);
					processor.getPatternEngine().perfParams.tripletMode = active;
				}
				updateMidiLearnButtons();
			};
		addAndMakeVisible(tripletModeButton);

		tripletModeMidiLearnButton.onClick = [this]() { onTripletModeMidiLearnClicked(); };
		tripletModeMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);
		addAndMakeVisible(tripletModeMidiLearnButton);

		tripletModeMidiLabel.setText("--", juce::dontSendNotification);
		tripletModeMidiLabel.getProperties().set("customFontHeight", ModernLookAndFeel::fontSizeMidiLabel);
		tripletModeMidiLabel.setColour(juce::Label::textColourId, modernLookAndFeel.textDimmed);
		tripletModeMidiLabel.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(tripletModeMidiLabel);

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

	void BeatCrafterEditor::onTripletModeMidiLearnClicked()
	{
		if (processor.isMidiLearning())
			processor.stopMidiLearn();
		else if (processor.hasMidiMapping(4, -1))
			processor.clearMidiMapping(4, -1);
		else
			processor.startMidiLearn(4, -1);
		updateMidiLearnButtons();
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
		auto slotRow = slotAreaWithMidi.removeFromTop(60).reduced(20, 10);
		slotManager->setBounds(slotRow.reduced(2));

		auto slotMidiArea = slotAreaWithMidi.removeFromTop(40);
		int slotWidth = slotRow.getWidth() / 8;
		for (int i = 0; i < 8; ++i)
		{
			auto slotBounds = juce::Rectangle<int>(
				slotRow.getX() + i * slotWidth,
				slotMidiArea.getY(),
				slotWidth,
				slotMidiArea.getHeight()).reduced(2, 0);
			slotMidiLearnButtons[i]->setBounds(slotBounds.removeFromTop(20));
			slotMidiLabels[i].setBounds(slotBounds);
		}

		auto contentArea = bounds.reduced(20, 10);
		auto rightColumn = contentArea.removeFromRight(240);
		contentArea.removeFromRight(10);

		int gridBottom = contentArea.getBottom();
		int midiAreaHeight = 40;
		int sliderWidth = 20;
		int colWidth = 60;
		int buttonSize = 32;
		int blockHeight = 80;

		layoutSliderBlock(rightColumn.removeFromLeft(colWidth),
			intensityLabel, intensitySlider,
			intensityMidiLearnButton, intensityMidiLabel,
			sliderWidth, gridBottom, midiAreaHeight);

		layoutSliderBlock(rightColumn.removeFromLeft(colWidth),
			liveJamIntensityLabel, liveJamIntensitySlider,
			liveJamIntensityMidiLearnButton, liveJamIntensityMidiLabel,
			sliderWidth, gridBottom, midiAreaHeight);

		auto toggleArea = rightColumn;
		int toggleColWidth = toggleArea.getWidth() / 2;

		layoutToggleBlock(toggleArea.removeFromLeft(toggleColWidth).removeFromBottom(blockHeight),
			surpriseMeButton, surpriseMeMidiLearnButton, surpriseMeMidiLabel, buttonSize);

		layoutToggleBlock(toggleArea.removeFromBottom(blockHeight),
			tripletModeButton, tripletModeMidiLearnButton, tripletModeMidiLabel, buttonSize);

		patternGrid->setBounds(contentArea);
	}

	void BeatCrafterEditor::layoutToggleBlock(juce::Rectangle<int> zone,
		juce::Component& toggleButton,
		juce::Component& midiLearnButton,
		juce::Label& midiLabel,
		int buttonSize)
	{
		midiLabel.setBounds(zone.removeFromBottom(16).reduced(2, 0));
		midiLearnButton.setBounds(zone.removeFromBottom(24).reduced(4, 2));
		toggleButton.setBounds(zone.withSizeKeepingCentre(buttonSize, buttonSize));
	}

	void BeatCrafterEditor::layoutSliderBlock(juce::Rectangle<int> zone,
		juce::Label& label,
		juce::Slider& slider,
		juce::Component& midiLearnButton,
		juce::Label& midiLabel,
		int sliderWidth,
		int gridBottom,
		int midiAreaHeight)
	{
		label.setBounds(zone.removeFromTop(20).reduced(2, 0));
		auto midiArea = zone.withTop(gridBottom - midiAreaHeight);
		midiLearnButton.setBounds(midiArea.removeFromTop(24).reduced(4, 2));
		midiLabel.setBounds(midiArea.removeFromTop(16).reduced(2, 0));
		auto sliderArea = zone.withBottom(gridBottom - midiAreaHeight);
		slider.setBounds(
			sliderArea.getCentreX() - sliderWidth / 2,
			sliderArea.getY() + 10,
			sliderWidth,
			sliderArea.getHeight() - 10);
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
			patternGrid->markDirty();
			patternGrid->setPattern(processor.getPatternEngine().getDisplayPattern());
			lastRepaintIntensity = engineIntensity;
		}

		updateMidiLearnButtons();
	}

	void BeatCrafterEditor::updatePatternDisplay()
	{
		auto& engine = processor.getPatternEngine();
		engine.setIntensity(processor.intensityParam->get());
		engine.invalidateCache();
		patternGrid->markDirty();
		patternGrid->setPattern(engine.getDisplayPattern());
	}

	void BeatCrafterEditor::applyToggleButtonState(IconButton& btn,
		const char* svgData, int svgSize,
		bool isActive)
	{
		btn.setIconFromSVG(svgData, svgSize,
			isActive ? modernLookAndFeel.accent : modernLookAndFeel.textDimmed,
			modernLookAndFeel.backgroundMid);
		btn.setToggleState(isActive, juce::dontSendNotification);
	}

	void BeatCrafterEditor::applyMidiLearnButtonState(IconButton& btn,
		bool isLearning,
		bool hasMapping,
		const juce::String& mappingDescription,
		const juce::String& defaultTooltip)
	{
		if (isLearning)
		{
			btn.setIconFromSVG(BinaryData::broadcast_svg, BinaryData::broadcast_svgSize,
				juce::Colours::white, juce::Colours::red.darker(0.3f));
			btn.setTooltip("Click to cancel listening");
		}
		else if (hasMapping)
		{
			btn.setIconFromSVG(BinaryData::linksimplebreak_svg, BinaryData::linksimplebreak_svgSize,
				juce::Colours::white, juce::Colours::green.darker(0.4f));
			btn.setTooltip(mappingDescription);
		}
		else
		{
			btn.setIconFromSVG(BinaryData::linksimple_svg, BinaryData::linksimple_svgSize,
				modernLookAndFeel.textDimmed, modernLookAndFeel.backgroundMid);
			btn.setTooltip(defaultTooltip);
		}
	}

	void BeatCrafterEditor::updateMidiLearnPair(IconButton& btn,
		juce::Label& label,
		bool isLearning,
		bool hasMapping,
		const juce::String& mappingDescription,
		const juce::String& defaultTooltip)
	{
		applyMidiLearnButtonState(btn, isLearning, hasMapping, mappingDescription, defaultTooltip);
		label.setText(hasMapping ? mappingDescription : "--", juce::dontSendNotification);
	}

	void BeatCrafterEditor::updateMidiLearnButtons()
	{
		updateMidiLearnPair(
			intensityMidiLearnButton, intensityMidiLabel,
			processor.isMidiLearning() && processor.getMidiLearnTarget() == 0,
			processor.hasMidiMapping(0, -1),
			processor.getMidiMappingDescription(0, -1),
			"Assign a MIDI CC to the intensity");
		intensityMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);

		for (int i = 0; i < 8; ++i)
		{
			updateMidiLearnPair(
				*slotMidiLearnButtons[i], slotMidiLabels[i],
				processor.isMidiLearning()
				&& processor.getMidiLearnTarget() >= 1
				&& processor.getMidiLearnSlot() == i,
				processor.hasMidiMapping(1, i),
				processor.getMidiMappingDescription(1, i),
				"Assign a MIDI CC to slot " + juce::String(i + 1));
		}

		updateMidiLearnPair(
			liveJamIntensityMidiLearnButton, liveJamIntensityMidiLabel,
			processor.isMidiLearning() && processor.getMidiLearnTarget() == 2,
			processor.hasMidiMapping(2, -1),
			processor.getMidiMappingDescription(2, -1),
			"Assign a MIDI CC to Chaos");
		liveJamIntensityMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);

		applyToggleButtonState(surpriseMeButton,
			BinaryData::wavesine_svg, BinaryData::wavesine_svgSize,
			processor.surpriseMeParam->get());
		surpriseMeButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);

		updateMidiLearnPair(
			surpriseMeMidiLearnButton, surpriseMeMidiLabel,
			processor.isMidiLearning() && processor.getMidiLearnTarget() == 3,
			processor.hasMidiMapping(3, -1),
			processor.getMidiMappingDescription(3, -1),
			"Assign a MIDI CC to Surprise Me");
		surpriseMeMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);

		applyToggleButtonState(tripletModeButton,
			BinaryData::numberthree_svg, BinaryData::numberthree_svgSize,
			processor.tripletModeParam->get());
		tripletModeButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);

		updateMidiLearnPair(
			tripletModeMidiLearnButton, tripletModeMidiLabel,
			processor.isMidiLearning() && processor.getMidiLearnTarget() == 4,
			processor.hasMidiMapping(4, -1),
			processor.getMidiMappingDescription(4, -1),
			"Assign a MIDI CC to Triplet Mode");
		tripletModeMidiLearnButton.setIconPadding(ModernLookAndFeel::iconPaddingHard);
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
		patternGrid->markDirty();
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
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
		patternGrid->markDirty();
		patternGrid->setPattern(&processor.getPatternEngine().getCurrentPattern());
	}
}