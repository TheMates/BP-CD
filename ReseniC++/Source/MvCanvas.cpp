/*
  ==============================================================================

    MvCanvas.cpp
    Created: 4 May 2018 10:54:04am
    Author:  Matouš

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvCanvas.h"

//==============================================================================
#pragma region Public


MvCanvas::MvCanvas()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}
MvCanvas::~MvCanvas()
{
	
	audioBufferPointer.release();
	_channelSelectors.release();
	_noiseSelectors.release();
	_decay10Selectors.release();
	_decay20Selectors.release();
	_decay30Selectors.release();

}
//==============================================================================
void MvCanvas::paint (Graphics& g)
{
	drawGrid(g);	//draws the background
	drawGraphs(g);		//draws each channel
}
void MvCanvas::resized()
{

}
//==============================================================================

//==== Methods for drawing ====
void MvCanvas::drawGrid(Graphics& g)
{
	g.setColour(Colours::white);
	g.fillRect(getLocalBounds());
	if (audioBufferPointer && !_channelSelectors.get()->isEmpty())
	{
		int i = 0;
		do
		{
			plotHorizontalLine(g, firstVerticalValue - verticalStep * (float)i, false, -1, Colours::lightgrey);
			i++;
		} while (firstVerticalValue - verticalStep * (float)i > currentMinDisplayValue);
		i = 0;
		do
		{
			plotVerticalLine(g, firstHorizontalValue + horizontalSetp * (float)i, Colours::lightgrey);
			i++;
		} while (firstHorizontalValue + horizontalSetp * (float)i < currentEndDisplaySample);

	}

}
void MvCanvas::drawGraphs(Graphics& g)
{
	if (audioBufferPointer && !_channelSelectors.get()->isEmpty())		//for some reason this was hit before the recording finished, so had to add second condition
	{
		//find the first
		for (int channel = 0; channel < audioBufferPointer->getNumChannels(); ++channel)
		{
			if (_channelSelectors->operator[](channel)->get()->getToggleState())	//check if i want to draw this one
			{
				plotAudioBufferChannel(g, channel);	//draw the channel data
			}
			if (additionaLundebyDataPointer.get() != nullptr)		//check if i'm focused in lundeby tab
			{	//draw decays and noises if buttons are on
				if (_decay10Selectors->operator[](channel)->get()->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::DecayOn))
				{
					plotRegression(g, additionaLundebyDataPointer.get()->getUnchecked(channel).DecPar10, channel,1);
				}
				if (_decay20Selectors->operator[](channel)->get()->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::DecayOn))
				{
					plotRegression(g, additionaLundebyDataPointer.get()->getUnchecked(channel).DecPar20, channel,2);
				}
				if (_decay30Selectors->operator[](channel)->get()->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::DecayOn))
				{
					plotRegression(g, additionaLundebyDataPointer.get()->getUnchecked(channel).DecPar30, channel,3);
				}
				if (_noiseSelectors->operator[](channel)->get()->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::NoiseOn))
				{
					plotHorizontalLine(g, additionaLundebyDataPointer.get()->getUnchecked(channel).NoiseLvl,true, channel);
				}

			}
			if (additionaSchroederDataPointer.get() != nullptr)		//check if i'm focused in schroeder tab
			{
				if (_decay10Selectors->operator[](channel)->get()->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::DecayOn))
				{
					plotRegression(g, additionaSchroederDataPointer.get()->getUnchecked(channel).DecPar10, channel,1);
				}
				if (_decay20Selectors->operator[](channel)->get()->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::DecayOn))
				{
					plotRegression(g, additionaSchroederDataPointer.get()->getUnchecked(channel).DecPar20, channel,2);
				}
				if (_decay30Selectors->operator[](channel)->get()->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::DecayOn))
				{
					plotRegression(g, additionaSchroederDataPointer.get()->getUnchecked(channel).DecPar30, channel,3);
				}
			}
		}
	}
}
void MvCanvas::plotAudioBufferChannel(Graphics& g, int channelIndex)
{
	auto bufferReader = audioBufferPointer->getReadPointer(channelIndex);					//reader from buffer
	int samples = currentEndDisplaySample - currentStartDispaySample;										//number of samples
	
	float displaySamples = (float)currentViewWidth*5.0f;			//let's say i will only load 10 times more samples from the actual data, than screen resolution,
	float xInterval = (float)samples / (float)displaySamples;			//this is x interval i'll be skipping when reading from the data. It's ok that it's float, i have the interpolate method

	Range<float> displayRange{ currentMinDisplayValue,currentMaxDisplayValue };

	switch (channelIndex)
	{
	case 0: 	g.setColour(Colour(StrokeColours::ch1));
		break;
	case 1:	g.setColour(Colour(StrokeColours::ch2));
		break;
	case 2: 	g.setColour(Colour(StrokeColours::ch3));
		break;
	case 3: 	g.setColour(Colour(StrokeColours::ch4));
		break;
	case 4: 	g.setColour(Colour(StrokeColours::ch5));
		break;
	case 5: 	g.setColour(Colour(StrokeColours::ch6));
		break;
	case 6: 	g.setColour(Colour(StrokeColours::ch7));
		break;
	case 7: 	g.setColour(Colour(StrokeColours::ch8));
		break;
	case 8: 	g.setColour(Colour(StrokeColours::ch9));
		break;
	case 9: 	g.setColour(Colour(StrokeColours::ch10));
		break;
	case 10: 	g.setColour(Colour(StrokeColours::ch11));
		break;
	case 11: 	g.setColour(Colour(StrokeColours::ch12));
		break;
	case 12: 	g.setColour(Colour(StrokeColours::ch13));
		break;
	case 13: 	g.setColour(Colour(StrokeColours::ch14));
		break;
	case 14:	g.setColour(Colour(StrokeColours::ch15));
		break;
	case 15: 	g.setColour(Colour(StrokeColours::ch6));
		break;
	default:	g.setColour(Colours::black);
		break;
	}


	Path signalPath;
	signalPath.preallocateSpace(3 * samples);
	PathStrokeType pst(0.75f);

	Point<float> previousPoint;
	Point<float> nextPoint;



	if (!isnan(bufferReader[0]))		//oh sometimes, when signal is too low, it goes nan - soo.. let's take care of that
	{
		nextPoint = { (float)currentStartDispaySample, bufferReader[0] };
		signalPath.startNewSubPath(nextPoint);
		previousPoint = nextPoint;
	}
	else
	{
		nextPoint = { (float)currentStartDispaySample, 0.0 };
		signalPath.startNewSubPath(nextPoint);
		previousPoint = nextPoint;
	}
	for (int i = 1; i < displaySamples; ++i)
	{
		if (1 + i * xInterval > samples)		//i think it should never happen, that i*xInterval is greater than samples, but to be sure...
		{
			if (!isnan(bufferReader[(int)(currentStartDispaySample + i * xInterval)]))
			{
				signalPath.lineTo(currentStartDispaySample +i * xInterval, bufferReader[samples - 1]);
			}
			else
			{
				signalPath.lineTo(currentStartDispaySample+ i * xInterval, 0.0);
			}
		}
		else
		{
			if (!isnan(bufferReader[(int)(currentStartDispaySample + i * xInterval)]))
			{
				nextPoint = { currentStartDispaySample + i * xInterval, interpolate(bufferReader[(int)(currentStartDispaySample + i * xInterval)], bufferReader[(int)(currentStartDispaySample + i * xInterval) + 1], xInterval) };
				if (nextPoint.getY() == previousPoint.getY() && bufferReader[(int)(currentStartDispaySample + i * xInterval)] != bufferReader[(int)(currentStartDispaySample + i * xInterval) + 1])
				{
					continue;
				}
				signalPath.lineTo(nextPoint);
				previousPoint = nextPoint;
			}
			else
			{
				nextPoint = { currentStartDispaySample + i * xInterval, 0.0 };
				signalPath.lineTo(nextPoint);
				previousPoint = nextPoint;
			}
		}
	}


	//it renders.. sometimes slightly more to the right, dont know why, when the windows is small, its ok, but when its large, its worse
	g.strokePath(signalPath, pst, AffineTransform::fromTargetPoints(0.0, globalDisplayRange.getEnd(), 0, 0,
		0.0, globalDisplayRange.getStart(), 0, getHeight(),
		(float)audioBufferPointer->getNumSamples(), globalDisplayRange.getEnd(), getWidth(), 0));


	bufferReader = nullptr;
}
void MvCanvas::plotHorizontalLine(Graphics& g, float yValue, bool isNoiseLevel , int channelIndex, Colour colour)
{
	float yPosition = getHeight()* (globalDisplayRange.getEnd() - yValue) / globalDisplayRange.getLength();
	if (!isnan(yPosition))
	{
		Line<float> noise{ 0.0f ,yPosition ,(float)getWidth(), yPosition };

		if (colour != Colours::black)
		{
			g.setColour(colour);
		}
		float lineThickness;
		float dl[]{ 7,7 };
		if (isNoiseLevel)
		{
			lineThickness = 1.5;
			switch (channelIndex)
			{
			case 0: 	g.setColour(Colour(StrokeColours::ch1));
				break;
			case 1:	g.setColour(Colour(StrokeColours::ch2));
				break;
			case 2: 	g.setColour(Colour(StrokeColours::ch3));
				break;
			case 3: 	g.setColour(Colour(StrokeColours::ch4));
				break;
			case 4: 	g.setColour(Colour(StrokeColours::ch5));
				break;
			case 5: 	g.setColour(Colour(StrokeColours::ch6));
				break;
			case 6: 	g.setColour(Colour(StrokeColours::ch7));
				break;
			case 7: 	g.setColour(Colour(StrokeColours::ch8));
				break;
			case 8: 	g.setColour(Colour(StrokeColours::ch9));
				break;
			case 9: 	g.setColour(Colour(StrokeColours::ch10));
				break;
			case 10: 	g.setColour(Colour(StrokeColours::ch11));
				break;
			case 11: 	g.setColour(Colour(StrokeColours::ch12));
				break;
			case 12: 	g.setColour(Colour(StrokeColours::ch13));
				break;
			case 13: 	g.setColour(Colour(StrokeColours::ch14));
				break;
			case 14:	g.setColour(Colour(StrokeColours::ch15));
				break;
			case 15: 	g.setColour(Colour(StrokeColours::ch6));
				break;
			default:	g.setColour(Colours::black);
				break;
			}
			g.drawDashedLine(noise, dl, 2, lineThickness);
		}
		else
		{
			lineThickness = 1.0;
			g.drawLine(noise, lineThickness);
		}
	}
}
void MvCanvas::plotVerticalLine(Graphics& g, float xValue, Colour colour)
{
	if (colour != Colours::black)
	{
		g.setColour(colour);
	}
	Line<float> line{ (getWidth()*xValue*(_currentSampleRate / 1000.0f)) / (float)audioBufferPointer.get()->getNumSamples() ,0 ,(getWidth()*xValue*(_currentSampleRate / 1000.0f)) / (float)audioBufferPointer.get()->getNumSamples(),(float)getHeight() };
	g.drawLine(line, 1.0);
}
void MvCanvas::plotRegression(Graphics& g,DecayParameters parameters, int channelIndex, int regressionIndex)
{
	if (isnan(parameters.Intercept) || isnan(parameters.Slope))
	{
		return;
	}

	Path signalPath;

	switch (channelIndex)
	{
	case 0: 	g.setColour(Colour(StrokeColours::ch1));
		break;
	case 1:	g.setColour(Colour(StrokeColours::ch2));
		break;
	case 2: 	g.setColour(Colour(StrokeColours::ch3));
		break;
	case 3: 	g.setColour(Colour(StrokeColours::ch4));
		break;
	case 4: 	g.setColour(Colour(StrokeColours::ch5));
		break;
	case 5: 	g.setColour(Colour(StrokeColours::ch6));
		break;
	case 6: 	g.setColour(Colour(StrokeColours::ch7));
		break;
	case 7: 	g.setColour(Colour(StrokeColours::ch8));
		break;
	case 8: 	g.setColour(Colour(StrokeColours::ch9));
		break;
	case 9: 	g.setColour(Colour(StrokeColours::ch10));
		break;
	case 10: 	g.setColour(Colour(StrokeColours::ch11));
		break;
	case 11: 	g.setColour(Colour(StrokeColours::ch12));
		break;
	case 12: 	g.setColour(Colour(StrokeColours::ch13));
		break;
	case 13: 	g.setColour(Colour(StrokeColours::ch14));
		break;
	case 14:	g.setColour(Colour(StrokeColours::ch15));
		break;
	case 15: 	g.setColour(Colour(StrokeColours::ch6));
		break;
	default:	g.setColour(Colours::black);
		break;
	}
	float x1, x2;
	x1 = (globalDisplayRange.getEnd() - parameters.Intercept) / parameters.Slope;
	x2 = (globalDisplayRange.getStart() - parameters.Intercept) / parameters.Slope;

	Line<float> decay{ (getWidth()*x1) / (float)audioBufferPointer.get()->getNumSamples() ,0 ,(getWidth()*x2) / (float)audioBufferPointer.get()->getNumSamples(),(float)getHeight() };

	ScopedPointer< float >dl;
	switch (regressionIndex)
	{
	case 1: dl = new float[4]{ 7, 5, 2, 5 };
			g.drawDashedLine(decay, dl, 4, 1.5);
		break;
	case 2: dl = new float[6] { 7, 5, 2, 5, 2, 5 };
			g.drawDashedLine(decay, dl, 6, 1.5);
			break;
	case 3: dl = new float[8] { 7, 5, 2, 5, 2, 5, 2, 5};
			g.drawDashedLine(decay, dl, 8, 1.5);
			break;
	default:
		break;
	}


}

//==== Methods for managing pointers ====
void MvCanvas::setAudioBufferPointer(ScopedPointer<AudioBuffer<float>> &audioBufferToReadFrom)
{
	audioBufferPointer.release();
	audioBufferPointer = audioBufferToReadFrom.get();
}
void MvCanvas::setAdditionalDataPointer(ScopedPointer<Array<LundebyReturn>> &additionaLundebyData)
{
	if (!additionaLundebyData.get()->isEmpty())
	{
		additionaLundebyDataPointer.release();
		additionaSchroederDataPointer.release();
		additionaLundebyDataPointer = OptionalScopedPointer<Array<LundebyReturn>>{ additionaLundebyData.get() ,false };;
	}
}
void MvCanvas::setAdditionalDataPointer(ScopedPointer<Array<SchroederReturn>>  &additionaSchroederData)
{
	if (!additionaSchroederData.get()->isEmpty())
	{
		additionaLundebyDataPointer.release();
		additionaSchroederDataPointer.release();
		additionaSchroederDataPointer = OptionalScopedPointer<Array<SchroederReturn>>{ additionaSchroederData.get() ,false };
	}
}
void MvCanvas::releaseAditionalData()
{
	additionaLundebyDataPointer.release();
	additionaSchroederDataPointer.release();
}
void MvCanvas::setChannelSelectorsPointer(OwnedArray<ScopedPointer<ToggleButton>> &channelSelectorsToReferTo, 
	OwnedArray<ScopedPointer<TextButton>> &noiseSelectorsToReferTo, OwnedArray<ScopedPointer<TextButton>> &decay10SelectorsToReferTo,
	OwnedArray<ScopedPointer<TextButton>> &decay20SelectorsToReferTo, OwnedArray<ScopedPointer<TextButton>> &decay30SelectorsToReferTo)
{
	_channelSelectors.release();
	_noiseSelectors.release();
	_decay10Selectors.release();
	_decay20Selectors.release();
	_decay30Selectors.release();

	_channelSelectors = &channelSelectorsToReferTo;
	_noiseSelectors = &noiseSelectorsToReferTo;
	_decay10Selectors = &decay10SelectorsToReferTo;
	_decay20Selectors = &decay20SelectorsToReferTo;
	_decay30Selectors = &decay30SelectorsToReferTo;
}

//==== Setting other parameters ====
void MvCanvas::setCurrentSampleRate(float currentSampleRate)
{
	_currentSampleRate = currentSampleRate;
}
GridParameters MvCanvas::setGridParameters()
{
	verticalStep = getGridStep(false);
	horizontalSetp = getGridStep(true);
	firstVerticalValue = getFirstGridValue(verticalStep);
	firstHorizontalValue = getFirstGridValue(horizontalSetp, true);
	GridParameters result;
	result.firstHorizontalValuePx = (getWidth()*firstHorizontalValue*(_currentSampleRate/1000.0)) / (float)audioBufferPointer.get()->getNumSamples();
	result.firstVerticalValuePx = getHeight()* (globalDisplayRange.getEnd() - firstVerticalValue) / globalDisplayRange.getLength();
	result.horizontalSetpPx = ((getWidth()*(_currentSampleRate/1000.0)*(firstHorizontalValue + horizontalSetp) )/ (float)audioBufferPointer.get()->getNumSamples()) - result.firstHorizontalValuePx;
	result.verticalStepPx = (getHeight()* (globalDisplayRange.getEnd() - (firstVerticalValue - verticalStep)) / globalDisplayRange.getLength()) - result.firstVerticalValuePx;;
	return result;
}
void MvCanvas::setDisplayBoundaries(int startDisplaySample, int endDisplaySample, int maxValuePosition, int minValuePosition, int viewedWidth, int viewedHeight)
{
	setGlobalRange();
	currentStartDispaySample = startDisplaySample;
	currentEndDisplaySample = endDisplaySample;
	
	float valpx = ((globalDisplayRange.getEnd() - globalDisplayRange.getStart())) / getHeight();	//"how much" is one pixel
	
	currentMaxDisplayValue = globalDisplayRange.getEnd() - (float)maxValuePosition*valpx;
	currentMinDisplayValue = globalDisplayRange.getEnd() - (float)minValuePosition*valpx;
	currentViewWidth = viewedWidth;
	currentViewHeight = viewedHeight;
}

#pragma endregion


#pragma region Private
void MvCanvas::setGlobalRange()
{
		Range<float> globalRange = audioBufferPointer->findMinMax(0, 0, audioBufferPointer->getNumSamples());	//will be used to set the display value bonds
		Range<float> channelRange;
		//iterate through the rest
		for (int i = 1; i < audioBufferPointer->getNumChannels(); ++i)
		{
			channelRange = audioBufferPointer->findMinMax(i, 0, audioBufferPointer->getNumSamples());
			if (channelRange.getStart() < globalRange.getStart())
				globalRange.setStart(channelRange.getStart());
			if (channelRange.getEnd() > globalRange.getEnd())
				globalRange.setEnd(channelRange.getEnd());
		}
		globalDisplayRange.setStart(globalRange.getStart() - (globalRange.getEnd() -globalRange.getStart())*0.05);
		globalDisplayRange.setEnd(globalRange.getEnd() + (globalRange.getEnd() - globalRange.getStart())*0.05);
}
int MvCanvas::getNumberOrder(const float x)
{
	if (x < 1)
	{
		int n = 1;
		while (pow(10, n)*x<1)	//this is true only when number is 
		{
			++n;
		}
		return n;
	}
	else
	{
		int n = -1;
		while (pow(10, n)*x>1)	//this is true only when number is 
		{
			--n;
		}
		return n + 1;
	}
}
float MvCanvas::getFirstGridValue(float step,bool isHorizontal)
{
	if (step != 0.0f)
	{
		float temp;
		if (isHorizontal)
		{
			temp = currentStartDispaySample/ (_currentSampleRate/1000.0) - step;
		}
		else
		{
			temp = currentMaxDisplayValue - step;
		}
		temp /= step;
		temp = (int)temp;
		if (temp == 0.0f)
		{
			if (isHorizontal)
			{
				return 0.0f;
			}
			else
			{
				return step;
			}

		}
		temp *= step;
		if(isHorizontal)
		{
			return temp;
		}
		else
		{
		return temp + step;
		}
	}
	return 0.0f;
}
float MvCanvas::getGridStep(bool isHorizotal)
{
	float gamma, x;
	if (isHorizotal)
	{
		gamma = (float)currentViewWidth / 40.0;	//let's say i want numbers/lines to be max 40 from each other
		x = (currentEndDisplaySample/(_currentSampleRate/1000.0)  - currentStartDispaySample/ (_currentSampleRate/1000.0)) / gamma;
	}
	else
	{
		gamma = (float)currentViewHeight / 40.0;	//let's say i want numbers/lines to be max 40 from each other
		x = (currentMaxDisplayValue - currentMinDisplayValue) / gamma;
	}
	//i want steps by 1,2,5 and their multiples
	int order = getNumberOrder(x);		
	if (x*powf(10, (float)order) < 2)	//now i can compare with my steps
	{
		return powf(10, -(float)order)*2.0;
	}
	else if (x*powf(10, (float)order) < 5)
	{
		return powf(10, -(float)order)*5.0;
	}
	else
	{
		return powf(10, -(float)order) * 10;
	}
}
float MvCanvas::interpolate(float sample1, float sample2, float position)
{
	if (!isnan(sample1) && !isnan(sample2))
	{
		float delta = sample2 - sample1;
		float pos = position - (float)((int)position);	//leaving only decimals
		return sample1 + pos * delta;
	}
	else
	{
		return 0.0;
	}
}

#pragma endregion
