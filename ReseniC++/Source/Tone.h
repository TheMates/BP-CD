/*
  ==============================================================================

    Tone.h
    Created: 5 Apr 2018 2:15:29am
    Author:  Matouš

  ==============================================================================
*/

#pragma once
#define _USE_MATH_DEFINES
#include "../JuceLibraryCode/JuceHeader.h"
#include "Signal.h"
#include "Tone.h"
#include <math.h>

class Tone : public Signal
{
public:
	//constructors
	Tone();
	Tone(float Frequency);
	Tone(float Frequency, int LengthTime);
	Tone(float Frequency, int LengthTime, float Amplitude);
	Tone(float Frequency, int LengthTime, float Amplitude, int SampleRate);
	Tone(float Frequency, int LengthTime, float Amplitude, int SampleRate, int BitRate);

	//destructors
	~Tone();

	//property-like
	float getFrequency(void);
	void setFrequency(float value);

	//methods
	void GenerateSignal() override;

private:
	float _frequency;
	void udpate();

};