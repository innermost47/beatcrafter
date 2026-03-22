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

		if (offscreenBuffer.isNull()
			|| offscreenBuffer.getWidth() != getWidth()
			|| offscreenBuffer.getHeight() != getHeight()
			|| bufferDirty)
		{
			offscreenBuffer = juce::Image(
				juce::Image::ARGB, getWidth(), getHeight(), true);
			juce::Graphics bg(offscreenBuffer);

			drawBackground(bg);
			drawGrid(bg);
			if (currentPattern) {
				drawSteps(bg);
				drawPlayhead(bg);
			}
			drawTrackLabels(bg);
			drawStepNumbers(bg);

			bufferDirty = false;
		}

		g.drawImageAt(offscreenBuffer, 0, 0);
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
	}

	void PatternGrid::drawGrid(juce::Graphics& g) {
		if (!currentPattern) return;

		int numSteps = currentPattern->getLength();
		int numTracks = currentPattern->getNumTracks();

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

				if (stepObj.isActive()) {
					float alpha = 0.5f + stepObj.getVelocity() * 0.5f;
					g.setColour(lookAndFeel->stepActive.withAlpha(alpha));
					g.fillRoundedRectangle(bounds.reduced(1.0f), 3.0f);
				}
				else {
					if (step % 4 == 0)
					{
						g.setColour(lookAndFeel->backgroundLight.withAlpha(0.4f));
						g.fillRoundedRectangle(bounds, 2.0f);
					}
					g.setColour(lookAndFeel->stepInactive);
					g.drawRoundedRectangle(bounds, 2.0f, 1.0f);
				}
			}
		}
	}

	void PatternGrid::drawPlayhead(juce::Graphics& g) {
		if (!patternEngine) return;

		int playheadPos = patternEngine->getCurrentStep();
		if (playheadPos < 0) return;

		float x = headerWidth + playheadPos * cellWidth;

		g.setColour(lookAndFeel->accent.withAlpha(0.3f));
		g.fillRect(x - 2.0f, (float)headerHeight, cellWidth + 4.0f, (float)(getHeight() - headerHeight));
	}

	void PatternGrid::drawTrackLabels(juce::Graphics& g) {
		if (!currentPattern) return;

		if (!lookAndFeel)
			lookAndFeel = dynamic_cast<ModernLookAndFeel*>(&getLookAndFeel());
		g.setFont(lookAndFeel->getPluginFont(ModernLookAndFeel::fontSizeGridLabel));
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

		for (int i = 0; i < currentPattern->getLength(); ++i) {
			auto bounds = juce::Rectangle<float>(headerWidth + i * cellWidth, 0,
				cellWidth, headerHeight);
			if (i % 4 == 0) {
				g.setColour(lookAndFeel->textColour);
				g.setFont(lookAndFeel->getPluginFont(ModernLookAndFeel::fontSizeStepNumberLarge));
			}
			else {
				g.setColour(lookAndFeel->textDimmed);
				g.setFont(lookAndFeel->getPluginFont(ModernLookAndFeel::fontSizeStepNumberSmall));
			}
			g.drawText(juce::String(i + 1), bounds, juce::Justification::centred);
		}
	}
}