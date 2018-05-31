/*
  ==============================================================================

    Signal.cpp
    Created: 5 Apr 2018 1:09:18am
    Author:  Matouš

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Signal.h"


using namespace std;

//==============================================================================

#pragma region Constructors

Signal::Signal()
{
	_sampleRate = 44100;    // default
	_bitRate = 24;         //default
	_length = 44100;       //default 1s
	_ampl = 0.5f;         //default
	_lengthTime = 1000;     //default, in miliseconds
	_signal = AudioBuffer<float>(); 

}

Signal::Signal(int LengthTime)
{
	_sampleRate = 44100;    // default
	_bitRate = 24;         //default
	_lengthTime = LengthTime;
	_ampl = 0.5f;         //default
	_length = lengthToSamples(_lengthTime);
	_signal = AudioBuffer<float>();
}

Signal::Signal(float Amplitude)
{
	_sampleRate = 44100;    // default
	_bitRate = 24;         //default
	_length = 44100;       //default 1s
	_ampl = Amplitude;
	_lengthTime = 1000;     //default, in miliseconds
	_signal = AudioBuffer<float>();

}

Signal::Signal(float Amplitude, int LengthTime)
{
	_sampleRate = 44100;    // default
	_bitRate = 24;         //default
	_lengthTime = LengthTime;
	_ampl = Amplitude;
	_length = lengthToSamples(_lengthTime);
	_signal = AudioBuffer<float>();

}

Signal::Signal(float Amplitude, int LengthTime, int SampleRate)
{
	_sampleRate = SampleRate;
	_bitRate = 24;         //default
	_lengthTime = LengthTime;
	_ampl = Amplitude;
	_length = lengthToSamples(_lengthTime);
	_signal = AudioBuffer<float>();

}

Signal::Signal(float Amplitude, int LengthTime, int SampleRate, int BitRate)
{
	_sampleRate = SampleRate;
	_bitRate = BitRate;
	_lengthTime = LengthTime;
	_ampl = Amplitude;
	_length = lengthToSamples(_lengthTime);
	_signal = AudioBuffer<float>();
}

#pragma endregion


Signal::~Signal()
{
	//delete &_sampleRate;
	//delete &_bitRate;
	//delete &_lengthTime;
	//delete &_length;
	//delete &_ampl;

	_signal.clear();	//vyčistím vektor
	/*//trik na uvolnění místa v paměti, vytvoří se temp vektor 
	//kterej je prázdnej a prohodí se s tím původním, takže se původní data zapomenou trvale
	std::vector<float>(_signal).swap(_signal);	
	//delete &_signal;
	*/
	_signal.setSize(0, 0, false, true, false);	//mělo by to uvolnit alokovaný místo

}



#pragma region Properties-like

int Signal::getBitrate()
{
	return _bitRate;
}
void Signal::setBitrate(int value)
{
	_bitRate = value;
}

int Signal::getSampleRate()
{
	return _sampleRate;
}
void Signal::setSampleRate(int value)
{
	_sampleRate = value;
}

float Signal::getAmplitude()
{
	return _ampl;
}
void Signal::setAmplitude(float value)
{
	_ampl = value;
}

int Signal::getLength()
{
	return _length;
}
void Signal::setLength(int value)
{
	_length = value;
}

int Signal::getLengthTime()
{
	return _lengthTime;
}
void Signal::setLengthTime(int value)		//it's not alway enabled to set this property
{
	_lengthTime = value;
	_length = lengthToSamples(value);
}

#pragma endregion


int Signal::lengthToSamples(int lengthTime)
{
	return (int)(((float)getSampleRate() / 1000.0)*lengthTime);	
}

/*
float* Signal::ToArray()
{
	float* resarray = new float[_signal.size()];
	copy(_signal.begin(), _signal.end(), resarray);
	return resarray;
}
*/
AudioBuffer<float> Signal::GetSignal()
{
	if (_signal.getNumSamples() < 1)
		throw "Signal is not created yet.";
	return _signal;
}
/*
float* Signal::GetSignalArr()
{
	return ToArray();
}
*/
