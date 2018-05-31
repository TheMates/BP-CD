/*
  ==============================================================================

    MvTabbedComponent.h
    Created: 30 Apr 2018 7:28:55pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvLookAndFeel.h"
#include "MvStructs.h"
//==============================================================================
/*
Class that extends a TabbedComponent with Listener.
*/
class MvTabbedComponent    : public TabbedComponent
{
public:
    /**
	Just like normal TabbedComponent, but has a callback method to check, if current tab has changed.
	P:
	@param orientation - TabbedButtonBar::Orientation, where tabs should be.
	*/
	MvTabbedComponent(TabbedButtonBar::Orientation orientation);
    ~MvTabbedComponent();

	void paint(Graphics& g) override;

	class Listener
	{
	public:
		/**
		Callback method when current tab changes.
		*/
		virtual void  currentTabChanged(int newCurrentTabIndex, const String & newCurrentTabName) = 0;
	};
	void addListener(Listener* listener) { listeners.add(listener); };
	void removeListener(Listener* listener) { listeners.remove(listener); };

	void currentTabChanged(int newCurrentTabIndex, const String & newCurrentTabName) override;

private:
	ListenerList<MvTabbedComponent::Listener> listeners;
	ScopedPointer<MvLookAndFeel> MvLaF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MvTabbedComponent)
};
