/*
  ==============================================================================

    MvLookAndFeel.h
    Created: 21 May 2018 2:23:21pm
    Author:  Matouš

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#define BackgroundGreyColour 0xffe8e6e7
#define OutlineBlueColor 0xff3a88bb//0xff97dfdf
#define LightBlueColor 0xffc9eaff
#define PopupMenuHighlightBlueColor 0xffa1cce6
#define ButtonOnBlueColor 0xff71a8cb

//==============================================================================
/*
*/
class MvLookAndFeel    : public LookAndFeel_V4
{
public:
    MvLookAndFeel()
		:LookAndFeel_V4(LookAndFeel_V4::getLightColourScheme())
    {
		//MvLookAndFeel->setColour(ResizableWindow::backgroundColourId, Colours::lightgrey);
		setColour(ResizableWindow::backgroundColourId, Colour(BackgroundGreyColour));

		setColour(TextEditor::backgroundColourId, Colour(LightBlueColor));
		setColour(TextEditor::outlineColourId, Colour(OutlineBlueColor));
		setColour(TextEditor::focusedOutlineColourId, Colour(OutlineBlueColor));
		setColour(TextEditor::highlightedTextColourId, Colours::black);
		setColour(TextEditor::textColourId, Colours::black);

		setColour(Label::textColourId, Colours::black);

		setColour(ComboBox::ColourIds::backgroundColourId, Colour(LightBlueColor));
		setColour(ComboBox::ColourIds::textColourId, Colours::black);
		setColour(ComboBox::ColourIds::arrowColourId, Colours::darkgrey);
		setColour(ComboBox::ColourIds::outlineColourId, Colour(OutlineBlueColor));

		setColour(ListBox::ColourIds::backgroundColourId, Colour(LightBlueColor));
		setColour(ListBox::ColourIds::outlineColourId, Colour(OutlineBlueColor));
		setColour(ListBox::ColourIds::textColourId, Colours::black);

		setColour(TextButton::ColourIds::buttonColourId, Colour(OutlineBlueColor));
		setColour(TextButton::ColourIds::textColourOffId, Colours::white);
		//setColour(TextButton::ColourIds::buttonOnColourId, Colour(ButtonOnBlueColor));
		//setColour(TextButton::ColourIds::textColourOnId, Colours::white);

		setColour(ToggleButton::ColourIds::tickColourId, Colours::darkgrey);

		setColour(PopupMenu::ColourIds::backgroundColourId, Colour(BackgroundGreyColour));
		setColour(PopupMenu::ColourIds::textColourId, Colours::black);
		setColour(PopupMenu::ColourIds::headerTextColourId, Colours::black);
		setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colour(PopupMenuHighlightBlueColor));
		setColour(PopupMenu::ColourIds::highlightedTextColourId, Colours::black);

		setColour(TabbedButtonBar::ColourIds::frontTextColourId, Colours::white);
		setColour(TabbedButtonBar::ColourIds::tabTextColourId, Colours::white);
		setColour(TabbedComponent::ColourIds::backgroundColourId, Colour(BackgroundGreyColour));

		setColour(Slider::ColourIds::backgroundColourId, Colour(0xffa9a9a9));
		setColour(Slider::ColourIds::thumbColourId, Colour(0xff656565));

    }

    ~MvLookAndFeel()
    {
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MvLookAndFeel)
};
