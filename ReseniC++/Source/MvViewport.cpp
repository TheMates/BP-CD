/*
  ==============================================================================

    MvViewport.cpp
    Created: 6 May 2018 2:40:55pm
    Author:  Matouš

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvViewport.h"

//==============================================================================
MvViewport::MvViewport()
	:Viewport()
{
}

MvViewport::~MvViewport()
{
}

void MvViewport::visibleAreaChanged(const Rectangle< int > &newVisibleArea)
{
	listeners.call(&Listener::visibleAreaChanged,newVisibleArea);
}
