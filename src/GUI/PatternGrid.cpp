#include "PatternGrid.h"

namespace BeatCrafter {

	PatternGrid::PatternGrid() {
		lookAndFeel = dynamic_cast<ModernLookAndFeel*>(&getLookAndFeel());
		startTimerHz(30);
	}

	PatternGrid::~PatternGrid() {
		stopTimer();
	}

	void PatternGrid::setPattern(const Pattern* pattern) {
		currentPattern = pattern;
		repaint();
	}

	void PatternGrid::setPatternEngine(PatternEngine* engine) {
		patternEngine = engine;
	}

	void PatternGrid::updateWithIntensity(const Pattern& intensifiedPattern) {
		static Pattern tempPattern("Display");
		tempPattern = intensifiedPattern;
		currentPattern = &tempPattern;
		repaint();
	}

	void PatternGrid::paint(juce::Graphics& g) {
		if (!lookAndFeel) {
			lookAndFeel = dynamic_cast<ModernLookAndFeel*>(&getLookAndFeel());
		}

		drawBackground(g);
		drawGrid(g);

		if (currentPattern) {
			drawSteps(g);
			drawPlayhead(g);
		}

		drawTrackLabels(g);
		drawStepNumbers(g);
	}

	void PatternGrid::resized() {
		auto bounds = getLocalBounds();

		int numSteps = currentPattern ? currentPattern->getLength() : 16;
		int numTracks = currentPattern ? currentPattern->getNumTracks() : 8;

		cellWidth = (bounds.getWidth() - headerWidth) / static_cast<float>(numSteps);
		cellHeight = (bounds.getHeight() - headerHeight) / static_cast<float>(numTracks);
	}

	void PatternGrid::mouseDown(const juce::MouseEvent& event) {
		if (!currentPattern) return;
		if (isVelocityPopupOpen) return;

		if (event.mods.isCtrlDown()) {
			startMidiDragAndDrop(event);
			return;
		}

		auto [track, step] = getStepFromPosition(event.getPosition());
		if (track >= 0 && step >= 0) {
			if (patternEngine) {
				Pattern* editablePattern = patternEngine->getCurrentBasePattern();
				if (editablePattern) {
					auto& stepObj = editablePattern->getTrack(track).getStep(step);

					if (event.mods.isRightButtonDown()) {
						if (stepObj.isActive()) {
							showVelocityPopup(track, step);
						}
					}
					else {
						bool newState = !stepObj.isActive();
						stepObj.setActive(newState);
						if (newState) {
							stepObj.setVelocity(0.7f);
						}
						if (onStepChanged)
							onStepChanged(track, step, newState);
					}

					currentPattern = patternEngine->getDisplayPattern();
					repaint();
				}
			}
		}
	}

	void PatternGrid::showVelocityPopup(int track, int step) {
		Pattern* editablePattern = patternEngine->getCurrentBasePattern();
		if (!editablePattern) return;

		auto& stepObj = editablePattern->getTrack(track).getStep(step);
		if (!stepObj.isActive()) return;

		closeVelocityPopup();

		velocityPopup = std::make_unique<VelocityPopup>(
			[this, track, step](float newVelocity) {
				Pattern* editablePattern = patternEngine->getCurrentBasePattern();
				if (editablePattern) {
					auto& stepObj = editablePattern->getTrack(track).getStep(step);
					stepObj.setVelocity(newVelocity);
					currentPattern = patternEngine->getDisplayPattern();
					repaint();
				}
			},
			[this]() {
				closeVelocityPopup();
			}
		);

		velocityPopup->setVelocity(stepObj.getVelocity());
		addAndMakeVisible(velocityPopup.get());
		auto stepBounds = getStepBounds(track, step);
		int popupX = stepBounds.getX() + stepBounds.getWidth() + 5;
		int popupY = stepBounds.getY() - 20;
		popupX = juce::jmin(popupX, getWidth() - velocityPopup->getWidth());
		popupY = juce::jmax(popupY, 0);

		velocityPopup->setTopLeftPosition(popupX, popupY);
		isVelocityPopupOpen = true;
	}

	void PatternGrid::closeVelocityPopup() {
		if (velocityPopup) {
			removeChildComponent(velocityPopup.get());
			velocityPopup.reset();
			isVelocityPopupOpen = false;
		}
	}

	void PatternGrid::mouseDrag(const juce::MouseEvent& event) {
		if (!currentPattern) return;

		if (event.mods.isCtrlDown()) {
			if (event.getDistanceFromDragStart() > 5) {
				setMouseCursor(juce::MouseCursor::DraggingHandCursor);
				startMidiDragAndDrop(event);
			}
		}
	}

	void PatternGrid::mouseUp(const juce::MouseEvent& event) {
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}

	void PatternGrid::startMidiDragAndDrop(const juce::MouseEvent& event) {
		auto midiSequence = createMidiFromPattern();

		if (midiSequence.getNumEvents() == 0) return;

		juce::File tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
		juce::File midiFile = tempDir.getChildFile("pattern_" + juce::String(juce::Random::getSystemRandom().nextInt()) + ".mid");

		juce::MidiFile midi;
		midi.addTrack(midiSequence);
		midi.setTicksPerQuarterNote(480);

		juce::FileOutputStream stream(midiFile);
		if (stream.openedOk()) {
			midi.writeTo(stream);
			stream.flush();
			juce::StringArray files;
			files.add(midiFile.getFullPathName());
			juce::DragAndDropContainer::performExternalDragDropOfFiles(files, true);
		}
	}

	juce::MidiMessageSequence PatternGrid::createMidiFromPattern() const {
		juce::MidiMessageSequence sequence;

		if (!currentPattern) return sequence;

		double ticksPerStep = 480.0 / 4.0;

		for (int track = 0; track < currentPattern->getNumTracks(); ++track) {
			int midiNote = 36 + track;

			for (int step = 0; step < currentPattern->getLength(); ++step) {
				const auto& stepObj = currentPattern->getTrack(track).getStep(step);

				if (stepObj.isActive()) {
					double timeStamp = step * ticksPerStep;
					int velocity = static_cast<int>(stepObj.getVelocity() * 127);
					auto noteOn = juce::MidiMessage::noteOn(1, midiNote, static_cast<juce::uint8>(velocity));
					noteOn.setTimeStamp(timeStamp);
					sequence.addEvent(noteOn);
					auto noteOff = juce::MidiMessage::noteOff(1, midiNote);
					noteOff.setTimeStamp(timeStamp + ticksPerStep * 0.1);
					sequence.addEvent(noteOff);
				}
			}
		}

		return sequence;
	}

	void PatternGrid::mouseMove(const juce::MouseEvent& event) {
		auto [track, step] = getStepFromPosition(event.getPosition());

		if (track != hoveredTrack || step != hoveredStep) {
			hoveredTrack = track;
			hoveredStep = step;
			repaint();
		}
	}

	void PatternGrid::mouseExit(const juce::MouseEvent& event) {
		hoveredTrack = -1;
		hoveredStep = -1;
		repaint();
	}

	void PatternGrid::timerCallback() {
		if (currentPattern) {
			int newPlayhead = currentPattern->getCurrentStep();
			if (newPlayhead != playheadPosition) {
				playheadPosition = newPlayhead;
				repaint();
			}
		}
	}

	juce::Rectangle<float> PatternGrid::getStepBounds(int track, int step) const {
		float x = headerWidth + step * cellWidth;
		float y = headerHeight + track * cellHeight;
		return juce::Rectangle<float>(x, y, cellWidth, cellHeight);
	}

	std::pair<int, int> PatternGrid::getStepFromPosition(juce::Point<int> pos) const {
		if (pos.x < headerWidth || pos.y < headerHeight)
			return { -1, -1 };

		int step = static_cast<int>((pos.x - headerWidth) / cellWidth);
		int track = static_cast<int>((pos.y - headerHeight) / cellHeight);

		int numSteps = currentPattern ? currentPattern->getLength() : 16;
		int numTracks = currentPattern ? currentPattern->getNumTracks() : 8;

		if (step >= 0 && step < numSteps && track >= 0 && track < numTracks)
			return { track, step };

		return { -1, -1 };
	}

	void PatternGrid::drawBackground(juce::Graphics& g) {
		g.fillAll(lookAndFeel->backgroundDark);

		g.setColour(lookAndFeel->backgroundMid);
		g.fillRect(juce::Rectangle<float>(0.0f, 0.0f, (float)getWidth(), (float)headerHeight));
		g.fillRect(juce::Rectangle<float>(0.0f, 0.0f, (float)headerWidth, (float)getHeight()));
	}

	void PatternGrid::drawGrid(juce::Graphics& g) {
		if (!currentPattern) return;

		int numSteps = currentPattern->getLength();
		int numTracks = currentPattern->getNumTracks();

		g.setColour(lookAndFeel->backgroundLight.withAlpha(0.3f));

		for (int i = 0; i <= numSteps; ++i) {
			float x = headerWidth + i * cellWidth;

			if (i % 4 == 0) {
				g.setColour(lookAndFeel->backgroundLight.withAlpha(0.5f));
				g.drawLine(x, headerHeight, x, getHeight(), 2.0f);
			}
			else {
				g.setColour(lookAndFeel->backgroundLight.withAlpha(0.2f));
				g.drawLine(x, headerHeight, x, getHeight(), 1.0f);
			}
		}

		g.setColour(lookAndFeel->backgroundLight.withAlpha(0.3f));
		for (int i = 0; i <= numTracks; ++i) {
			float y = headerHeight + i * cellHeight;
			g.drawLine(headerWidth, y, getWidth(), y, 1.0f);
		}
	}

	void PatternGrid::drawSteps(juce::Graphics& g) {
		if (!currentPattern) return;

		for (int track = 0; track < currentPattern->getNumTracks(); ++track) {
			for (int step = 0; step < currentPattern->getLength(); ++step) {
				auto bounds = getStepBounds(track, step).reduced(2.0f);
				const auto& stepObj = currentPattern->getTrack(track).getStep(step);

				if (track == hoveredTrack && step == hoveredStep) {
					g.setColour(lookAndFeel->stepHover);
					g.fillRoundedRectangle(bounds, 2.0f);
				}

				if (stepObj.isActive()) {
					float alpha = 0.5f + stepObj.getVelocity() * 0.5f;
					g.setColour(lookAndFeel->stepActive.withAlpha(alpha));
					g.fillRoundedRectangle(bounds.reduced(1.0f), 3.0f);

					float velocityHeight = bounds.getHeight() * stepObj.getVelocity() * 0.3f;
					g.setColour(lookAndFeel->accent);
					g.fillRect(bounds.getX(),
						bounds.getBottom() - velocityHeight,
						bounds.getWidth(),
						velocityHeight);
				}
				else {
					g.setColour(lookAndFeel->stepInactive);
					g.drawRoundedRectangle(bounds, 2.0f, 1.0f);
				}
			}
		}
	}

	void PatternGrid::drawPlayhead(juce::Graphics& g) {
		if (!currentPattern) return;

		int playheadPos = currentPattern->getCurrentStep();

		if (playheadPos < 0) return;

		float x = headerWidth + playheadPos * cellWidth;

		g.setColour(lookAndFeel->accent.withAlpha(0.3f));
		g.fillRect(x - 2, headerHeight, cellWidth + 4, getHeight() - headerHeight);

		g.setColour(lookAndFeel->accent);
		g.drawLine(x + cellWidth * 0.5f, headerHeight,
			x + cellWidth * 0.5f, getHeight(), 2.0f);
	}

	void PatternGrid::drawTrackLabels(juce::Graphics& g) {
		if (!currentPattern) return;

		g.setFont(12.0f);

		for (int i = 0; i < currentPattern->getNumTracks(); ++i) {
			auto bounds = juce::Rectangle<float>(0, headerHeight + i * cellHeight,
				headerWidth, cellHeight);

			g.setColour(lookAndFeel->backgroundMid);
			g.fillRect(bounds);

			g.setColour(lookAndFeel->textColour);
			g.drawText(currentPattern->getTrack(i).getName(),
				bounds.reduced(4.0f),
				juce::Justification::centredLeft);
		}
	}

	void PatternGrid::drawStepNumbers(juce::Graphics& g) {
		if (!currentPattern) return;

		g.setFont(10.0f);
		g.setColour(lookAndFeel->textDimmed);

		for (int i = 0; i < currentPattern->getLength(); ++i) {
			auto bounds = juce::Rectangle<float>(headerWidth + i * cellWidth, 0,
				cellWidth, headerHeight);

			if (i % 4 == 0) {
				g.setColour(lookAndFeel->textColour);
				g.setFont(11.0f);
			}
			else {
				g.setColour(lookAndFeel->textDimmed);
				g.setFont(10.0f);
			}

			g.drawText(juce::String(i + 1),
				bounds,
				juce::Justification::centred);
		}
	}

}