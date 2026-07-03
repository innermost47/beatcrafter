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
		bufferDirty = true;
		repaint();
	}

	void PatternGrid::setPatternEngine(PatternEngine* engine) {
		patternEngine = engine;
	}

	void PatternGrid::paint(juce::Graphics& g)
	{

		if (!lookAndFeel)
			lookAndFeel = dynamic_cast<ModernLookAndFeel*>(&getLookAndFeel());

		float scale = g.getInternalContext().getPhysicalPixelScaleFactor();
		int bufferW = juce::roundToInt(getWidth() * scale);
		int bufferH = juce::roundToInt(getHeight() * scale);

		if (offscreenBuffer.isNull()
			|| offscreenBuffer.getWidth() != bufferW
			|| offscreenBuffer.getHeight() != bufferH
			|| bufferDirty)
		{

			offscreenBuffer = juce::Image(
				juce::Image::ARGB, bufferW, bufferH, true);
			juce::Graphics bg(offscreenBuffer);
			bg.addTransform(juce::AffineTransform::scale(scale));

			drawBackground(bg);
			if (currentPattern) {
				drawLEDs(bg);
				drawPlayhead(bg);
			}
			drawTrackLabels(bg);
			drawStepNumbers(bg);

			bufferDirty = false;
		}

		g.drawImageTransformed(offscreenBuffer, juce::AffineTransform::scale(1.0f / scale));
	}

	void PatternGrid::markDirty()
	{
		bufferDirty = true;
		if (!isTimerRunning())
			repaint();
	}

	void PatternGrid::resized() {
		auto bounds = getLocalBounds();
		int numSteps = currentPattern ? currentPattern->getLength() : 16;
		int numTracks = currentPattern ? currentPattern->getNumTracks() : 8;
		cellWidth = (bounds.getWidth() - headerWidth) / static_cast<float>(numSteps);
		cellHeight = (bounds.getHeight() - headerHeight) / static_cast<float>(numTracks);

		cellHeight = juce::jmin(cellHeight, 18.0f);
		cellWidth = juce::jmin(cellWidth, 22.0f);

		bufferDirty = true;
	}

	void PatternGrid::timerCallback() {
		if (patternEngine) {
			int newPlayhead = patternEngine->getCurrentStep();
			if (newPlayhead != playheadPosition) {
				playheadPosition = newPlayhead;
				bufferDirty = true;
				repaint();
			}
		}
	}

	juce::Rectangle<float> PatternGrid::getStepBounds(int track, int step) const {
		float x = headerWidth + step * cellWidth;
		float y = headerHeight + track * cellHeight;
		return juce::Rectangle<float>(x, y, cellWidth, cellHeight);
	}

	void PatternGrid::drawBackground(juce::Graphics& g) {
		g.fillAll(lookAndFeel->backgroundDark);
		g.setColour(lookAndFeel->backgroundMid);
		g.fillRect(juce::Rectangle<float>(0.0f, 0.0f, (float)getWidth(), (float)headerHeight));
		g.fillRect(juce::Rectangle<float>(0.0f, 0.0f, (float)headerWidth, (float)getHeight()));

		if (currentPattern) {
			int numSteps = currentPattern->getLength();
			g.setColour(lookAndFeel->backgroundLight.withAlpha(0.18f));
			for (int i = 4; i < numSteps; i += 4) {
				float x = headerWidth + i * cellWidth;
				g.drawLine(x, (float)headerHeight, x, (float)getHeight(), 1.0f);
			}
		}
	}

	void PatternGrid::drawLEDs(juce::Graphics& g) {
		if (!currentPattern) return;

		float ledDiameter = juce::jmin(cellWidth, cellHeight) * 0.5f;
		ledDiameter = juce::jlimit(5.0f, 9.0f, ledDiameter);

		for (int track = 0; track < currentPattern->getNumTracks(); ++track) {
			for (int step = 0; step < currentPattern->getLength(); ++step) {
				auto cell = getStepBounds(track, step);
				float cx = cell.getCentreX();
				float cy = cell.getCentreY();

				auto ledBounds = juce::Rectangle<float>(ledDiameter, ledDiameter)
					.withCentre({ cx, cy });

				const auto* stepObj = currentPattern->getTrack(track).getStep(step);

				if (stepObj) {
					if (stepObj->isActive()) {
						float velocity = stepObj->getVelocity();
						float alpha = juce::jlimit(0.f, 1.f, 0.7f + velocity * 0.3f);
						g.setColour(lookAndFeel->stepActive.withAlpha(alpha));
						g.fillEllipse(ledBounds);
					}
					else {
						bool isDownbeat = (step % 4 == 0);
						g.setColour(lookAndFeel->stepInactive.withAlpha(isDownbeat ? 0.75f : 0.55f));
						g.fillEllipse(ledBounds);
					}
				}
			}
		}
	}

	void PatternGrid::drawPlayhead(juce::Graphics& g) {
		if (!patternEngine) return;

		int playheadPos = patternEngine->getCurrentStep();
		if (playheadPos < 0 || !currentPattern) return;

		float x = headerWidth + playheadPos * cellWidth;
		g.setColour(lookAndFeel->accent.withAlpha(0.18f));
		g.fillRect(x, (float)headerHeight, cellWidth, (float)(getHeight() - headerHeight));

		auto marker = juce::Rectangle<float>(x + 2.0f, headerHeight - 3.0f,
			cellWidth - 4.0f, 2.0f);
		g.setColour(lookAndFeel->accent);
		g.fillRect(marker);
	}

	void PatternGrid::drawTrackLabels(juce::Graphics& g) {
		if (!currentPattern) return;

		if (!lookAndFeel)
			lookAndFeel = dynamic_cast<ModernLookAndFeel*>(&getLookAndFeel());

		g.setFont(lookAndFeel->getPluginFont(ModernLookAndFeel::fontSizeGridLabel));

		for (int i = 0; i < currentPattern->getNumTracks(); ++i) {
			int y = headerHeight + juce::roundToInt(i * cellHeight);
			int h = juce::roundToInt(cellHeight);
			auto bounds = juce::Rectangle<int>(0, y, headerWidth, h);

			g.setColour(lookAndFeel->backgroundMid);
			g.fillRect(bounds);
			g.setColour(lookAndFeel->textColour);
			g.drawText(currentPattern->getTrack(i).getName(),
				bounds.reduced(4, 0),
				juce::Justification::centredLeft);
		}

	}

	void PatternGrid::drawStepNumbers(juce::Graphics& g) {
		if (!currentPattern) return;
		g.setColour(lookAndFeel->textDimmed);
		g.setFont(lookAndFeel->getPluginFont(ModernLookAndFeel::fontSizeStepNumberSmall));

		for (int i = 0; i < currentPattern->getLength(); ++i) {
			if (i % 4 != 0) continue;
			int x = headerWidth + juce::roundToInt(i * cellWidth);
			int w = juce::roundToInt(cellWidth * 4);
			auto bounds = juce::Rectangle<int>(x, 0, w, headerHeight);
			g.drawText(juce::String(i + 1), bounds.reduced(2, 0),
				juce::Justification::centredLeft);
		}

	}
}