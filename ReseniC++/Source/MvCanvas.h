/*
  ==============================================================================

    MvCanvas.h
    Created: 4 May 2018 10:54:04am
    Author:  Matouš

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvStructs.h"

//==============================================================================
/*
*/
enum StrokeColours //00,2a,57,7f
{
	ch1 = 0xff0000FF,
	ch2 = 0xffff0000,
	ch3 = 0xff00ff00,
	ch4 = 0xff00002b,
	ch5 = 0xffff1ab8,
	ch6 = 0xffffd300,
	ch7 = 0xff005700,
	ch8 = 0xff8383ff,
	ch9 = 0xff9e4f46,
	ch10 = 0xff00ffc1,
	ch11 = 0xff008395,
	ch12 = 0xff95d34f,
	ch13 = 0xfff69edb,
	ch14 = 0xff7b1a69,
	ch15 = 0xff232308,
	ch16 = 0xfff68308
};
enum ToggleButtonColours //00,2a,57,7f
{
	tb1 = 0xc00000FF,
	tb2 = 0xc0ff0000,
	tb3 = 0xc000ff00,
	tb4 = 0xc000002b,
	tb5 = 0xc0ff1ab8,
	tb6 = 0xc0ffd300,
	tb7 = 0xc0005700,
	tb8 = 0xc08383ff,
	tb9 = 0xc09e4f46,
	tb10 = 0xc000ffc1,
	tb11 = 0xc0008395,
	tb12 = 0xc095d34f,
	tb13 = 0xc0f69edb,
	tb14 = 0xc07b1a69,
	tb15 = 0xc0232308,
	tb16 = 0xc0f68308

};
enum NoiseDecayButtonColour
{
	Off = 0xffc1c1c1,
	NoiseOn = 0xff87d0f8,
	DecayOn = 0xff82ee5c
};


class MvCanvas    : public Component
{
public:
    MvCanvas();
    ~MvCanvas();
	
    void paint (Graphics&) override;
    void resized() override;

	//==== Methods for drawing ====
	/**
	Draws grid on canvas. Calls plotHorizontalLine, plotVerticalLine.
	P:
	@param &g - Graphics, reference to local graphics, this method gets called from paint() method.
	*/
	void drawGrid(Graphics& g);
	/**
	Draws graphs on canvas. Calls plotAudioBufferChannel, plotHorizontalLine, plotVerticalLine, plotRegression.
	P:
	@param &g - Graphics, reference to local graphics, this method gets called from paint() method.
	*/
	void drawGraphs(Graphics& g);
	/**
	Plots audiobuffer on canvas
	P:
	@param &g - Graphics, reference to local graphics, this method gets called from paint() method.
	@param channelIndex - channel, to which the regression corresponds.
	*/
	void plotAudioBufferChannel(Graphics& g, int channelIndex);
	/**
	Plots horizontal line on canvas.
	P:
	@param &g - Graphics, reference to local graphics, this method gets called from drawGraphs() or drawGrid() method.
	@param yValue - float, y value of horizontal line to be drawn.
	@param isNoiseLevel - bool, true if this horizontal line represents noise level, than line will be dashed.
	@param channelIndex - channel, to which the regression corresponds. If -1, colour of line will be grey, not corresponding to channel colours.
	@param colour - Colour, colour of the line to be drawn. If empty, default black.
	*/
	void plotHorizontalLine(Graphics& g, float yValue, bool isNoiseLevel = false, int channelIndex = -1,Colour colour = Colours::black );
	/**
	Plots vertical line on canvas.
	P:
	@param &g - Graphics, reference to local graphics, this method gets called from drawGraphs() or drawGrid() or  method.
	@param xValue - float, x value of vertical line to be drawn.
	@param channelIndex - channel, to which the regression corresponds.
	*/
	void plotVerticalLine(Graphics& g, float xValue, Colour colour = Colours::black);
	/**
	Plots regression line on canvas.
	P:
	@param &g - Graphics, reference to local graphics, this method gets called from drawGraphs() method.
	@param parameters - DecayParameters, parameters of regression line.
	@param channelIndex - channel, to which the regression corresponds.
	*/
	void plotRegression(Graphics& g,DecayParameters parameters, int channelIndex, int regressionIndex);

	//==== Methods for managing pointers ====
	/**
	Sets pointer to AudioBuffer data.
	P:
	@param &audioBufferToReadFrom - ScopedPointer<AudioBuffer<float>>, pointer to AudioBuffer
	*/
	void setAudioBufferPointer(ScopedPointer<AudioBuffer<float>> &audioBufferToReadFrom );
	/**
	Sets pointer to additional LundebyReturn data.
	P:
	@param &additionaLundebyData - ScopedPointer<Array<LundebyReturn>>, pointer to LundebyReturn data.
	*/
	void setAdditionalDataPointer(ScopedPointer<Array<LundebyReturn>> &additionaLundebyData);
	/**
	Sets pointer to additional SchroederReturn data.
	P:
	@param &additionaSchroederData - ScopedPointer<Array<SchroederReturn>>, pointer to SchroederReturn data.
	*/
	void setAdditionalDataPointer(ScopedPointer<Array<SchroederReturn>>  &additionaSchroederData);
	/**
	Releases pointers to all data.
	*/
	void releaseAditionalData();
	/**
	Sets pointers to channel, noise, decays electors.
	P:
	@param &channelSelectorsToReferTo - OwnedArray<ScopedPointer<ToggleButton>>, array of channel selectors (toggle buttons).
	@param &noiseSelectorsToReferTo - OwnedArray<ScopedPointer<TextButton>>, array of noise selectors (text buttons).
	@param &decaySelectorsToReferTo - OwnedArray<ScopedPointer<TextButton>>, array of decay selectors (text buttons).
	*/
	void setChannelSelectorsPointer(OwnedArray<ScopedPointer<ToggleButton>> &channelSelectorsToReferTo,
		OwnedArray<ScopedPointer<TextButton>> &noiseSelectorsToReferTo, OwnedArray<ScopedPointer<TextButton>> &decay10SelectorsToReferTo,
		OwnedArray<ScopedPointer<TextButton>> &decay20SelectorsToReferTo, OwnedArray<ScopedPointer<TextButton>> &decay30SelectorsToReferTo);

	//==== Setting other parameters ====
	/**
	Sets current sample rate
	P:
	@param currentSampleRate - float, current sample rate from AudioDeviceManager.
	*/
	void setCurrentSampleRate(float currentSampleRate);
	/**
	Sets public variables verticalStep, horizontalSetp, firstVerticalValue, firstHorizontalValue.
	P:
	@returns - GridParameters, struct of 4 float values: verticalStepPx, horizontalSetpPx, firstVerticalValuePx, firstHorizontalValuePx
	*/
	GridParameters setGridParameters();
	/**
	Sets current max and min display values.
	P:
	@param startDisplaySample - int, current first sample of canvas, that will be drawn. Canvas gets resized if zoomed, so not all pixels are viewed in parent Viewport
	@param endDisplaySample - int, current last sample of canvas, that will be drawn.
	@param maxValuePosition - int, current Y position of max value, that will be drawn.
	@param minValuePosition - int, current Y position of min value, that will be drawn.
	@param viewedWidth - int, current viewed width of parent Viewport.
	@param viewedHeight - int, current viewed height of parent Viewport.
	*/
	void setDisplayBoundaries(int startDisplaySample, int endDisplaySample, int maxValuePosition, int minValuePosition, int viewedWidth,int viewedHeight);

	//==== Public variables, so the parent (MvSignalPlot) can acces them ====
	float verticalStep, horizontalSetp, firstVerticalValue, firstHorizontalValue ;

private:
	//==== Pointers to data ====
	ScopedPointer<AudioBuffer<float>> audioBufferPointer;
	OptionalScopedPointer<Array<LundebyReturn>> additionaLundebyDataPointer;		//gotta use optional scoped pointers, bcs i dont want to delete the original data
	OptionalScopedPointer<Array<SchroederReturn>> additionaSchroederDataPointer;
	//==== Poitners to channel, decay and noise selectors from MvSignalPlot, which owns this component ====
	ScopedPointer<OwnedArray<ScopedPointer<ToggleButton>>> _channelSelectors{};
	ScopedPointer<OwnedArray<ScopedPointer<TextButton>>> _decay10Selectors{}, _decay20Selectors{}, _decay30Selectors{}, _noiseSelectors{};
	
	
	//==== My variables ====
	Range<float> globalDisplayRange{};	//1.1 the actual value range
	int currentStartDispaySample, currentEndDisplaySample, currentViewWidth, currentViewHeight;
	float currentMaxDisplayValue, currentMinDisplayValue, _currentSampleRate;

	//==== My methods ====
	/**
	Sets global range with respect to AudioBuffer data and values inside.
	Global range si actually 1.1 greater than the values range, so the data is not drawn to the edge of canvas.
	*/
	void setGlobalRange();
	/**
	Returns power of 10, with which number must be multiplied to get number between 0 -- 9.99
	*/
	int getNumberOrder(const float x);
	/**
	Returns firts grid value - depends on zoom and slider position.
	P:
	@param stemp - float, step of grid.
	@param isHorizontal - bool, true if i want to compute first horizontal value.
	@returns - float, first value.
	*/
	float getFirstGridValue(float step, bool isHorizontal = false);
	/**
	Returns grid step - 0.1, 0.2, 0.5, 1, 2,...
	P:
	@param isHroizontal - bool, true if i want to compute horizontal steps
	@return - float, step value.
	*/
	float getGridStep(bool isHorizontal);
	/**
	Returns intersample position.
	P:
	@param sample1 - float, first smaple
	@param sample2 - float, second sample
	@param position - float, position between them (if greater than 1, only decimals will be taken)
	@returns - float, intersample value.
	*/
	float interpolate(float sample1, float sample2, float position);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MvCanvas)
};

