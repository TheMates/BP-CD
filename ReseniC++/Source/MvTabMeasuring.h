/*
  ==============================================================================

    MvTabMeasuring.h
    Created: 15 Apr 2018 8:46:38pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvLookAndFeel.h"
#include "MvSignalPlot.h"
#include "MvTabbedComponent.h"
#include "MvTable.h"
#include "MvButtons.h"

//==============================================================================

class MvTabMeasuring : public Component , public TextEditor::Listener, public ComboBox::Listener, public Button::Listener, public MvTabbedComponent::Listener,
	public Slider::Listener
{
public:
	MvTabMeasuring();
	~MvTabMeasuring();

	void paint(Graphics& g) override;
	void resized() override;

#pragma region GetSet
	//====Getters for parent====
	/**
	Returns currently set .
	P:
	@returns - float, amplitude
	*/
	float getAmplitude();
	/**
	Returns currently set start frequency.
	P:
	@returns - start frequency
	*/
	float getStartFrequency();
	/**
	Returns currently set stop frequency.
	P:
	@returns - float, stop frequency
	*/
	float getStopFrequency();
	/**
	Returns currently set length in samples.
	P:
	@returns - length in samples.
	*/
	float getLengthTime();
	/**
	Returns current mls order.
	P:
	@returns - int, current mls order
	*/
	int getMLSOrder();
	/**
	Returns currently selected type of signal.
	P:
	@returns - int, type of signal. 1 = log sweep, 2 = lin sweep, 3 = mls
	*/
	int getTypeOfSignal();
	/**
	Returns current sample rate
	P:
	@returns - current sample rate
	*/
	float getCurrentSampleRate();
	/**
	Sets current sample rate to new value.
	P:
	@param currentSampleRate - float, new sample rate.
	*/
	void updateCurrentSampleRate(float currentSampleRate);
	/**
	Returns number of measurement repetitions.
	P:
	@returns - int, number of measurement repetitions
	*/
	int getNOfRepetitions();
#pragma endregion

	//==== Other methods ====
	/**
	Updates references to data.
	P:
	@param &recordedData - AudioBuffer<float>
	@param &irData - AudioBuffer<float>
	@param &lundebyData - AudioBuffer<float>
	@param &schroederData - AudioBuffer<float>
	@param &additionalLundebyData - Array<LundebyReturn>
	@param &additionalSchroederData - rray<SchroederReturn>
	*/
	void updateMainComponentReferences(AudioBuffer<float> &recordedData, AudioBuffer<float> &irData,
		AudioBuffer<float> &lundebyData, AudioBuffer<float> &schroederData, Array<LundebyReturn> &additionalLundebyData, Array<SchroederReturn> &additionalSchroederData);
	/**
	Sends pointers to data, that should be drawn, to plot and result table.
	*/
	void drawData();

	class Listener
	{
	public:
		virtual void buttonTriggered(int buttonId) = 0;
	};
	void addListener(Listener* listener);
	void removeListener(Listener* listener);


private:
		//====My Components====
	ScopedPointer<TextEditor> amplitudeText, startFrequencyText, stopFrequencyText, lengthTimeText, mlsOrderText, nOfRepetitionsText;
	ScopedPointer<Label> amplitudeLabel, startFrequencyLabel, stopFrequencyLabel, lengthTimeLabel, mlsOrderLabel, signalTypeLabel, nOfRepetitionsLabel, dBFSUnit, startHzUnit, stopHzUnit, msUnit;
	ScopedPointer<ComboBox> signalTypeCombo;
	ScopedPointer<ImageButton> startMeasuringButton, testPlayInput, testStopRecording, testClearRecordedData;
	Rectangle<float> divider{};
	ScopedPointer<MvTable> resultTable;
	ScopedPointer<Slider> mlsPlusMinus, repetitionsPlusMinus;
	//==== Signal plotter ====
	ScopedPointer<MvTabbedComponent> _datasetChooser;
	MvSignalPlot _plot{};

	//==== My variables ====
	ListenerList<MvTabMeasuring::Listener> listeners;
	ScopedPointer<AudioBuffer<float>> _recordedDataPointer,_irDataPointer, _lundebyDataPointer,_schroederDataPointer;
	ScopedPointer<Array<SchroederReturn>> _schroederAdditionalDataPointer;
	ScopedPointer<Array<LundebyReturn>> _lundebyAdditionalDataPointer;
	//====Actual parameters of the signal====
	float _amplitude, _startFrequency, _stopFrequency, _lengthTime, _length, _currentSampleRate = 0.0;
	int _mlsOrder, _nOfRepetitions;
	int _typeOfSignal;		//type of signal: 1=log sweep, 2= lin sweep, 3=mls


	//==== Callbacks ====
	/**
	Callback for text editor - changed.
	P:
	@param &currentTextEditor - TextEditor.
	*/
	void textEditorTextChanged(TextEditor &currentTextEditor) override;
	/**
	Callback for text editor - lost focus.
	P:
	@param &currentTextEditor - TextEditor.
	*/
	void textEditorFocusLost(TextEditor &currentTextEditor) override;
	/**
	Callback method of button.
	P:
	@param currentButton - *Button, button that has been clicked.
	*/
	void buttonClicked(Button *currentButton);
	/**
	Callback method of combo box.
	P:
	@param comboBox - *ComboBox, combo box, that has changed.
	*/
	void ComboBox::Listener::comboBoxChanged(ComboBox * comboBox);
	/**
	Callback method of slider.
	P:
	@param slider - *Slider, slider, that has changed.
	*/
	void sliderValueChanged(Slider * slider) override;
	/**
	Callback method for MvTabbedComponent.
	P:
	@param newCurrentTabIndex - int, id of new current tab.
	@param &newCurrentTabName - String, name of new current tab.
	*/
	void currentTabChanged(int newCurrentTabIndex, const String & newCurrentTabName) override;

	//==== Constructor related methods ====
	/**
	Initializes all local components.
	*/
	void initializeAllMyComponents();
	/**
	Makes visible all components in this component.
	*/
	void addAndMakeVisibleAllMyComponents();
	/**
	Sets bounds to signal setup menu - text boxes, labels and combo boxes.
	*/
	void drawSignalSetupMenu();
	/**
	Sets bounds to main 4 buttons.
	*/
	void drawButtons();
	/**
	Updates visibility of text boxes in signal setup menu.
	*/
	void updateSignalSetupMenu();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MvTabMeasuring)
};
