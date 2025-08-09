#include "PatternGrid.h"

namespace BeatCrafter {

	PatternGrid::PatternGrid() {
		lookAndFeel = dynamic_cast<ModernLookAndFeel*>(&getLookAndFeel());
		startTimerHz(30); // 30 FPS pour animation fluide
	}

	PatternGrid::~PatternGrid() {
		stopTimer();
	}

	void PatternGrid::setPattern(Pattern* pattern) {
		currentPattern = pattern;
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

		// Calculate cell dimensions
		int numSteps = currentPattern ? currentPattern->getLength() : 16;
		int numTracks = currentPattern ? currentPattern->getNumTracks() : 8;

		cellWidth = (bounds.getWidth() - headerWidth) / static_cast<float>(numSteps);
		cellHeight = (bounds.getHeight() - headerHeight) / static_cast<float>(numTracks);
	}

	void PatternGrid::mouseDown(const juce::MouseEvent& event) {
		if (!currentPattern) return;

		auto [track, step] = getStepFromPosition(event.getPosition());

		if (track >= 0 && step >= 0) {
			auto& stepObj = currentPattern->getTrack(track).getStep(step);
			bool newState = !stepObj.isActive();
			stepObj.setActive(newState);

			if (onStepChanged)
				onStepChanged(track, step, newState);

			repaint();
		}
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

		// Header backgrounds
		g.setColour(lookAndFeel->backgroundMid);
		g.fillRect(juce::Rectangle<float>(0.0f, 0.0f, (float)getWidth(), (float)headerHeight));
		g.fillRect(juce::Rectangle<float>(0.0f, 0.0f, (float)headerWidth, (float)getHeight()));
	}

	void PatternGrid::drawGrid(juce::Graphics& g) {
		if (!currentPattern) return;

		int numSteps = currentPattern->getLength();
		int numTracks = currentPattern->getNumTracks();

		g.setColour(lookAndFeel->backgroundLight.withAlpha(0.3f));

		// Vertical lines
		for (int i = 0; i <= numSteps; ++i) {
			float x = headerWidth + i * cellWidth;

			// Stronger lines every 4 steps (beat markers)
			if (i % 4 == 0) {
				g.setColour(lookAndFeel->backgroundLight.withAlpha(0.5f));
				g.drawLine(x, headerHeight, x, getHeight(), 2.0f);
			}
			else {
				g.setColour(lookAndFeel->backgroundLight.withAlpha(0.2f));
				g.drawLine(x, headerHeight, x, getHeight(), 1.0f);
			}
		}

		// Horizontal lines
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

				// Hover effect
				if (track == hoveredTrack && step == hoveredStep) {
					g.setColour(lookAndFeel->stepHover);
					g.fillRoundedRectangle(bounds, 2.0f);
				}

				// Step state
				if (stepObj.isActive()) {
					// Velocity affects opacity
					float alpha = 0.5f + stepObj.getVelocity() * 0.5f;
					g.setColour(lookAndFeel->stepActive.withAlpha(alpha));
					g.fillRoundedRectangle(bounds.reduced(1.0f), 3.0f);

					// Velocity indicator (small bar at bottom)
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
		if (!currentPattern || playheadPosition < 0) return;

		float x = headerWidth + playheadPosition * cellWidth;

		// Glow effect
		g.setColour(lookAndFeel->accent.withAlpha(0.3f));
		g.fillRect(x - 2, headerHeight, cellWidth + 4, getHeight() - headerHeight);

		// Main playhead line
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

			// Highlight beat numbers
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

} // namespace BeatCrafter