/*
  ==============================================================================

    MLS.cpp
    Created: 5 Apr 2018 10:30:41pm
    Author:  Matouš

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MLS.h"

using namespace std;

#pragma region Constructors

MLS::MLS()
{
	_N = 12;    //default
	setLength(12);
	setBuffer();
	setTaps(12);
}

MLS::MLS(int N)
{
	_N = N;
	setLength(N);
	setBuffer();
	setTaps(N);
}
MLS::MLS(int N, float Amplitude)
	:Signal::Signal(Amplitude)
{
	_N = N;
	setLength(N);
	setBuffer();
	setTaps(N);
}
MLS::MLS(int N, float Amplitude, int SampleRate)
	: Signal::Signal(Amplitude, 1000, SampleRate)     //not using the Length, so passing "some" number. Is's overwritten in next few statements
{
	_N = N;
	setLength(N);
	setBuffer();
	setTaps(N);
}
MLS::MLS(int N, float Amplitude, int SampleRate, int BitRate)
	: Signal::Signal(Amplitude, 1000, SampleRate, BitRate)
{
	_N = N;
	//_buffer = new List<bool>(N);
	setLength(N);
	setBuffer();
	setTaps(N);
}

#pragma endregion

MLS::~MLS()
{

}


#pragma region Property-like
int MLS::getNOrder()
{
	return _N;
}
void MLS::setNOrder(int value)
{
	_N = value;
}

void MLS::setLength(int N)
{
	_length = (int)pow(2, N) - 1;
	_lengthTime = (int)(((double)_length / (double)_sampleRate) * 1000.0);
}

void MLS::setLengthTime(int value)
{
	throw "Setting length in time is not allowed in MLS class!";
}

#pragma endregion


void MLS::setBuffer()
{
	for (int i = 0; i < _N; i++)
		_buffer.push_back(true);
}
void MLS::setTaps(int N)
{
	switch (N)
	{
	case 2:  _taps = 2; _tap1 = 1; _tap2 = 2; break;
	case 3:  _taps = 2; _tap1 = 1; _tap2 = 3; break;
	case 4:  _taps = 2; _tap1 = 1; _tap2 = 4; break;
	case 5:  _taps = 2; _tap1 = 1; _tap2 = 5; break;
	case 6:  _taps = 2; _tap1 = 1; _tap2 = 6; break;
	case 7:  _taps = 2; _tap1 = 1; _tap2 = 7; break;
	case 8:  _taps = 4; _tap1 = 2; _tap2 = 3; _tap3 = 4; _tap4 = 8; break;
	case 9:  _taps = 2; _tap1 = 4; _tap2 = 9; break;
	case 10: _taps = 2; _tap1 = 3; _tap2 = 10; break;
	case 11: _taps = 2; _tap1 = 2; _tap2 = 11; break;
	case 12: _taps = 4; _tap1 = 1; _tap2 = 4; _tap3 = 6; _tap4 = 12; break;
	case 13: _taps = 4; _tap1 = 1; _tap2 = 3; _tap3 = 4; _tap4 = 13; break;
	case 14: _taps = 4; _tap1 = 1; _tap2 = 3; _tap3 = 5; _tap4 = 14; break;
	case 15: _taps = 2; _tap1 = 1; _tap2 = 15; break;
	case 16: _taps = 4; _tap1 = 2; _tap2 = 3; _tap3 = 5; _tap4 = 16; break;
	case 17: _taps = 2; _tap1 = 3; _tap2 = 17; break;
	case 18: _taps = 2; _tap1 = 7; _tap2 = 18; break;
	case 19: _taps = 4; _tap1 = 1; _tap2 = 2; _tap3 = 5; _tap4 = 19; break;
	case 20: _taps = 2; _tap1 = 3; _tap2 = 20; break;
	case 21: _taps = 2; _tap1 = 2; _tap2 = 21; break;
	case 22: _taps = 2; _tap1 = 1; _tap2 = 22; break;
	case 23: _taps = 2; _tap1 = 5; _tap2 = 23; break;
	case 24: _taps = 4; _tap1 = 1; _tap2 = 3; _tap3 = 4; _tap4 = 24; break;
	default: throw "N-order must be between 2 and 24!";
	
	}


}

void MLS::GenerateSignal()
{
	bool xorbit1;
	bool xorbit2;

	_signal.setSize(1, getLength());

	vector<bool>::iterator start, end;

	//the for cycle is duplicate, but testing for taps number is done only once
	if (_taps == 2)
	{
		for (int i = 0; i < pow(2, _N) - 1; i++)
		{
			xorbit1 = _buffer[_tap1 - 1] ^ _buffer[_tap2 - 1];      //carefully, this is zero based, in matlab, it's one based
			start = _buffer.begin();
			_buffer.insert(start, xorbit1);
			end = _buffer.end();
			_buffer.erase(end - 1);

			if (xorbit1 == true)
			{
				_signal.setSample(0, i, -1.0f * _ampl);
			}
			else
			{
				_signal.setSample(0, i, 0.9999999f * _ampl);
			}
		}
	}
	else
	{
		for (int i = 0; i < pow(2, _N) - 1; i++)
		{
			xorbit1 = _buffer[_tap1 - 1] ^ _buffer[_tap2 - 1];
			xorbit2 = _buffer[_tap3 - 1] ^ _buffer[_tap4 - 1];
			xorbit1 = xorbit1 ^ xorbit2;

			_buffer.insert(_buffer.begin(), xorbit1);

			_buffer.erase(_buffer.end()-1);
			if (xorbit1 == true)
			{
				_signal.setSample(0, i, -1.0f * _ampl);
			}
			else
			{
				_signal.setSample(0, i, 0.9999999f * _ampl);
			}
		}
	}
	_signal.reverse(0, _signal.getNumSamples());
}
