/*
  ==============================================================================

    MvTabDeviceConfig.h
    Created: 15 Apr 2018 8:18:26pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "MvStructs.h"
//==============================================================================
class MvTabDeviceConfig    : public Component
{
public:
	MvTabDeviceConfig(AudioDeviceSelectorComponent *selectComp);
	~MvTabDeviceConfig();

	void paint(Graphics& g) override;

	void resized() override;
	//====Variables====

	AudioDeviceSelectorComponent * myAudioSelector;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MvTabDeviceConfig)

private:
	
};
