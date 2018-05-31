/*
  ==============================================================================

    MvTabCalibration.h
    Created: 15 Apr 2018 8:46:25pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "MvMicrophoneIndicator.h"
#include "MvButtons.h"

//==============================================================================
/*
*/
class MvTabCalibration : public Component, public Button::Listener, public Timer, public Slider::Listener , public MvMicrophoneIndicator::Listener//, public ChangeListener
{
public:
	MvTabCalibration(AudioDeviceManager *deviceManager);
	~MvTabCalibration();

	void paint(Graphics& g) override;
	void resized() override;
	
	//==== My public methods ====
	/**
	Sets bounds to microphone indicators with respect to their number.
	*/
	void updateIndicatorBounds();
	/**
	Sets calibration slider value to 0.0.
	*/
	void resetCalibrationSlider();
	/**
	Sets current value to desired channel. It's passed to microphone indicator, that will display it
	P:
	@param value - float, value of max abs value in one buffer interval.
	@param channel - int, channel of buffer, corresponds to microphone indicator
	*/
	void setCurrentInputValue(float value, int channel);
	/**
	Returns current global calibration level.
	P:
	@param channel - int, index of channel (0 based).
	@returns - float, current global calibration level
	*/
	float getCurrentCalibrationConstant(int channel);
	/**
	Takes care of right indicators number, with respect to active device inputs.
	Here microphone indicators are added or deleted.
	*/
	void updateIndicators();
	/**
	Returns vector of active input channels. Size is equal to number of active channels and each
	cell carries the id of channel (1 based). Used by channel indicators.
	P:
	@param mask - BigInteger, mask, that is returned from AudioIODevice->getActiveInputChannels().
	@returns - vector<int>, vector of active inputs.
	*/
	static std::vector<int> maskToActiveChannels(juce::BigInteger mask);
	/**
	Returns array of active input channels. Size is equal to number of active channels and each
	cell carries the id of channel (0 based). Used my main component.
	P:
	@param mask - BigInteger, mask, that is returned from AudioIODevice->getActiveInputChannels().
	@returns - Array<int>, array of active inputs.
	*/
	static Array<int> maskToActiveChannelsAr(juce::BigInteger mask);
	/**
	Deletes all microphone indicators
	*/
	void deleteIndicators();
	//==== Callbacks ====
	/**
	Sets input values to microphone indicators at 30 Hz rate.
	*/
	void timerCallback();
	/**
		Callback method, when the slider changes value, it set's it to microphone indiacators as current calibration level.
		P:
		@param slider - *Slider, slider, who's value has changed.
		*/
	void sliderValueChanged(Slider* slider) override;
	/**
	Callback method of button.
	P:
	@param button - *Button, button that got clicked.
	*/
	void buttonClicked(Button* button);
	/**
	Callback method of MvMicrophoneIndicator. It notifies parent (this component) that their calibration level has
	changed and it gets set as global calibration level
	P:
	@param indicator - *MvMicrophoneIndicator, microphone indicator, who asks for value.
	*/
	void MvMicrophoneIndicator::Listener::globalCalibrationLevelChanged(float vewValue);
	/**
	Callback method of MvMicrophoneIndicator, which is (are) a child(ren) of this component. It asks for global calibration level so it can set it as theirs.
	P:
	@param indicator - *MvMicrophoneIndicator, microphone indicator, who asks for value.
	*/
	void MvMicrophoneIndicator::Listener::giveMeGlobalCalibrationLevel(MvMicrophoneIndicator *indicator);


	class Listener
	{
	public:
		/**
		Callback method that notifies parent, that button here has been triggerd. 
		P:
		@param buttonId - int, id of button. 1 - start measuring, 2 - stop palying, 3 - play input signal, 4 - delete recorded data
		*/
		virtual void buttonTriggered(int buttonId) = 0;
	};

	void addListener(Listener* listener);
	void removeListener(Listener* listener);


private:
	//==== My components ====
	ScopedPointer<ImageButton> testLvlButton;
	ScopedPointer<Slider> calibrationSlider;

	//==== My variables ====
	AudioIODevice * device;
	AudioDeviceManager * myDeviceManager;
	ListenerList<MvTabCalibration::Listener> listeners;
	std::vector<ScopedPointer<MvMicrophoneIndicator>> _myIndicators;		//tady budou všechny indikátory který se zobrazí
	std::vector<int> _activeInputs;										//indexy (kanály) jednotlivejch indikátorů
	std::vector<float> _currentInputValues;								//tady budou aktuální hodnoty RMS z každýho kanálu, který se budou kreslit
	float _globalCalibrationLevel;
	std::vector<int[2]> fadeColours{ 16 };
	
	//==== My methods ====
	/**
	Sets calibration slider visibility as visible if there are some indicators. When there is none, it disappeares.
	*/
	void updateCalibrationSliderVisibility();
	/**
	Fills the array of fill colours. I'm using this, so i can get the colour by interating number.
	*/
	void fillFadeColours();


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MvTabCalibration)
};
