/*
  ==============================================================================

    MvTable.h
    Created: 14 May 2018 2:03:46pm
    Author:  Matouš

  ==============================================================================
*/
#define nOfCells  8
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvLookAndFeel.h"
#include "MvStructs.h"

//==============================================================================
/*
*/
class MvTable    : public Component
{
public:
    MvTable();
    ~MvTable();

    void paint (Graphics&) override;
    void resized() override;
	/**
	Updates references to data to read from
	P:
	@param &additionalLundebyData - Array<LundebyReturn>, data to read from
	@param &additionalSchroederData - Array<SchroederReturn>, data to read from
	*/
	void updateDataReferences(Array<LundebyReturn> &additionalLundebyData, Array<SchroederReturn> &additionalSchroederData);
	/**
	Updates current sample rate
	P:
	@pram currentSampleRate - float, current sample rate.
	*/
	void updateSampleRate(float currentSampleRate);

private:
	OptionalScopedPointer<Array<SchroederReturn>> _schroederAdditionalDataPointer;
	OptionalScopedPointer<Array<LundebyReturn>> _lundebyAdditionalDataPointer;

	ScopedPointer<TextEditor> channelText, t10SchText, t20SchText, t30SchText, t10LunText, t20LunText, t30LunText, predelayText;
	OwnedArray<ScopedPointer<TextEditor>> data;
	float _currentSampleRate = 0.0;

	//==== Constructor related methods ====
	void initalizeAllMyComponents();
	void addAndMakeVisibleAllMyComponents();

	//==== Other methods ====	
	void updateData();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MvTable)
};
