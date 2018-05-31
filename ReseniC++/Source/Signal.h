/*
  ==============================================================================

    SignalGenerator.h
    Created: 5 Apr 2018 1:09:18am
    Author:  Matouš

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include <math.h>
#include <exception>


//==============================================================================
/*
*/
class Signal
{
public:
	//constructors
    Signal();
	Signal(int LengthTime);
	Signal(float Amplitude);
	Signal(float Amplitude, int LengthTime);
	Signal(float Amplitude, int LengthTime, int SampleRate);
	Signal(float Amplitude, int LengthTime, int SampleRate, int BitRate);

	//descturctor
	~Signal();

	//methods

	//float* ToArray();

	//property-like
	int getBitrate();
	void setBitrate(int value);

	int getSampleRate();
	void setSampleRate(int value);

	float getAmplitude();
	void setAmplitude(float value);

	int getLength();
	virtual void setLength(int value);

	int getLengthTime();
	virtual void setLengthTime(int value);		//it's not alway enabled to set this property
	/**
	Function generates signal from parameters given in constructor. When object type Signal is created,
	the signal itself is not generated until you call this method. Signal object contains a vector field,
	which is empty after initializing, so this method needs to be called to actually generate the signal itself.
	*/
	virtual void GenerateSignal() = 0;		//needs to be defined at all children
	/**
	Returns vector<float> that is the signal itself. It is important, to call the GenerateSignal() method before
	you call this one, otherwise the signal is not generated yet.
	P:
	@return vector<float>, generated signal
	*/
	AudioBuffer<float> GetSignal();
	/**
	Returns float[] (float*) array of signal. It is important, to call the GenerateSignal() method before
	you call this one, otherwise the signal is not generated yet.
	P:
	@return float*, generated signal.
	*/
protected:
	AudioBuffer<float> _signal;
	int _sampleRate;
	int _bitRate;
	int _length;
	int _lengthTime;
	float _ampl;

	//methods
	int lengthToSamples(int length);

};
