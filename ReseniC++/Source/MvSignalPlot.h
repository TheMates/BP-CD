/*
  ==============================================================================

    MvSignalPlot.h
    Created: 30 Apr 2018 12:16:08pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <limits>
#include "MvCanvas.h"
#include "MvViewport.h"
#include "MvStructs.h"

//==============================================================================
enum SelectedPlotTab
{
	IR = 0,
	Lundeby,
	Schoeder,
	TF
};

class MvSignalPlot    : public Component, public Button::Listener, public Slider::Listener, public ScrollBar::Listener, public MvViewport::Listener
{
public:
    MvSignalPlot();
    ~MvSignalPlot();
	
    void paint (Graphics&) override;
    void resized() override;

	//==== Methods for managing pointers ====
	/**
	Adds audio buffer pointer to data to canvas.
	P:
	@param &audioBufferToReadFrom - AudioBuffer<float>
	*/
	void addToGraph(AudioBuffer<float> &audioBufferToReadFrom);
	/**
	Adds additional Lundeby data pointers to canvas.
	P:
	@param &additionalDataToReadFrom - Array<LundebyReturn>
	*/
	void addToGraphAdditional(Array<LundebyReturn> &additionalDataToReadFrom);
	/**
	Adds additional Schroeder data pointers to canvas.
	P:
	@param &additionalDataToReadFrom - Array<SchroederReturn>
	*/
	void addToGraphAdditional(Array<SchroederReturn> &additionalDataToReadFrom);
	/**
	Releases pointers to data.
	*/
	void releaseAdditional();

	//==== Other setters ====
	/**
	Updates canvas with current sample rate.
	P:
	@param currentSampleRate - float
	*/
	void setCurrentSampleRate(float currentSampleRate);
	/**
	Changes current tab variable in this component.
	P:
	@param tabIndex - int
	*/
	void setCurrentTab(int tabIndex);
	
	//==== Callbacks ====
	/**
	Callback for clicked button
	P:
	@param button - *Button
	*/
	void buttonClicked(Button* button) override;
	/**
	Callback method for zoom sliders. Takes care of zooming the canvas and setting scrollbars.
	P:
	@param slider - *Slider
	*/
	void sliderValueChanged(Slider *slider) override;
	/**
	Callback for scrollbar of viewport.
	P:
	@param scrollBarThatHasMoved - *ScrollBar
	@param newRangeStart - double
	*/
	void scrollBarMoved(ScrollBar * scrollBarThatHasMoved, double newRangeStart) override;
	/**
	Updates canvas display boundaries with respect to new visible area in viewport.
	P:
	@param &newVisibleArea - Rectangle<int>, new area
	*/
	void visibleAreaChanged(const Rectangle< int > &newVisibleArea) override;


private:
	//==== My components ====
	ScopedPointer<MvViewport> viewPort;
	ScopedPointer<Slider> xZoomSlider, yZoomSlider;
	ScopedPointer<MvCanvas> canvas;

	//==== My variables ====
	float xZoom = 1.0, yZoom= 1.0;
	int currentTab = 0;

	//==== Read pointer ====
	ScopedPointer<AudioBuffer<float>> audioBufferPointer;
	ScopedPointer<Array<LundebyReturn>> additionaLundebyDataPointer;;
	ScopedPointer<Array<SchroederReturn>> additionaSchroederDataPointer;

	//==== Other fields ====
	OwnedArray<ScopedPointer<ToggleButton>> _channelSelectors;
	OwnedArray<ScopedPointer<TextButton>> _decay10Selectors;
	OwnedArray<ScopedPointer<TextButton>> _decay20Selectors;
	OwnedArray<ScopedPointer<TextButton>> _decay30Selectors;
	OwnedArray<ScopedPointer<TextButton>> _noiseSelectors;
	OwnedArray<ScopedPointer<Label>> _channelSelectorsLabels;

	//==== Other methods ====
	/**
	Manages owned arrays of toggle and text buttons for selectors.
	*/
	void updateChannelInfo();
	/**
	Updates canvas pointers to data and channel, decay and nosie selectors.
	*/
	void updateCanvasPointers();
	/**
	Updates canvas with actual start, end sample, max, min value pixel and viewport width and height.
	*/
	void updateCanvasDisplayBoundaries();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MvSignalPlot)
};