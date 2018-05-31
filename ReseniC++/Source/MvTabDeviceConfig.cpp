#include "MvTabDeviceConfig.h"
//==============================================================================
/*
*/
	MvTabDeviceConfig::MvTabDeviceConfig(AudioDeviceSelectorComponent *selectComp)
	{
		myAudioSelector = selectComp;
		addAndMakeVisible(selectComp);
		setSize(700, 500);
	}

	MvTabDeviceConfig::~MvTabDeviceConfig()
	{
	}

	void MvTabDeviceConfig::paint(Graphics& g) 
	{
		g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	}

	void MvTabDeviceConfig::resized()
	{
		myAudioSelector->setBounds(getLocalBounds().reduced(4));
	}
