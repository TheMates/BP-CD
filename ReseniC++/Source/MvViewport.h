/*
  ==============================================================================

    MvViewport.h
    Created: 6 May 2018 2:40:55pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/*
	Viewport class extended with listener
*/
class MvViewport    : public Viewport
{
public:
    MvViewport();
    ~MvViewport();

	class Listener
	{
	public:
		/**
		Callback method to notify listener about new visible area.
		*/
		virtual void  visibleAreaChanged(const Rectangle< int > &newVisibleArea) = 0;
	};
	void addListener(Listener* listener) { listeners.add(listener); };
	void removeListener(Listener* listener) { listeners.remove(listener); };

	void visibleAreaChanged(const Rectangle< int > &newVisibleArea) override;

private:
	ListenerList<MvViewport::Listener> listeners;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MvViewport)
};
