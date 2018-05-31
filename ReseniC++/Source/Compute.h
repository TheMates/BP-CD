/*
  ==============================================================================

    Compute.h
    Created: 6 Apr 2018 2:55:14am
    Author:  Matouš

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <exception>
#include <vector>
#include <iostream>
#include <algorithm>
#include "MvStructs.h"
#include <fstream>
#include <string>
#include "MLSTags.h"


using namespace juce::dsp;

static class Compute
{
	//constructor
public:
	
	Compute();

	//descturctor
	~Compute();
	
	//IRSweep
	/**
	Function, that computes impulse response from passed input and output.
	P:
	@param inputSignalPointer - &AudioBuffer<float>, reference to input signal (linear or logarithmic sweep), it always has one channel
	@param recordedDataPointer - &AudioBuffer<float>, reference to output signal - recorded with mic, it has that many channels, as active inputs
	@param irDataPointer - &AudioBuffer<float>, reference to AudioBuffer, that should be filled with processed data
	*/
	static void IRSweep(AudioBuffer<float> &inputSignalPointer, AudioBuffer<float> &recordedDataPointer, AudioBuffer<float> &irDataPointer);
	//IRMLS
	/**
	Function, that yields impulse response from MLS signal.
	Signal is permutated by first permutation matrix, than hadamard transform is performed and than second permutation is performed.
	After dividing by scalar (n^Order) we get IR.
	P:
	@param  &irDataPointer - AudioBuffer<float>, reference to room impulse response to the MLS signal
	@param  &recordedDataPointer - AudioBuffer<float>, reference to output signal - recorded with mic, it has that many channels, as active inputs
	@param Order int, order of used input signal
	@return vector<float>, impulse response of length 2^Order - 1.
	*/
	static void IRMLS(AudioBuffer<float> &irDataPointer, AudioBuffer<float> &recordedDataPointer, int Order);
	//Lundeby's method
	/**
	Function, that focuses on reverberation time. 
	Impulse response is squared, plotted logarithmicly and "smoothed" - divided into sections of 20-50ms which are arevaged (locally integrated), first noise level is determined from the tail of IR, than the early decay
	is fitted with line, that gives us the reverberation time.
	P:
	@param irDataPointer - &AudioBuffer<float>, reference to already computed impulse response
	@param lundebyDataPointer - &AudioBuffer<float>, reference to AudioBuffer, that should be filled with processed data
	@param lundebyAdditionalData - &Array<LundebyReturn, reference to Array, that should be filled with processed data
	@param fs - int, sample rate
	
	LundebyReturn, struct of 6 fields
	DeayStartPoint- int, startpoint (index) of regression line computation.
	DecPar - DecayParameters, struct containing Slope and Intercept - parameters of fitted line y = a*x +b.
	NoiseLvl - float, noise level in dB.
	Crosspoint - int, crosspoint of decay regression and noise level in samples.
	T60 - float, reverberation time.
	IntegrInterval - int, final interval of local integrating
	*/
	static void LundebysMethod(AudioBuffer<float> &irDataPointer, AudioBuffer<float> &lundebyDataPointer, Array<LundebyReturn> &lundebyAdditionalData, int fs);
	//Schroeder integral
	/**
	Performs backward integration of input signal and from that computes reverberation time
	P:
	@param irDataPointer - &AudioBuffer<float>, reference to already computed impulse response
	@param schroederDataPointer - &AudioBuffer<float>, reference to AudioBuffer, that should be filled with processed data
	@param schroederAdditionalData - &Array<SchroederReturn, reference to Array, that should be filled with processed data
	@param fs int, sample rate
	
	SchroederReturn - struct that contains 2 fields
	DecPar - DecayParameters, struct containing Slope and Intercept - parameters of fitted line y = a*x +b.
	T60 - float, reverberation time
	*/
	static void SchroederInt(AudioBuffer<float> &irDataPointer, AudioBuffer<float> &schroederDataPointer, Array<SchroederReturn> &schroederAdditionalData, Array<LundebyReturn> &lundebyAdditionalData, const float fs);
	/**
	Filters out signals in Impulse response below start frequency of input signal. There are causing errors after deconvolution
	P:
	@param  &irDataPointer - AudioBuffer<float>, reference to room impulse response to the MLS signal
	@param fs - float, sample rate
	@param cutOffFrequency - float, start frequency of input signal
	*/
	static void FilterIR(AudioBuffer<float> &irDataPointer, const float fs, const float cutOffFrequency);
	/**
	Normalizes channels in buffer
	P:
	@param  &signalPointer - AudioBuffer<float>, reference to buffer
	*/
	static void NormalizeBuffer(AudioBuffer<float> &signalPointer);

private:
	//IRMLS
	
	/**
	Performs Hadamard transform of input signal
	P:
	@param input - vector<float>, signal to be permutated (should be n^2 length but accepts n^2-1 too)
	@return vector<float>, new vector of permutated input signal (n^2 length)
	*/
	static std::vector<float> HadamardTransform(ScopedPointer<std::vector<float>> input);
	/**
	Imports tags to vector from a binary file, location is fixed by now.
	P:
	@param tagNumber int, 1 or 2, there are 2 types of permutation matrices
	@param order int, order of mls signal
	@return vector<float>, vector of tag numbers
	*/
	static std::vector<int> importTags(int tagNumber, int order);
	/**
	Converts passed char[4] to int value. Serves for converting from hex to dec.
	P:
	@param chars char*, char[4] that contains one int number. Conversion from char is performed to hex and than properly to dec
	@return vector<int>, vector of tags numbers
	*/
	static int CharToInt(char* chars);


	//IRSweep
	/**
	Performs FFT on passed signal.
	P:
	@param &input vector<float>, signal to be FFT transformed
	@param nfft int, order of FFT - needs to be n^2.
	@return juce::dsp::Complex<float>, complex Fourier transformed input signal
	*/
	static Complex<float>* FFTfx(const float* recordedDataChannelReadPointer,  const int samples, const int nfft);
	/**
	Returns conjugate complex signal to input.
	P:
	@param &input juce::dsp::Complex<float>, input signal to b
	@param inpSize int, length of input signal
	@return juce::dsp::Complex<float>, conjugate to the input signal
	*/
	static Complex<float>* ConjugateArray(ScopedPointer< juce::dsp::Complex<float>> &input, const int inpSize);
	/**
	Performs complex multiplication
	P:
	@param &left juce::dsp::Complex<float>, left operand
	@param &right juce::dsp::Complex<float>, right operand
	@param inpSize int, length of input signal
	@return juce::dsp::Complex<float>
	*/
	static Complex<float>* CpMul(ScopedPointer< juce::dsp::Complex<float>> &left, ScopedPointer< juce::dsp::Complex<float>>& right, const int inpSize);
	/**
	Performs complex division, parametrs are passed by reference
	P:
	@param &nom juce::dsp::Complex<float>, nominator
	@param &denom juce::dsp::Complex<float>, denominator
	@param inpSize int, length of input signals (should be equal, not handled)
	@return juce::dsp::Complex<float>
	*/
	static Complex<float>* CpDivR(ScopedPointer< juce::dsp::Complex<float>> &nom, ScopedPointer< juce::dsp::Complex<float>>& denom, const int inpSize);
	/**
	Performs complex division, parametrs are passed by value
	P:
	@param nom juce::dsp::Complex<float>, nominator
	@param denom juce::dsp::Complex<float>, denominator
	@param inpSize int, length of input signals (should be equal, not handled)
	@return juce::dsp::Complex<float>
	*/
	static Complex<float>* CpDiv(Complex<float>* nom, Complex<float>* denom, const int inpSize);
	/**
	Performs complex division - dividing by real number, parametrs are passed by reference
	P:
	@param nom juce::dsp::Complex<float>, nominator
	@param denom float, denominator
	@param inpSize int, length of input signals (should be equal, not handled)
	@return juce::dsp::Complex<float>
	*/
	static Complex<float>* CpDiv(Complex<float>* nom, float denom, const int inpSize);
	/**
	Returns magnitude of passed complex signal.
	P:
	@param &inputfx juce::dsp::Complex<float>, complex signal
	@param inpSize int, length of input signals (should be equal, not handled)
	@return vector<float>, magnitude of complex input signal
	*/
	static void CpToMagnitude(ScopedPointer< juce::dsp::Complex<float>> &inputfx, float *irDataChannelWriter, const int inpSize);

	//Lundeby's method
	/**
	Returns mean of passed signal, between passed start and stop index
	P:
	@param &inputfx vector<float>, input signal
	@param StartIndex int, index from where mean will be calculated
	@param StopIndex int, index to where mean will be calculated
	@return float, value of mean
	*/
	static float Mean(std::vector<float> &inputfx, const int StartIndex, const int StopIndex);
	/**
	Returns index of first value in signal, that is smaller than given value.
	P:
	@param &inputfx vector<float>, input signal
	@param value float, value for comparison
	@param StartIndex int, [optional] def = 0
	@param StopIndex int, [optional] def = inputfx's length
	@return int, index of value in input vector, that is first smaller than passed value	
	*/
	static int FirstLessThan(std::vector<float> &inputfx, const float value, int StartIndex = 0, int StopIndex = -1);
	/**
	Returns index of first value in signal, that is smaller than given value. Based on regression line.
	P:
	@param Slope float, slope of regression line
	@param Intercept float, intercept of regression line
	@param value float, value
	@return int, index of value in input vector, that is first smaller than passed value
	*/
	static int FirstLessThan(float Slope, float Intercept, float value);
	/**
	Returns last index that contains value greater than passed
	P:
	@param &inputfx vector<float>, input signal
	@param value float, value
	@param StartIndex int, [optional] def = 0
	@param StopIndex int, [optional] def = inputfx's length
	@return int, index of value in input vector, that is last greater than passed value
	*/
	static int LastGreaterThan(std::vector<float> &inputfx, const float value, int StartIndex = 0, int StopIndex = -1);
	/**
	[OBSOLETE] - use MovingAverage
	Performs local integration of passed signal
	P:
	@param &inputfx vector<float>, input signal
	@param fs int, sample rate
	@param interval int, interval of integration - either in ms or in samples
	@param method bool, false - for passing it ms, true - for passing in number of samples
	@return vector<float> locally integrated input	
	*/
	static std::vector<float> LocalIntegration(std::vector<float> &inputfx, const int fs, int interval, bool method);
	/**
	Averages the given data with moving average filter.
	P:
	@param &inputfx vector<float>, input signal
	@param fs int, sample rate
	@param interval int, interval of average filter - either in ms or in samples - must be odd. If not, will be prolonged.
	@param method bool, false - for passing it ms, true - for passing in number of samples
	@return vector<float> locally integrated input
	*/
	static std::vector<float> MovingAverage(std::vector<float> &inputfx, const int fs, int interval, bool method);
	/**
	Returns squarred array of input.
	P:
	@param &inputfx vector<float>, intput signal
	@return vector<float>, squarred input	
	*/
	static std::vector<float> SquaredArr(const float * irDataChannelPointer, const int samples);
	/**
	Returns squarred array of input but input gets changed!
	P:
	@param &inputfx vector<float>, intput signal
	@return vector<float>, squarred input
	*/
	static std::vector<float> SquaredArrR(std::vector<float> &inputfx);
	/**
	Changes the value of input signal to dB and normalizes it to max value of input signal
	P:
	@param &inputfx vector<float>, input signal
	@return vector<float>, output
	*/
	static std::vector<float> Log10N(std::vector<float> &inputfx);
	/**
	Changes the value of input signal to dB and normalizes it to max value of input signal. Changes the input!
	P:
	@param &inputfx vector<float>, input signal
	@return vector<float>, output
	*/
	static std::vector<float> Log10NR(std::vector<float> &inputfx);
	/**
	Returns dynamic range of passed signal - max-min.
	P:
	@param &inputfx vector<float>, input signal
	@param StartIndex int, [optional] def = 0
	@param StopIndex int, [optional] def = inputfx's length
	@return float, dynamic range
	*/
	static float DynRange(std::vector<float> &inputfx, int StartIndex =0, int StopIndex =-1);
	/**
	Returns regression parametrs of line that fit best into passed data.
	P:
	@param &x vector<float>, x values of signal
	@param &y vector<float>, y values of signal
	@param StartIndex int, [optional] def = 0
	@param StopIndex int, [optional] def = inputfx's length
	@return DecayParameters - struct that contains 2 fields
	.Slope
	.Intercept
	*/
	static DecayParameters Compute::LinLSSolver(std::vector<float> & x, std::vector<float> & y, int StartIndex = 0, int StopIndex = -1);



	// Schroeder's integral
	/**
	Performs backward integration of input signal
	P:
	@param &inputfx vector<float>, input signal
	@param predelay int, predelay in samples - sample with max value
	@return vector<float>, output	
	*/
	static std::vector<float> ReversedSum(std::vector<float> &inputfx, int predelay);
};


