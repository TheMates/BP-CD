/*
  ==============================================================================

    Tone.cpp
    Created: 5 Apr 2018 2:15:29am
    Author:  Matouš

  ==============================================================================
*/



#include "../JuceLibraryCode/JuceHeader.h"
#include "Tone.h"

#pragma region Constructors

Tone::Tone()
{
	_frequency = 1000;		//default
}

Tone::Tone(float Frequency)
{
	_frequency = Frequency;
}
Tone::Tone(float Frequency, int LengthTime)
	:Signal::Signal(LengthTime)
{
	_frequency = Frequency;
}
Tone::Tone(float Frequency, int LengthTime, float Amplitude)
	: Signal::Signal(Amplitude, LengthTime)
{
	_frequency = Frequency;
}
Tone::Tone(float Frequency, int LengthTime, float Amplitude, int SampleRate)
	: Signal::Signal(Amplitude, LengthTime, SampleRate)
{
	_frequency = Frequency;
}
Tone::Tone(float Frequency, int LengthTime, float Amplitude, int SampleRate, int BitRate)
	: Signal::Signal(Amplitude, LengthTime, SampleRate, BitRate)
{
	_frequency = Frequency;
}

#pragma endregion



Tone::~Tone()
{

}

#pragma region Property-like

float Tone::getFrequency()
{
	return _frequency;
}
void Tone::setFrequency(float value)
{
	_frequency = value;
}

#pragma endregion

void Tone::udpate()
{

}


void Tone::GenerateSignal()
{
	_signal.setSize(1, getLength());

	for (int n = 0; n < getLength(); n++)
	{
		float tempSample = (float)(getAmplitude()*sin(2 * M_PI *n*getFrequency() / getSampleRate()));
		if (tempSample == 1.0f)
		{
			tempSample = 0.9999999f;    //if it's 1f exactly, it goes over in NAudio.Wave.WaveFileWriter.WriteSamples
		}
		_signal.setSample(0,n,tempSample);
	}
}