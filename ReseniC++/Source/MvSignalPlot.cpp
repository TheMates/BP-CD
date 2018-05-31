/*
  ==============================================================================

    MvSignalPlot.cpp
    Created: 30 Apr 2018 12:16:08pm
    Author:  Matouš

  ==============================================================================
*/
#define _USE_MATH_DEFINES

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvSignalPlot.h"

using namespace std;

//==============================================================================
#pragma region Public

MvSignalPlot::MvSignalPlot()
{
	canvas = new MvCanvas();
	addAndMakeVisible(canvas);
	viewPort = new MvViewport();
	viewPort->setViewedComponent(canvas.get());
	viewPort->setScrollOnDragEnabled(true);	
	viewPort->addListener(this);
	viewPort->getHorizontalScrollBar().addListener(this);
	viewPort->getVerticalScrollBar().addListener(this);
	addAndMakeVisible(viewPort);

	xZoomSlider = new Slider(Slider::SliderStyle::LinearHorizontal,Slider::TextEntryBoxPosition::NoTextBox);
	yZoomSlider = new Slider(Slider::SliderStyle::LinearVertical, Slider::TextEntryBoxPosition::NoTextBox);
	xZoomSlider->setRange(1.0, 100.0);
	yZoomSlider->setRange(1.0, 10.0);
	xZoomSlider->addListener(this);
	yZoomSlider->addListener(this);
	addAndMakeVisible(xZoomSlider);
	addAndMakeVisible(yZoomSlider);

}
MvSignalPlot::~MvSignalPlot()
{
	canvas.release();		//toto dělá potíže? proč? proč to tam musí bejt?
	audioBufferPointer.release();
	additionaLundebyDataPointer.release();
	additionaSchroederDataPointer.release();
	
	_decay10Selectors.clear(true);
	_decay20Selectors.clear(true);
	_decay30Selectors.clear(true);
	_noiseSelectors.clear(true);
	_channelSelectors.clear(true);
	_channelSelectorsLabels.clear(true);
}
void MvSignalPlot::paint(Graphics& g)
{
	//g.fillAll(Colour(0xfff0f0f0));   // clear the background
	g.fillAll(Colours::white);   // clear the background

	g.setColour(Colours::grey);
	g.drawRect(viewPort->getX() - 2, 0, viewPort->getWidth() + 4, viewPort->getBottom() + 2, 2);   // draw an outline around the component
	for (int i = 0; i < _channelSelectors.size(); ++i)
	{
		switch (i)
		{
		case 0: 	g.setColour(Colour(ToggleButtonColours::tb1));
			break;
		case 1:	g.setColour(Colour(ToggleButtonColours::tb2));
			break;
		case 2: 	g.setColour(Colour(ToggleButtonColours::tb3));
			break;
		case 3: 	g.setColour(Colour(ToggleButtonColours::tb4));
			break;
		case 4: 	g.setColour(Colour(ToggleButtonColours::tb5));
			break;
		case 5: 	g.setColour(Colour(ToggleButtonColours::tb6));
			break;
		case 6: 	g.setColour(Colour(ToggleButtonColours::tb7));
			break;
		case 7: 	g.setColour(Colour(ToggleButtonColours::tb8));
			break;
		case 8: 	g.setColour(Colour(ToggleButtonColours::tb9));
			break;
		case 9: 	g.setColour(Colour(ToggleButtonColours::tb10));
			break;
		case 10: 	g.setColour(Colour(ToggleButtonColours::tb11));
			break;
		case 11: 	g.setColour(Colour(ToggleButtonColours::tb12));
			break;
		case 12: 	g.setColour(Colour(ToggleButtonColours::tb13));
			break;
		case 13: 	g.setColour(Colour(ToggleButtonColours::tb14));
			break;
		case 14:	g.setColour(Colour(ToggleButtonColours::tb15));
			break;
		case 15: 	g.setColour(Colour(ToggleButtonColours::tb16));
			break;
		default:	g.setColour(Colours::black);
			break;
		}
		g.fillRoundedRectangle(_channelSelectors[i]->get()->getX() + 4, _channelSelectors[i]->get()->getY() + 4, 16.0f, 16.0f, 2.0f);
	}
	//draw axis labels
	g.setColour(Colours::black);
	g.setFont(Font(16.0).withTypefaceStyle("Italic"));
	g.drawSingleLineText("t", viewPort->getWidth() / 2 + viewPort->getX(), viewPort->getBottom() + 25);
	g.setFont(Font(16.0).withTypefaceStyle("Regular"));
	g.drawSingleLineText("[ms]", viewPort->getWidth() / 2 + viewPort->getX() + 8, viewPort->getBottom() + 25);

	g.addTransform(AffineTransform::rotation(-M_PI_2));
	g.setFont(Font(16.0).withTypefaceStyle("Italic"));
	switch (currentTab)
	{
	case SelectedPlotTab::IR:
		g.drawSingleLineText("h  t", -(viewPort->getHeight() / 2)-10, 15);
		break;
	case SelectedPlotTab::Lundeby:	//same
	case SelectedPlotTab::Schoeder:
		g.drawSingleLineText("env  t", -(viewPort->getHeight() / 2)-20, 15);
		break;
	case SelectedPlotTab::TF:
		g.drawSingleLineText("s  t", -(viewPort->getHeight() / 2)-10, 15);
		break;
	default:
		break;
	}
	if (currentTab == SelectedPlotTab::Lundeby || currentTab == SelectedPlotTab::Schoeder)
	{
		g.setFont(Font(16.0).withTypefaceStyle("Regular"));
		g.drawSingleLineText("  ( ) [dB]", -(viewPort->getHeight() / 2)-2, 15);
		g.addTransform(AffineTransform::rotation(M_PI_2));
	}
	else if(currentTab == SelectedPlotTab::TF)
	{
		g.setFont(Font(16.0).withTypefaceStyle("Regular"));
		g.drawSingleLineText("( )", -(viewPort->getHeight() / 2)+1, 15);
		g.addTransform(AffineTransform::rotation(M_PI_2));
	}
	else
	{
		g.setFont(Font(16.0).withTypefaceStyle("Regular"));
		g.drawSingleLineText("( )", -(viewPort->getHeight() / 2) + 2, 15);
		g.addTransform(AffineTransform::rotation(M_PI_2));
	}
	
	//draw axis numbers
	if (!_channelSelectors.isEmpty())
	{
		GridParameters parameters = canvas->setGridParameters();
		int firstVerticalValuePx = parameters.firstVerticalValuePx - viewPort->getViewPositionY();
		int firstHorizontalValuePx = parameters.firstHorizontalValuePx - viewPort->getViewPositionX();
		float i = 0.0f;
		g.setColour(Colours::black);
		g.setFont(12.0f);
		do
		{
			g.drawSingleLineText(String::formatted("%.5g",canvas->firstVerticalValue - i*canvas->verticalStep), viewPort->getX()-3 , 6+(firstVerticalValuePx + i * parameters.verticalStepPx),Justification::right);
			++i;
		} while (firstVerticalValuePx + i * parameters.verticalStepPx < viewPort->getHeight());
		i = 0.0f;
		do
		{
			if (firstHorizontalValuePx + i * parameters.horizontalSetpPx >= viewPort->getX()-62)	//some offseting , there is a safe offset for the display to render, so some numbers actually dont need to gt drawn
			{
				g.drawSingleLineText(String(canvas->firstHorizontalValue + i * canvas->horizontalSetp), viewPort->getX() + (firstHorizontalValuePx + i * parameters.horizontalSetpPx), viewPort->getBottom() + 12, Justification::horizontallyCentred);
			}
			++i;
		} while (firstHorizontalValuePx + i * parameters.horizontalSetpPx < viewPort->getWidth());
		

	}


}
void MvSignalPlot::resized()
{
	viewPort->setBounds(62, 2, getWidth() - 164, getHeight() - 45);
	canvas->setBounds(0, 0, xZoom * viewPort->getWidth(), yZoom * viewPort->getHeight());

	xZoomSlider->setBounds(viewPort->getX()-8, getHeight() - 15,  viewPort->getWidth() +8, 20);
	yZoomSlider->setBounds(viewPort->getRight(), viewPort->getY() -4, 20, viewPort->getHeight()+4);
	updateCanvasDisplayBoundaries();


	for (int i = 0; i < _channelSelectors.size(); ++i) 
	{
		_channelSelectors[i]->get()->setBounds(getWidth() - 45, 10 + i * 40, 24, 24);
		_channelSelectorsLabels[i]->get()->setBounds(getWidth() - 53, 27 + i * 40, 50, 30);
	}
	if (additionaLundebyDataPointer != nullptr && !additionaLundebyDataPointer.get()->isEmpty())	//first check lundeby, that has both, noise and decay
	{
		for (int i = 0; i < _noiseSelectors.size(); ++i)
		{
			_decay10Selectors[i]->get()->setBounds(_channelSelectors[i]->get()->getX() - 10 - 2*14, _channelSelectors[i]->get()->getY() + 4, 13, 16);
			_decay20Selectors[i]->get()->setBounds(_channelSelectors[i]->get()->getX() - 10 - 14, _channelSelectors[i]->get()->getY() + 4, 13, 16);
			_decay30Selectors[i]->get()->setBounds(_channelSelectors[i]->get()->getX() - 10, _channelSelectors[i]->get()->getY() + 4, 13, 16);
			_noiseSelectors[i]->get()->setBounds(_channelSelectors[i]->get()->getX() +21.5, _channelSelectors[i]->get()->getY() + 4, 13, 16);
		}
	}
	if (additionaSchroederDataPointer != nullptr && !additionaSchroederDataPointer.get()->isEmpty())	//then check schroeder, which only has decay
	{
		for (int i = 0; i < _noiseSelectors.size(); ++i)
		{
			_decay10Selectors[i]->get()->setBounds(_channelSelectors[i]->get()->getX() - 10 - 2 * 14, _channelSelectors[i]->get()->getY() + 4, 13, 16);
			_decay20Selectors[i]->get()->setBounds(_channelSelectors[i]->get()->getX() - 10 - 14, _channelSelectors[i]->get()->getY() + 4, 13, 16);
			_decay30Selectors[i]->get()->setBounds(_channelSelectors[i]->get()->getX() - 10, _channelSelectors[i]->get()->getY() + 4, 13, 16);
		}
	}
}

//==== Methods for managing pointers ====
void MvSignalPlot::addToGraph(AudioBuffer<float> &audioBufferToReadFrom)
{
	audioBufferPointer.release();
	audioBufferPointer = &audioBufferToReadFrom;
	updateCanvasPointers();
	updateChannelInfo();
	repaint();		//jumps into MvCanvas paint method
}
void MvSignalPlot::addToGraphAdditional(Array<LundebyReturn> &additionalDataToReadFrom)
{
	additionaSchroederDataPointer.release();	//i clear both of them so only one is full
	additionaLundebyDataPointer.release();
	additionaLundebyDataPointer = &additionalDataToReadFrom;
	canvas->setAdditionalDataPointer(additionaLundebyDataPointer);
}
void MvSignalPlot::addToGraphAdditional(Array<SchroederReturn> &additionalDataToReadFrom)
{
	additionaLundebyDataPointer.release();		//i clear both of them so only one is full
	additionaSchroederDataPointer.release();
	additionaSchroederDataPointer = &additionalDataToReadFrom;
	canvas->setAdditionalDataPointer(additionaSchroederDataPointer);
}
void MvSignalPlot::releaseAdditional()
{
	additionaLundebyDataPointer.release();		//i clear both of them so only one is full
	additionaSchroederDataPointer.release();
	canvas->releaseAditionalData();
}

//==== Other setters ====
void MvSignalPlot::setCurrentSampleRate(float currentSampleRate)
{
	canvas->setCurrentSampleRate(currentSampleRate);
}
void MvSignalPlot::setCurrentTab(int tabIndex)
{
	currentTab = tabIndex;
}

//==== Callbacks ====
void MvSignalPlot::buttonClicked(Button * button)
{
	if (button->getComponentID().operator[](0) == 'd')	//decay button, id starts with d
	{
		if (button->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::Off))
		{
			button->setColour(TextButton::buttonColourId, Colour(NoiseDecayButtonColour::DecayOn));
		}
		else
		{
			button->setColour(TextButton::buttonColourId, Colour(NoiseDecayButtonColour::Off));
		}
	}
	
	if (button->getComponentID().operator[](0) == 'n')	//noise button, id starts with n
	{
		if (button->findColour(TextButton::buttonColourId) == Colour(NoiseDecayButtonColour::Off))
		{
			button->setColour(TextButton::buttonColourId, Colour(NoiseDecayButtonColour::NoiseOn));
		}
		else
		{
			button->setColour(TextButton::buttonColourId, Colour(NoiseDecayButtonColour::Off));
		}
	}

	repaint();	//jumps into MvCanvas paint method
}
void MvSignalPlot::sliderValueChanged(Slider *slider)
{
	double previousHorizontalPosition = viewPort->getHorizontalScrollBar().getCurrentRangeStart();					//thumb position before scaling
	double previousHorizontalRangeEnd = viewPort->getHorizontalScrollBar().getMaximumRangeLimit();					//thumb range before scaling
	double horizontalPositionRaito = (double)previousHorizontalPosition / (double)previousHorizontalRangeEnd;		//relative position

	double previousVerticalPosition = viewPort->getVerticalScrollBar().getCurrentRangeStart();						//thumb position before scaling
	double previousVerticalRangeEnd = viewPort->getVerticalScrollBar().getMaximumRangeLimit();						//thumb range before scaling
	double verticalPositionRaito = (double)previousVerticalPosition / (double)previousVerticalRangeEnd;				//relative position

	if (slider == xZoomSlider)
	{
		xZoom = xZoomSlider->getValue();
		canvas->setBounds(0, 0, xZoom * viewPort->getWidth(), yZoom * viewPort->getHeight());
		double nextHorizontalRangeEnd = viewPort->getHorizontalScrollBar().getMaximumRangeLimit();			//thumb range after scaling
		double nextHorizontalPosition = nextHorizontalRangeEnd * horizontalPositionRaito;					//sets the relative position equal to preivous
		viewPort->getHorizontalScrollBar().setCurrentRangeStart(nextHorizontalPosition);
		viewPort->getVerticalScrollBar().setCurrentRangeStart(previousVerticalPosition);
	}
	else
	{
		yZoom = yZoomSlider->getValue();
		canvas->setBounds(0, 0, xZoom * viewPort->getWidth(), yZoom * viewPort->getHeight());
		double nextVerticalRangeEnd = viewPort->getVerticalScrollBar().getMaximumRangeLimit();			//thumb range after scaling
		double nextVerticalPosition = nextVerticalRangeEnd * verticalPositionRaito;					//sets the relative position equal to preivous
		viewPort->getVerticalScrollBar().setCurrentRangeStart(nextVerticalPosition);
		viewPort->getHorizontalScrollBar().setCurrentRangeStart(previousHorizontalPosition);
	}

	updateCanvasDisplayBoundaries();
	repaint();
}
void MvSignalPlot::scrollBarMoved(ScrollBar * scrollBarThatHasMoved, double 	newRangeStart)
{
	updateCanvasDisplayBoundaries();
	repaint();
}
void MvSignalPlot::visibleAreaChanged(const Rectangle< int > &newVisibleArea)
{
	updateCanvasDisplayBoundaries();
	repaint();
}

#pragma endregion


#pragma region Private

//==== Other methods ====
void MvSignalPlot::updateChannelInfo()
{
	
	_channelSelectors.clear(true);
	_channelSelectorsLabels.clear(true);
	_noiseSelectors.clear(true);
	_decay10Selectors.clear(true);
	_decay20Selectors.clear(true);
	_decay30Selectors.clear(true);

	for (int i = 0; i < audioBufferPointer->getNumChannels(); ++i)
	{
		_channelSelectors.add(new ScopedPointer<ToggleButton>{new ToggleButton{}});
		_channelSelectors[i]->get()->setToggleState(true, dontSendNotification);
		_channelSelectors[i]->get()->setComponentID(String::formatted("Ch %d", i + 1));	//dont actually need it right now, but might be handy
		_channelSelectors[i]->get()->addListener(this);
		_channelSelectors[i]->get()->setTriggeredOnMouseDown(true);
		_channelSelectors[i]->get()->setColour(ToggleButton::tickColourId, Colours::white);

		_channelSelectorsLabels.add(new ScopedPointer<Label>{new Label{ "",String::formatted("Ch %d",i + 1) } });
		_channelSelectorsLabels[i]->get()->setFont(Font(Font::getDefaultMonospacedFontName(), 12.00f, Font::plain));
		_channelSelectorsLabels[i]->get()->setColour(Label::textColourId, Colours::black);

		addAndMakeVisible(_channelSelectors[i]->get());
		addAndMakeVisible(_channelSelectorsLabels[i]->get());

		_noiseSelectors.add(new ScopedPointer<TextButton>{ new TextButton{} });
		_noiseSelectors[i]->get()->setButtonText("---");		//like noise level
		_noiseSelectors[i]->get()->setComponentID(String::formatted("n%d", i + 1));
		_noiseSelectors[i]->get()->setColour(TextButton::ColourIds::buttonColourId, Colour(NoiseDecayButtonColour::Off));
		_noiseSelectors[i]->get()->setColour(TextButton::textColourOnId, Colours::white);
		_noiseSelectors[i]->get()->setConnectedEdges(Button::ConnectedOnLeft);
		_noiseSelectors[i]->get()->addListener(this);
		_noiseSelectors[i]->get()->setTriggeredOnMouseDown(true);
		_noiseSelectors[i]->get()->setColour(TextButton::textColourOnId, Colours::white);
		_noiseSelectors[i]->get()->setColour(TextButton::textColourOffId, Colours::white);

		_decay10Selectors.add(new ScopedPointer<TextButton>{ new TextButton{} });
		_decay10Selectors[i]->get()->setButtonText("\\");		//like decay slope
		_decay10Selectors[i]->get()->setComponentID(String::formatted("d1%d", i + 1));
		_decay10Selectors[i]->get()->setColour(TextButton::ColourIds::buttonColourId, Colour(NoiseDecayButtonColour::Off));
		_decay10Selectors[i]->get()->setColour(TextButton::textColourOnId, Colours::white);
		_decay10Selectors[i]->get()->setConnectedEdges(Button::ConnectedOnRight);
		_decay10Selectors[i]->get()->addListener(this);
		_decay10Selectors[i]->get()->setTriggeredOnMouseDown(true);
		_decay10Selectors[i]->get()->setColour(TextButton::textColourOnId, Colours::white);
		_decay10Selectors[i]->get()->setColour(TextButton::textColourOffId, Colours::white);


		_decay20Selectors.add(new ScopedPointer<TextButton>{ new TextButton{} });
		_decay20Selectors[i]->get()->setButtonText("\\");		//like decay slope
		_decay20Selectors[i]->get()->setComponentID(String::formatted("d2%d", i + 1));
		_decay20Selectors[i]->get()->setColour(TextButton::ColourIds::buttonColourId, Colour(NoiseDecayButtonColour::Off));
		_decay20Selectors[i]->get()->setColour(TextButton::textColourOnId, Colours::white);
		_decay20Selectors[i]->get()->setConnectedEdges(Button::ConnectedOnRight| Button::ConnectedOnLeft);
		_decay20Selectors[i]->get()->addListener(this);
		_decay20Selectors[i]->get()->setTriggeredOnMouseDown(true);
		_decay20Selectors[i]->get()->setColour(TextButton::textColourOnId, Colours::white);
		_decay20Selectors[i]->get()->setColour(TextButton::textColourOffId, Colours::white);

		_decay30Selectors.add(new ScopedPointer<TextButton>{ new TextButton{} });
		_decay30Selectors[i]->get()->setButtonText("\\");		//like decay slope
		_decay30Selectors[i]->get()->setComponentID(String::formatted("d3%d", i + 1));
		_decay30Selectors[i]->get()->setColour(TextButton::ColourIds::buttonColourId, Colour(NoiseDecayButtonColour::Off));
		_decay30Selectors[i]->get()->setColour(TextButton::textColourOnId, Colours::white);
		_decay30Selectors[i]->get()->setConnectedEdges(Button::ConnectedOnRight | Button::ConnectedOnLeft);
		_decay30Selectors[i]->get()->addListener(this);
		_decay30Selectors[i]->get()->setTriggeredOnMouseDown(true);
		_decay30Selectors[i]->get()->setColour(TextButton::textColourOnId, Colours::white);
		_decay30Selectors[i]->get()->setColour(TextButton::textColourOffId, Colours::white);

	};

	if (additionaLundebyDataPointer != nullptr && !additionaLundebyDataPointer.get()->isEmpty())	//first check lundeby, that has both, noise and decay
	{
		for (int i = 0; i < _noiseSelectors.size(); ++i)
		{
			addAndMakeVisible(_noiseSelectors[i]->get());
			addAndMakeVisible(_decay10Selectors[i]->get());
			addAndMakeVisible(_decay20Selectors[i]->get());
			addAndMakeVisible(_decay30Selectors[i]->get());
		}
	}
	if (additionaSchroederDataPointer != nullptr && !additionaSchroederDataPointer.get()->isEmpty())	//then check schroeder, which only has decay
	{
		for (int i = 0; i < _noiseSelectors.size(); ++i)
		{
			addAndMakeVisible(_decay10Selectors[i]->get());
			addAndMakeVisible(_decay20Selectors[i]->get());
			addAndMakeVisible(_decay30Selectors[i]->get());
		}
	}
	resized();
}	
void MvSignalPlot::updateCanvasPointers()
{
	canvas->setAudioBufferPointer(audioBufferPointer);
	canvas->setChannelSelectorsPointer(_channelSelectors,_noiseSelectors,_decay10Selectors, _decay20Selectors, _decay30Selectors);
}
void MvSignalPlot::updateCanvasDisplayBoundaries()
{
	if (audioBufferPointer != NULL && audioBufferPointer->getNumSamples()>0)
	{
		int startSample = (int)(((float)viewPort->getViewPositionX() / (float)canvas->getWidth())*(float)audioBufferPointer->getNumSamples());
		int endSample = (int)((((float)viewPort->getViewPositionX() + viewPort->getViewWidth()) / (float)canvas->getWidth())*(float)audioBufferPointer->getNumSamples());

		int safeRenderOffset = (endSample - startSample)*0.3;
		if (startSample - safeRenderOffset < 0)
		{
			startSample = 0;
		}
		else
		{
			startSample -= safeRenderOffset;
		}
		if (endSample + safeRenderOffset > audioBufferPointer->getNumSamples())
		{
			endSample = audioBufferPointer->getNumSamples() - 1;
		}
		else
		{
			endSample += safeRenderOffset;
		}
		int maxValuePosition = viewPort->getViewPositionY();
		int minValuePosition = viewPort->getViewPositionY() + viewPort->getViewHeight();

		canvas->setDisplayBoundaries(startSample, endSample, maxValuePosition, minValuePosition, viewPort->getViewWidth(), viewPort->getViewHeight());

	}
}

#pragma endregion
