#pragma once
#include <juce_core/juce_core.h>  
#include "../Core/Pattern.h"
#include "../Core/PatternEngine.h"
#include "LookAndFeel.h"

namespace BeatCrafter {
	class VelocityPopup : public juce::Component {
	public:
		VelocityPopup(std::function<void(float)> onVelocityChange,
			std::function<void()> onClose)
			: onVelocityChanged(onVelocityChange), onClosed(onClose) {
			addAndMakeVisible(velocitySlider);
			velocitySlider.setSliderStyle(juce::Slider::LinearVertical);
			velocitySlider.setRange(0.1, 1.0, 0.01);
			velocitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

			velocitySlider.onValueChange = [this]() {
				if (onVelocityChanged) {
					onVelocityChanged(velocitySlider.getValue());
				}
				};
			addAndMakeVisible(closeButton);
			closeButton.setButtonText("x");
			closeButton.onClick = [this]() {
				if (onClosed) {
					onClosed();
				}
				};
			setSize(60, 160);
		}

		void paint(juce::Graphics& g) override {
			g.setColour(juce::Colour(0xFF2A2A2A));
			g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);

			g.setColour(juce::Colour(0xFF404040));
			g.drawRoundedRectangle(getLocalBounds().toFloat(), 4.0f, 1.0f);
		}


		void resized() override {
			auto bounds = getLocalBounds().reduced(4);
			closeButton.setBounds(bounds.removeFromTop(16).removeFromRight(16));
			bounds.removeFromTop(4);
			velocitySlider.setBounds(bounds);
		}

		void setVelocity(float velocity) {
			velocitySlider.setValue(velocity, juce::dontSendNotification);
		}

		bool hitTest(int x, int y) override {
			return true;
		}

	private:
		juce::Slider velocitySlider;
		juce::TextButton closeButton;
		std::function<void(float)> onVelocityChanged;
		std::function<void()> onClosed;
	};

	class PatternGrid : public juce::Component, public juce::DragAndDropContainer,
		public juce::Timer {
	public:
		PatternGrid();
		~PatternGrid() override;

		void setPattern(const Pattern* pattern);
		void setPatternEngine(PatternEngine* engine);
		const Pattern* getPattern() const {
			return currentPattern;
		}

		void paint(juce::Graphics& g) override;
		void resized() override;
		void mouseDown(const juce::MouseEvent& event) override;
		void mouseMove(const juce::MouseEvent& event) override;
		void mouseDrag(const juce::MouseEvent& event) override;
		void mouseUp(const juce::MouseEvent& event) override;
		void mouseExit(const juce::MouseEvent& event) override;

		void timerCallback() override;

		void updateWithIntensity(const Pattern& intensifiedPattern);

		std::function<void(int track, int step, bool active)> onStepChanged;

	private:
		const Pattern* currentPattern = nullptr;
		ModernLookAndFeel* lookAndFeel = nullptr;
		PatternEngine* patternEngine = nullptr;

		int hoveredTrack = -1;
		int hoveredStep = -1;
		int playheadPosition = -1;

		float cellWidth = 0;
		float cellHeight = 0;
		float headerWidth = 80.0f;
		float headerHeight = 30.0f;

		std::unique_ptr<VelocityPopup> velocityPopup;
		bool isVelocityPopupOpen = false;

		juce::Rectangle<float> getStepBounds(int track, int step) const;
		std::pair<int, int> getStepFromPosition(juce::Point<int> pos) const;

		void drawBackground(juce::Graphics& g);
		void drawGrid(juce::Graphics& g);
		void drawSteps(juce::Graphics& g);
		void drawPlayhead(juce::Graphics& g);
		void drawTrackLabels(juce::Graphics& g);
		void drawStepNumbers(juce::Graphics& g);
		void showVelocityPopup(int track, int step);
		void closeVelocityPopup();

		void startMidiDragAndDrop(const juce::MouseEvent& event);
		juce::MidiMessageSequence createMidiFromPattern() const;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternGrid)
	};

}