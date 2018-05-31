/*
  ==============================================================================

    LinSweep.h
    Created: 5 Apr 2018 3:17:31am
    Author:  Matouš

  ==============================================================================
*/

#pragma once
#define _USE_MATH_DEFINES
#include "../JuceLibraryCode/JuceHeader.h"
#include "Signal.h"
#include <math.h>

class LinSweep : public Signal
{
public:
	//constructors
	LinSweep();
	LinSweep(float startFrequency, float stopFrequency);
	LinSweep(float startFrequency, float stopFrequency, int LengthTime);
	LinSweep(float startFrequency, float stopFrequency, int LengthTime, float Amplitude, int SampleRate);
	LinSweep(float startFrequency, float stopFrequency, int LengthTime, float Amplitude, int SampleRate, int BitRate);

	//destructor
	~LinSweep();

	//property-like
	float getStartFrequency();
	void setStartFrequency(float value);

	float getStopFrequency();
	void setStopFrequency(float value);

	void setLengthTime(int value) override;
	
	void GenerateSignal() override;

private:
	float _frequency1;			
	float _frequency2;			
	double _deltafi;            //to be initialized in constructor
	double _increment;          //tbi in const
	double _fi = 0;             //start value

	void update();
};