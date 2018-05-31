/*
  ==============================================================================

    MvTabbedComponent.cpp
    Created: 30 Apr 2018 7:28:55pm
    Author:  Matouš

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvTabbedComponent.h"

//==============================================================================
MvTabbedComponent::MvTabbedComponent(TabbedButtonBar::Orientation orientation)
	:TabbedComponent(orientation)
{
	MvLaF = new MvLookAndFeel();
	setLookAndFeel(MvLaF);
}

MvTabbedComponent::~MvTabbedComponent()
{
	setLookAndFeel(nullptr);
}
void MvTabbedComponent::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MvTabbedComponent::currentTabChanged(int newCurrentTabIndex, const String & newCurrentTabName)
{
	listeners.call(&Listener::currentTabChanged, newCurrentTabIndex, newCurrentTabName);
}
