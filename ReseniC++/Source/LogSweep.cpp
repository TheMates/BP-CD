/*
  ==============================================================================

    LogSweep.cpp
    Created: 5 Apr 2018 2:38:53pm
    Author:  Matouš

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"

#include "LogSweep.h"


#pragma region Constructors

LogSweep::LogSweep()
{
	_frequency1 = 1000;   //default
	_frequency2 = 2000;   //default
	update();
}

LogSweep::LogSweep(float startFrequency, float stopFrequency)
{
	_frequency1 = startFrequency;
	_frequency2 = stopFrequency;
	update();
}
LogSweep::LogSweep(float startFrequency, float stopFrequency, int LengthTime)
	: Signal::Signal(LengthTime)
{
	_frequency1 = startFrequency;
	_frequency2 = stopFrequency;
	update();
}

LogSweep::LogSweep(float startFrequency, float stopFrequency, int LengthTime, float Amplitude, int SampleRate)
	: Signal::Signal(Amplitude, LengthTime, SampleRate)
{
	_frequency1 = startFrequency;
	_frequency2 = stopFrequency;
	update();
}

LogSweep::LogSweep(float startFrequency, float stopFrequency, int LengthTime, float Amplitude, int SampleRate, int BitRate)
	: Signal::Signal(Amplitude, LengthTime, SampleRate, BitRate)
{
	_frequency1 = startFrequency;
	_frequency2 = stopFrequency;
	update();
}

#pragma endregion

LogSweep::~LogSweep()
{

}

#pragma region Property-like

float LogSweep::getStartFrequency()
{
	return _frequency1;
}
void LogSweep::setStartFrequency(float value)
{
	_frequency1 = value;
}

float LogSweep::getStopFrequency()
{
	return _frequency2;
}
void LogSweep::setStopFrequency(float value)
{
	_frequency2 = value;
}

void LogSweep::setLengthTime(int value)
{
	_lengthTime = value;
	_length = lengthToSamples(value);
	update();
}

#pragma endregion


void LogSweep::update()
{
	_deltafi = 2 * M_PI * (_frequency1 / (double)getSampleRate());
	_multiplier = pow(2.0, (log2(_frequency2 / _frequency1)) / getLength());
}

void LogSweep::GenerateSignal()
{
	_signal.setSize(1, getLength());

	for (int n = 0; n < getLength(); n++)
	{
		float tempSample = (float)sin(_fi);
		if (tempSample == 1.0f)
		{
			tempSample = 0.9999999f;    //if it's 1f exactly, it goes over in NAudio.Wave.WaveFileWriter.WriteSamples
		}
		_signal.setSample(0,n,getAmplitude()*tempSample);
		_fi += _deltafi;
		_deltafi *= _multiplier;
	}
}