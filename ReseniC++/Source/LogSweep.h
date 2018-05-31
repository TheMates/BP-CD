/*
  ==============================================================================

    LogSweep.h
    Created: 5 Apr 2018 2:38:53pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once
#define _USE_MATH_DEFINES
#include "../JuceLibraryCode/JuceHeader.h"
#include "Signal.h"
#include <math.h>

class LogSweep : public Signal
{
public:
	//constructors
	LogSweep();
	LogSweep(float startFrequency, float stopFrequency);
	LogSweep(float startFrequency, float stopFrequency, int LengthTime);
	LogSweep(float startFrequency, float stopFrequency, int LengthTime, float Amplitude, int SampleRate);
	LogSweep(float startFrequency, float stopFrequency, int LengthTime, float Amplitude, int SampleRate, int BitRate);

	//destructor
	~LogSweep();

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
	double _multiplier;          //tbi in const
	double _fi = 0;             //start value

	void update();
};