#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Core/Pattern.h"
#include "../Core/PatternEngine.h"
#include "LookAndFeel.h"

namespace BeatCrafter {

	class PatternGrid : public juce::Component,
		public juce::Timer
	{
	public:
		PatternGrid();
		~PatternGrid() override;

		void setPattern(const Pattern* pattern);
		void setPatternEngine(PatternEngine* engine);

		void paint(juce::Graphics&) override;
		void resized() override;
		void timerCallback() override;
		void markDirty();

	private:
		const Pattern* currentPattern = nullptr;
		PatternEngine* patternEngine = nullptr;
		ModernLookAndFeel* lookAndFeel = nullptr;

		float cellWidth = 0.0f;
		float cellHeight = 0.0f;
		int   headerWidth = 80;
		int   headerHeight = 20;
		int   playheadPosition = -1;

		juce::Image offscreenBuffer;
		bool bufferDirty = true;

		juce::Rectangle<float> getStepBounds(int track, int step) const;

		void drawBackground(juce::Graphics&);
		void drawGrid(juce::Graphics&);
		void drawSteps(juce::Graphics&);
		void drawPlayhead(juce::Graphics&);
		void drawTrackLabels(juce::Graphics&);
		void drawStepNumbers(juce::Graphics&);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternGrid)
	};
}