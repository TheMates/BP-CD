/*
  ==============================================================================

    MvTable.cpp
    Created: 14 May 2018 2:03:46pm
    Author:  Matouš

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MvTable.h"

//==============================================================================

#pragma region Public

MvTable::MvTable()
{
	initalizeAllMyComponents();
	addAndMakeVisibleAllMyComponents();
	setSize(95+4, 22 * nOfCells + 8);	//with some extra borders
}
MvTable::~MvTable()
{
	data.clear();
	_schroederAdditionalDataPointer.release();
	_lundebyAdditionalDataPointer.release();
}
void MvTable::paint (Graphics& g)
{
    g.fillAll (Colour(0xffb1b1b1));   // clear the background

}
void MvTable::resized()
{
	int columns = 0;
	if (!data.isEmpty())
	{
		columns = data.size() / nOfCells;
	}
	setSize(95 + columns*82 +4, getHeight());
	int cellWidth = 80, cellHeight = 22;
	
	channelText->setBounds(2, 2, cellWidth + 15, cellHeight);
	t10LunText->setBounds(2, cellHeight + 4, cellWidth + 15, cellHeight);
	t20LunText->setBounds(2, 2 * cellHeight + 4, cellWidth + 15, cellHeight);
	t30LunText->setBounds(2, 3 * cellHeight + 4, cellWidth + 15, cellHeight);
	t10SchText->setBounds(2, 4 * cellHeight + 5, cellWidth + 15, cellHeight);
	t20SchText->setBounds(2, 5 * cellHeight + 5, cellWidth + 15, cellHeight);
	t30SchText->setBounds(2, 6 * cellHeight + 5, cellWidth + 15, cellHeight);
	predelayText->setBounds(2, 7* cellHeight + 6, cellWidth + 15, cellHeight);
	if (!data.isEmpty())
	{
		for (int column = 0; column < columns; ++column)
		{
			data.getUnchecked(column * nOfCells)->get()->setBounds(99 + column * (cellWidth + 2), 2 , cellWidth, cellHeight);
			data.getUnchecked(column * nOfCells + 1)->get()->setBounds(99 + column * (cellWidth + 2),4+ 1 * (cellHeight), cellWidth, cellHeight);
			data.getUnchecked(column * nOfCells + 2)->get()->setBounds(99 + column * (cellWidth + 2),4+  2 * (cellHeight), cellWidth, cellHeight);
			data.getUnchecked(column * nOfCells + 3)->get()->setBounds(99 + column * (cellWidth + 2),4+  3 * (cellHeight), cellWidth, cellHeight);
			data.getUnchecked(column * nOfCells + 4)->get()->setBounds(99 + column * (cellWidth + 2),5+  4 * (cellHeight), cellWidth, cellHeight);
			data.getUnchecked(column * nOfCells + 5)->get()->setBounds(99 + column * (cellWidth + 2),5+  5 * (cellHeight), cellWidth, cellHeight);
			data.getUnchecked(column * nOfCells + 6)->get()->setBounds(99 + column * (cellWidth + 2),5+  6 * (cellHeight), cellWidth, cellHeight);
			data.getUnchecked(column * nOfCells + 7)->get()->setBounds(99 + column * (cellWidth + 2),6+  7 * (cellHeight), cellWidth, cellHeight);
		}
	}

}
void MvTable::updateDataReferences(Array<LundebyReturn> &additionalLundebyData, Array<SchroederReturn> &additionalSchroederData)
{
	_lundebyAdditionalDataPointer.release();
	_schroederAdditionalDataPointer.release();
	_lundebyAdditionalDataPointer.set(&additionalLundebyData, false);
	_schroederAdditionalDataPointer.set(&additionalSchroederData, false);
	updateData();
	resized();
}
void MvTable::updateSampleRate(float currentSampleRate)
{
	_currentSampleRate = currentSampleRate;
}
#pragma endregion

#pragma region Private

void MvTable::initalizeAllMyComponents()
{
	channelText = new TextEditor();
	channelText->setText("Channel", false);
	channelText->setJustification(juce::Justification::centred);
	channelText->setReadOnly(true);
	channelText->setColour(TextEditor::textColourId, Colours::black);

	t10LunText = new TextEditor();
	t10LunText->setText("T10 Lun [ms]", false);
	t10LunText->setJustification(juce::Justification::centred);
	t10LunText->setReadOnly(true);
	
	t20LunText = new TextEditor();
	t20LunText->setText("T20 Lun [ms]", false);
	t20LunText->setJustification(juce::Justification::centred);
	t20LunText->setReadOnly(true);
	
	t30LunText = new TextEditor();
	t30LunText->setText("T30 Lun [ms]", false);
	t30LunText->setJustification(juce::Justification::centred);
	t30LunText->setReadOnly(true);

	t10SchText = new TextEditor();
	t10SchText->setText("T10 Sch [ms]", false);
	t10SchText->setJustification(juce::Justification::centred);
	t10SchText->setReadOnly(true);

	t20SchText = new TextEditor();
	t20SchText->setText("T20 Sch [ms]", false);
	t20SchText->setJustification(juce::Justification::centred);
	t20SchText->setReadOnly(true);

	t30SchText = new TextEditor();
	t30SchText->setText("T30 Sch [ms]", false);
	t30SchText->setJustification(juce::Justification::centred);
	t30SchText->setReadOnly(true);

	predelayText = new TextEditor();
	predelayText->setText("Predelay [ms]", false);
	predelayText->setJustification(juce::Justification::centred);
	predelayText->setReadOnly(true);
}
void MvTable::addAndMakeVisibleAllMyComponents()
{
	addAndMakeVisible(channelText);
	addAndMakeVisible(t10LunText);
	addAndMakeVisible(t20LunText);
	addAndMakeVisible(t30LunText);
	addAndMakeVisible(t10SchText);
	addAndMakeVisible(t20SchText);
	addAndMakeVisible(t30SchText);
	addAndMakeVisible(predelayText);
}
void MvTable::updateData()
{
	data.clear();
	if (_lundebyAdditionalDataPointer != nullptr)
	{
		int size = _lundebyAdditionalDataPointer->size();
		for (int i = 0; i < size; ++i)
		{
			//channel
			data.add(new ScopedPointer<TextEditor>{ new TextEditor });
			data.getUnchecked(i*nOfCells)->get()->setText(String::formatted("Channel %d", i + 1));
			data.getUnchecked(i * nOfCells)->get()->setReadOnly(true);
			data.getUnchecked(i * nOfCells)->get()->setJustification(Justification::right);
			addAndMakeVisible(data.getUnchecked(i * nOfCells)->get());

			//t10 lun
			data.add(new ScopedPointer<TextEditor>{ new TextEditor });
			data.getUnchecked(i * nOfCells + 1)->get()->setText(String::formatted("%.1f", _lundebyAdditionalDataPointer->getUnchecked(i).T10*1000.0));
			data.getUnchecked(i * nOfCells + 1)->get()->setReadOnly(true);
			data.getUnchecked(i * nOfCells + 1)->get()->setJustification(Justification::right);
			addAndMakeVisible(data.getUnchecked(i * nOfCells + 1)->get());
			//t20 lun
			data.add(new ScopedPointer<TextEditor>{ new TextEditor });
			data.getUnchecked(i * nOfCells + 2)->get()->setText(String::formatted("%.1f", _lundebyAdditionalDataPointer->getUnchecked(i).T20*1000.0));
			data.getUnchecked(i * nOfCells + 2)->get()->setReadOnly(true);
			data.getUnchecked(i * nOfCells + 2)->get()->setJustification(Justification::right);
			addAndMakeVisible(data.getUnchecked(i * nOfCells + 2)->get());
			//t30 lun
			data.add(new ScopedPointer<TextEditor>{ new TextEditor });
			data.getUnchecked(i * nOfCells + 3)->get()->setText(String::formatted("%.1f", _lundebyAdditionalDataPointer->getUnchecked(i).T30*1000.0));
			data.getUnchecked(i * nOfCells + 3)->get()->setReadOnly(true);
			data.getUnchecked(i * nOfCells + 3)->get()->setJustification(Justification::right);
			addAndMakeVisible(data.getUnchecked(i * nOfCells + 3)->get());

			//t10 sch
			data.add(new ScopedPointer<TextEditor>{ new TextEditor });
			data.getUnchecked(i * nOfCells + 4)->get()->setText(String::formatted("%.1f", _schroederAdditionalDataPointer->getUnchecked(i).T10*1000.0));
			data.getUnchecked(i * nOfCells + 4)->get()->setReadOnly(true);
			data.getUnchecked(i * nOfCells + 4)->get()->setJustification(Justification::right);
			addAndMakeVisible(data.getUnchecked(i * nOfCells + 4)->get());
			//t20 sch
			data.add(new ScopedPointer<TextEditor>{ new TextEditor });
			data.getUnchecked(i * nOfCells + 5)->get()->setText(String::formatted("%.1f", _schroederAdditionalDataPointer->getUnchecked(i).T20*1000.0));
			data.getUnchecked(i * nOfCells + 5)->get()->setReadOnly(true);
			data.getUnchecked(i * nOfCells + 5)->get()->setJustification(Justification::right);
			addAndMakeVisible(data.getUnchecked(i * nOfCells + 5)->get());
			//t30 sch
			data.add(new ScopedPointer<TextEditor>{ new TextEditor });
			data.getUnchecked(i * nOfCells + 6)->get()->setText(String::formatted("%.1f", _schroederAdditionalDataPointer->getUnchecked(i).T30*1000.0));
			data.getUnchecked(i * nOfCells + 6)->get()->setReadOnly(true);
			data.getUnchecked(i * nOfCells + 6)->get()->setJustification(Justification::right);
			addAndMakeVisible(data.getUnchecked(i * nOfCells + 6)->get());

			//predelay
			data.add(new ScopedPointer<TextEditor>{ new TextEditor });
			data.getUnchecked(i * nOfCells + 7)->get()->setText(String::formatted("%.1f", _lundebyAdditionalDataPointer->getUnchecked(i).Predelay/(_currentSampleRate/1000.0)));
			data.getUnchecked(i * nOfCells + 7)->get()->setReadOnly(true);
			data.getUnchecked(i * nOfCells + 7)->get()->setJustification(Justification::right);
			addAndMakeVisible(data.getUnchecked(i * nOfCells + 7)->get());
		}
	}
}
#pragma endregion
