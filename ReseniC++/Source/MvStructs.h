/*
  ==============================================================================

    DecayParameters.h
    Created: 10 Apr 2018 8:43:47pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once

struct DecayParameters
{
	float Slope = 0.0;
	float Intercept = 0.0;
};

struct LundebyReturn 
{
	//std::vector<float> Envelope;		//Envelope of locally integrated IR ind dB.		//now i write the data to reference, so i dont need this
	int DecayStartpoint = 0;			//Startpoint of decay regression line computation.
	DecayParameters DecPar10;				//Decay parameters of regression line - y = k*x +y, decay.slope = k, decay.intercept = q.
	DecayParameters DecPar20;				//Decay parameters of regression line - y = k*x +y, decay.slope = k, decay.intercept = q.
	DecayParameters DecPar30;				//Decay parameters of regression line - y = k*x +y, decay.slope = k, decay.intercept = q.
	float NoiseLvl = 0.0;				//noise level in dB.
	int Predelay = 0;					//Index of sample with highest amplitude.
	int Corsspoint = 0;					//crosspoint of decay regression and noise level.
	float T10 = 0.0;					//T10 - computed from first 10 dB of decay.
	float T20 = 0.0;					//T20 - computed from first 20 dB of decay.	
	float T30 = 0.0;					//T30 - computed from first 30 dB of decay.
	float T60 = 0.0;					//Reverberation time.
	int IntegrInterval = 0;				//New interval of local integrating
};

struct SchroederReturn
{
	//std::vector<float> Envelope;		//Envelope of locally integrated IR ind dB.		//now i write it to the reference
	DecayParameters DecPar10;				//Decay parameters of regression line - y = k*x +y, decay.slope = k, decay.intercept = q.
	DecayParameters DecPar20;				//Decay parameters of regression line - y = k*x +y, decay.slope = k, decay.intercept = q.
	DecayParameters DecPar30;				//Decay parameters of regression line - y = k*x +y, decay.slope = k, decay.intercept = q.
	float T10 = 0.0;					//T10 - computed from first 10 dB of decay.
	float T20 = 0.0;					//T20 - computed from first 20 dB of decay.	
	float T30 = 0.0;					//T30 - computed from first 30 dB of decay.
	float T60 = 0.0;					//Reverberation time.

};
struct GridParameters
{
	float verticalStepPx; 
	float horizontalSetpPx; 
	float firstVerticalValuePx; 
	float firstHorizontalValuePx;
};
