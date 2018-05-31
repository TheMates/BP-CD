/*
  ==============================================================================

    MvMicrophoneIndicator.h
    Created: 27 Apr 2018 9:37:35pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class MvMicrophoneIndicator    : public Component , public TextButton::Listener
{
public:
    MvMicrophoneIndicator(String name, int deviceChannelIndex)
    {
		_nameOfInput = name;	//když je to delší než width tak se to nevleze, vyřešit...
		_MvMicrophoneIndicatorChannelIndex = deviceChannelIndex;		
		
		calibrateMeButton = new TextButton();
		setCalibByMeButton = new TextButton();
		deleteCalibrationConstantButton = new TextButton();

		calibrateMeButton->setColour(TextButton::buttonColourId, Colours::aliceblue);
		calibrateMeButton->setColour(TextButton::textColourOnId, Colours::black);
		calibrateMeButton->setColour(TextButton::textColourOffId, Colours::black);

		setCalibByMeButton->setColour(TextButton::buttonColourId, Colours::aliceblue);
		setCalibByMeButton->setColour(TextButton::textColourOnId, Colours::black);
		setCalibByMeButton->setColour(TextButton::textColourOffId, Colours::black);

		deleteCalibrationConstantButton->setColour(TextButton::buttonColourId, Colours::aliceblue);
		deleteCalibrationConstantButton->setColour(TextButton::textColourOnId, Colours::black);
		deleteCalibrationConstantButton->setColour(TextButton::textColourOffId, Colours::black);

		outerBorder = new Rectangle<float>{};
		indicatorBackground = new Rectangle<float>{};
		indicatorHider = new Rectangle<float>{};
		calibrationIndicatorRectangle = new Rectangle<float>{};

		calibrateMeButton->setButtonText(TRANS("M"));
		setCalibByMeButton->setButtonText(TRANS("B"));
		deleteCalibrationConstantButton->setButtonText(TRANS("d"));

		calibrateMeButton->addListener(this);
		setCalibByMeButton->addListener(this);
		deleteCalibrationConstantButton->addListener(this);


		addAndMakeVisible(calibrateMeButton);
		addAndMakeVisible(setCalibByMeButton);
		addAndMakeVisible(deleteCalibrationConstantButton);
    }

    ~MvMicrophoneIndicator()
    {
		//calibrateMeButton = nullptr;
		//setCalibByMeButton = nullptr;
    }

    void paint (Graphics& g) override
    {

		//Outer border
		//Colour fillColour1 = Colour(0xfffb7b7b), fillColour2 = Colour(0xffff0000);	//red so far, will be added dynamicaly, by enum or whatever...
		Colour strokeColour = Colours::black;
		g.setGradientFill(ColourGradient(topFade, 50, 0, bottomFade, 50, getHeight(),false));
		g.fillRoundedRectangle(*outerBorder, 4.0f);
		g.setColour(strokeColour);
		g.drawRoundedRectangle(*outerBorder, 4.0f, 1.0f);
		
		
		//Inner filling
		Colour fillColour = Colour(0xff464646);
		g.setColour(fillColour);
		g.fillPath(innerPath);

		//Indicator background
		Colour fillColour1 = Colour(0xffff0000);	//red
		Colour fillColour2 = Colour(0xff007414);	//green
		g.setGradientFill(ColourGradient(fillColour1, 50, 25, fillColour2, 50, getHeight()*0.4f, false));
		g.fillRect(*indicatorBackground);

		//Indicator hider - shows the actual value
		
		float levelRange = indicatorBackground->getHeight();
		float indicationValue = (levelRange * -(_currentCalibrationConstant + _currentValue)) / 61.1;	//level range is number of pixels, that MvMicrophoneIndicator1 can have max,co this way i 
		if (indicationValue > levelRange)	//so i dont draw over the bottom
		{
			indicationValue = levelRange;
		}
		
		indicatorHider->setBounds(indicatorBackground->getX(), indicatorBackground->getY(), indicatorBackground->getWidth(), indicationValue);
		g.setColour(Colour(0xff464646));
		g.fillRect(*indicatorHider);


		//Numbers
		float numRange = indicatorBackground->getHeight()-10;	//i need 10px for the text box
		float numY = indicatorBackground->getY();
		fillColour = Colours::lightgrey;
		g.setColour(fillColour);

		g.setFont(Font(13.00f, Font::plain).withTypefaceStyle("Regular"));
		g.drawText("-0-", 15, numY - 6, 34, 10, Justification::centred);
		g.drawText("-6-", 15, numY - 6 + numRange * 0.1, 34, 10, Justification::centred);
		g.drawText("-12-", 15, numY - 6 + numRange * 0.2, 34, 10, Justification::centred);
		g.drawText("-18-", 15, numY - 6 + numRange * 0.3, 34, 10, Justification::centred);
		g.drawText("-24-", 15, numY - 6 + numRange * 0.4, 34, 10, Justification::centred);
		g.drawText("-30-", 15, numY - 6 + numRange * 0.5, 34, 10, Justification::centred);
		g.drawText("-36-", 15, numY - 6 + numRange * 0.6, 34, 10, Justification::centred);
		g.drawText("-42-", 15, numY - 6 + numRange * 0.7, 34, 10, Justification::centred);
		g.drawText("-48-", 15, numY - 6 + numRange * 0.8, 34, 10, Justification::centred);
		g.drawText("-54-", 15, numY - 6 + numRange * 0.9, 34, 10, Justification::centred);
		g.drawText("-60-", 15, numY - 6 + numRange, 34, 10, Justification::centred);

		//calibration rectangle
		g.setColour(Colour(0xa900ffee));
		g.drawRect(*calibrationIndicatorRectangle, 2.0f);
		//Id of input
		g.setColour(Colours::black);
		g.setFont(Font(14.00f, Font::plain).withTypefaceStyle("Regular"));
		g.drawText(_nameOfInput, 0, getHeight() - 35, 100, 35, Justification::centred);

    }

    void resized() override
    {		
		outerBorder->setBounds(1, 1, getWidth()-2, getHeight()-2);
		indicatorBackground->setBounds( 17,33,30,(float)getHeight() - 70 );

		innerPath.clear();
		innerPath.startNewSubPath(15, 25);
		innerPath.lineTo(75, 25);
		innerPath.quadraticTo(85, 25, 85, 35);
		innerPath.lineTo(85, 65);
		innerPath.quadraticTo(85, 75, 75, 75);
		innerPath.lineTo(60, 75);
		innerPath.quadraticTo(50, 75, 50, 85);
		innerPath.lineTo(50, getHeight() - 138);
		innerPath.quadraticTo(50, getHeight() - 128, 60, getHeight() - 128);
		innerPath.lineTo(75, getHeight() - 128);
		innerPath.quadraticTo(85, getHeight() - 128, 85, getHeight() - 118);
		innerPath.lineTo(85, getHeight() - 45);
		innerPath.quadraticTo(85, getHeight() - 35, 75, getHeight() - 35);
		innerPath.lineTo(15, getHeight() - 35);
		innerPath.closeSubPath();
				
		calibrateMeButton->setBounds(50, getHeight() - 80, 30, 40);
		setCalibByMeButton->setBounds(50, 30, 30, 40);
		deleteCalibrationConstantButton->setBounds(50, getHeight() - 123, 30, 40);
		calibrationIndicatorRectangle->setBounds(11, indicatorBackground->getY()-3 + (-_currentCalibrationLevel/61.1f) *indicatorBackground->getHeight(), 43, 5);
		
    }

	void TextButton::Listener::buttonClicked(Button* button) 
	{
		if (button == calibrateMeButton)
		{
			listeners.call(&Listener::giveMeGlobalCalibrationLevel, this);	//i'm notifying parent (tabCalibration), that i wanna set current calib level to the global value
			//this callback in parent will call setCurrentCalibrationLevel

			_currentCalibrationConstant = _currentCalibrationLevel - _currentValue;
			deleteCalibrationConstantButton->setColour(TextButton::buttonColourId, Colour(0xffff4949));
		}
		if(button == setCalibByMeButton) //setCalibByMeButton
		{
			_currentCalibrationLevel = _currentValue;
			_currentCalibrationConstant = 0.0;
			deleteCalibrationConstantButton->setColour(TextButton::buttonColourId, Colours::aliceblue);
			
			listeners.call(&Listener::globalCalibrationLevelChanged, _currentCalibrationLevel);	//i'm notifying parents, that i wanna set the global calibration level to this new value
		}
		if (button == deleteCalibrationConstantButton)
		{
			_currentCalibrationConstant = 0.0;
			deleteCalibrationConstantButton->setColour(TextButton::buttonColourId, Colours::aliceblue);
		}
	}

	void MvMicrophoneIndicator::updateSignalLevel(float value)
	{
		if (value+_currentCalibrationConstant > _currentValue+_currentCalibrationConstant - 1.5)
		{
			_currentValue = value;
		}
		else
		{
			_currentValue -= 1.5;
		}
	}

#pragma region GetSet
	int getChannelIndex()
	{
		return _MvMicrophoneIndicatorChannelIndex;
	}
	void setChannelIndex(int index)
	{
		_MvMicrophoneIndicatorChannelIndex = index;
	}
	Rectangle<int> getRepaintArea()
	{
		return Rectangle<int>(indicatorBackground->getX(),indicatorBackground->getY(),indicatorBackground->getWidth(),indicatorBackground->getHeight());
	}
	float getCurrentCalibrationLevel()
	{
		return _currentCalibrationLevel;
	}
	void setCurrentCalibrationLevel(float value)
	{
		_currentCalibrationLevel = value;
		calibrationIndicatorRectangle->setBounds(11, indicatorBackground->getY() - 3 + (-_currentCalibrationLevel / 61.1f) *indicatorBackground->getHeight(), 43, 5);
		this->repaint();	
	}
	float getCurrentCalibrationConstant()
	{
		return _currentCalibrationConstant;
	}
	void setChannelColours(int top , int bottom)
	{
		topFade = Colour(top);
		bottomFade = Colour(bottom);
	}

#pragma endregion

	class Listener
	{
	public:
		virtual void globalCalibrationLevelChanged(float newValue) = 0;
		virtual void giveMeGlobalCalibrationLevel(MvMicrophoneIndicator *indicator) = 0;
	};

	void addListener(Listener* listener) { listeners.add(listener); };
	void removeListener(Listener* listener) { listeners.remove(listener); };


private:
	ScopedPointer< Rectangle<float>> outerBorder, indicatorBackground,indicatorHider,calibrationIndicatorRectangle;
	ScopedPointer<TextButton> calibrateMeButton, setCalibByMeButton, deleteCalibrationConstantButton;
	Path innerPath;
	String _nameOfInput;

	Colour bottomFade, topFade;

	ListenerList<MvMicrophoneIndicator::Listener> listeners;

	int nOfMvMicrophoneIndicators = 1;
	int _indicatorDisplayIndex = 0;
	int _MvMicrophoneIndicatorChannelIndex = 0;

	float _currentValue =0;
	float _currentCalibrationLevel = 0.0f;
	float _currentCalibrationConstant = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MvMicrophoneIndicator)
};
