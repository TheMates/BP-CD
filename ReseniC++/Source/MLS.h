/*
  ==============================================================================

    MLS.h
    Created: 5 Apr 2018 10:30:41pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once
#define _USE_MATH_DEFINES
#include "../JuceLibraryCode/JuceHeader.h"
#include "Signal.h"

class MLS : public Signal
{
public:
	//constructors
	MLS();
	MLS(int N);
	MLS(int N, float Amplitude);
	MLS(int N, float Amplitude, int SampleRate);
	MLS(int N, float Amplitude, int SampleRate, int BitRate);

	//destructor
	~MLS();

	//properties-like
	int getNOrder();
	void setNOrder(int value);

	void setLength(int N) override;

	void setLengthTime(int value) override;

	void GenerateSignal() override;


private:
	int _N;
	int _taps;
	int _tap1;
	int _tap2;
	int _tap3;
	int _tap4;
	std::vector<bool> _buffer;

	void setTaps(int N);
	void setBuffer();

};
