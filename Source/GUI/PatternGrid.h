#pragma once
#include <juce_core/juce_core.h>  
#include "../Core/Pattern.h"
#include "LookAndFeel.h"

namespace BeatCrafter {

	class PatternGrid : public juce::Component,
		public juce::Timer {
	public:
		PatternGrid();
		~PatternGrid() override;

		void setPattern(Pattern* pattern);
		Pattern* getPattern() const { return currentPattern; }

		void paint(juce::Graphics& g) override;
		void resized() override;
		void mouseDown(const juce::MouseEvent& event) override;
		void mouseMove(const juce::MouseEvent& event) override;
		void mouseExit(const juce::MouseEvent& event) override;

		void timerCallback() override;

		void updateWithIntensity(const Pattern& intensifiedPattern);

		// Callbacks
		std::function<void(int track, int step, bool active)> onStepChanged;

	private:
		Pattern* currentPattern = nullptr;
		ModernLookAndFeel* lookAndFeel = nullptr;

		int hoveredTrack = -1;
		int hoveredStep = -1;
		int playheadPosition = -1;

		float cellWidth = 0;
		float cellHeight = 0;
		float headerWidth = 80.0f;
		float headerHeight = 30.0f;

		juce::Rectangle<float> getStepBounds(int track, int step) const;
		std::pair<int, int> getStepFromPosition(juce::Point<int> pos) const;

		void drawBackground(juce::Graphics& g);
		void drawGrid(juce::Graphics& g);
		void drawSteps(juce::Graphics& g);
		void drawPlayhead(juce::Graphics& g);
		void drawTrackLabels(juce::Graphics& g);
		void drawStepNumbers(juce::Graphics& g);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternGrid)
	};

} // namespace BeatCrafter