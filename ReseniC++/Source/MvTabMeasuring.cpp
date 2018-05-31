#include "MvTabMeasuring.h"

//==============================================================================

#pragma region Public

	MvTabMeasuring::MvTabMeasuring()
	{
		initializeAllMyComponents();
		addAndMakeVisibleAllMyComponents();
	}
	MvTabMeasuring::~MvTabMeasuring()
	{
		_datasetChooser->removeListener(this);
		_recordedDataPointer.release();
		_irDataPointer.release();
		_lundebyDataPointer.release();
		_schroederDataPointer.release();
		_lundebyAdditionalDataPointer.release();
		_schroederAdditionalDataPointer.release();
	}
	void MvTabMeasuring::paint(Graphics& g)
	{
		g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));   // clear the background		
		g.setColour(Colours::lightgrey);
		g.fillRoundedRectangle(divider, 2.0f);
	}
	void MvTabMeasuring::resized()
	{
		drawSignalSetupMenu();
		drawButtons();
		_datasetChooser->setBounds(150, 10, getWidth() - 150, getHeight() - 198);
		resultTable->setBounds(150, getHeight() - 186, 0, 184);
	}

#pragma region GetSet
	//====Getters for parent====
	float MvTabMeasuring::getAmplitude()
	{
		return _amplitude;
	}
	float MvTabMeasuring::getStartFrequency()
	{
		return _startFrequency;
	}
	float MvTabMeasuring::getStopFrequency()
	{
		return _stopFrequency;
	}
	float MvTabMeasuring::getLengthTime()
	{
		return _lengthTime;
	}
	int MvTabMeasuring::getMLSOrder()
	{
		return _mlsOrder;
	}
	int MvTabMeasuring::getTypeOfSignal()
	{
		return _typeOfSignal;
	}
	float MvTabMeasuring::getCurrentSampleRate()
	{
		return _currentSampleRate;
	}
	void MvTabMeasuring::updateCurrentSampleRate(float currentSampleRate)
	{
		_currentSampleRate = currentSampleRate;
		_plot.setCurrentSampleRate(currentSampleRate);
		resultTable->updateSampleRate(currentSampleRate);
		if (_typeOfSignal == 3)	
		{
			textEditorTextChanged(*mlsOrderText);
		}
		else
		{
			stopFrequencyText->setText(String::formatted("%d",(int)(currentSampleRate/2.0)));
		}
		amplitudeText->setText(String::formatted("%.2f",20*log10f(_amplitude)), false);						//(true)goes through the textChanged and sets private parameters on startup
		startFrequencyText->setText(String::formatted("%.0f", _startFrequency),false);
		nOfRepetitionsText->setText(String::formatted("%d", _nOfRepetitions),false);
	}
	int MvTabMeasuring::getNOfRepetitions()
	{
		return _nOfRepetitions;
	}
#pragma endregion

	//==== Other methods ====
	void MvTabMeasuring::updateMainComponentReferences(AudioBuffer<float> &recordedData, AudioBuffer<float> &irData,
		AudioBuffer<float> &lundebyData, AudioBuffer<float> &schroederData, Array<LundebyReturn> &additionalLundebyData, Array<SchroederReturn> &additionalSchroederData)
	{
		_recordedDataPointer.release();
		_irDataPointer.release();
		_lundebyDataPointer.release();
		_schroederDataPointer.release();
		_lundebyAdditionalDataPointer.release();
		_schroederAdditionalDataPointer.release();

		_recordedDataPointer = &recordedData;
		_irDataPointer = &irData;
		_lundebyDataPointer = &lundebyData;
		_schroederDataPointer = &schroederData;
		_lundebyAdditionalDataPointer = &additionalLundebyData;
		_schroederAdditionalDataPointer = &additionalSchroederData;
	}
	void MvTabMeasuring::drawData()
	{
		resultTable->updateDataReferences(*_lundebyAdditionalDataPointer, *_schroederAdditionalDataPointer);
		switch (_datasetChooser->getCurrentTabIndex())
		{
		case IR:
			_plot.addToGraph(*_irDataPointer);
			break;
		case Lundeby:
			_plot.addToGraph(*_lundebyDataPointer);
			break;
		case Schoeder:
			_plot.addToGraph(*_schroederDataPointer);
			break;
		case TF:
			_plot.addToGraph(*_recordedDataPointer);
			break;
		default:
			break;
		}
	}

	//==== Listener ====

	void MvTabMeasuring::addListener(Listener* listener) 
	{
		listeners.add(listener); 
	}
	void MvTabMeasuring::removeListener(Listener* listener) 
	{ 
		listeners.remove(listener); 
	}

#pragma endregion


#pragma region Private

	//==== Callbacks ====
	void MvTabMeasuring::textEditorTextChanged(TextEditor &currentTextEditor)
	{
		if (&currentTextEditor == amplitudeText)
		{
			_amplitude = powf(10, (currentTextEditor.getText().getFloatValue() / 20.0));
			return;
		}

		if (&currentTextEditor == startFrequencyText)
		{
			_startFrequency = currentTextEditor.getText().getFloatValue();
			return;
		}
		if (&currentTextEditor == stopFrequencyText)
		{
			_stopFrequency = currentTextEditor.getText().getFloatValue();
			return;
		}
		if (&currentTextEditor == mlsOrderText)
		{
			_mlsOrder = mlsOrderText->getText().getIntValue();
			if (_currentSampleRate != 0.0)
			{
				lengthTimeText->setText(String((int)((1000 * pow(2, _mlsOrder) - 1) / _currentSampleRate)));
			}
			else
			{
				lengthTimeText->setText(String((int)((1000 * pow(2, _mlsOrder) - 1) / 44100.0)));
			}
			return;
		}

		if (&currentTextEditor == lengthTimeText)
		{
			_lengthTime = currentTextEditor.getText().getFloatValue();
			return;
		}
		if (&currentTextEditor == nOfRepetitionsText)
		{
			_nOfRepetitions = currentTextEditor.getText().getIntValue();
			return;
		}
	}
	void MvTabMeasuring::textEditorFocusLost(TextEditor &currentTextEditor)
	{
		if (&currentTextEditor == amplitudeText)
		{
			if (currentTextEditor.getText().getFloatValue() >= 0.0f)
			{
				currentTextEditor.setText("0.00");
			}
			_amplitude = powf(10, (currentTextEditor.getText().getFloatValue() / 20.0));
			currentTextEditor.setText(String::formatted("%5.2f", currentTextEditor.getText().getFloatValue()));
			return;
		}
		if (&currentTextEditor == mlsOrderText)
		{
			if (mlsOrderText->getText().getIntValue() > 18)
			{
				mlsOrderText->setText("18");					//cant make longer now, maybe in the future
			}
			else if (mlsOrderText->getText().getIntValue() < 2)
			{
				mlsOrderText->setText("2");
			}
			_mlsOrder = mlsOrderText->getText().getIntValue();
			lengthTimeText->setText(String((int)((1000 * pow(2, _mlsOrder) - 1) / _currentSampleRate)));
			mlsPlusMinus->setValue(_mlsOrder);
			return;
		}
		if (&currentTextEditor == nOfRepetitionsText)
		{
			if (nOfRepetitionsText->getText().getIntValue() > 99)
			{
				nOfRepetitionsText->setText("99");
			}
			else if (nOfRepetitionsText->getText().getIntValue() < 1)
			{
				nOfRepetitionsText->setText("1");
			}
			_nOfRepetitions = nOfRepetitionsText->getText().getIntValue();
			repetitionsPlusMinus->setValue(_nOfRepetitions);
		}
	}
	void MvTabMeasuring::buttonClicked(Button *currentButton)
	{
		if (currentButton == startMeasuringButton)
		{
			listeners.call(&MvTabMeasuring::Listener::buttonTriggered, 1);		//create input signal, start playing it and simultaneously recording it
			return;
		}
		if (currentButton == testPlayInput)
		{
			listeners.call(&MvTabMeasuring::Listener::buttonTriggered, 3);		//create input signal, play it
			return;
		}

		if (currentButton == testStopRecording)
		{
			listeners.call(&MvTabMeasuring::Listener::buttonTriggered, 2);		//stop recording the data
			return;
		}
		if (currentButton == testClearRecordedData)
		{
			listeners.call(&MvTabMeasuring::Listener::buttonTriggered, 4);		//clear the recorded data
		}
	}
	void MvTabMeasuring::comboBoxChanged(ComboBox * comboBox)
	{
		if (comboBox == signalTypeCombo)
		{
			updateSignalSetupMenu();
			_typeOfSignal = signalTypeCombo->getSelectedId();
		}
		if (_typeOfSignal == 3)
		{
			lengthTimeText->setText(String((int)((1000 * pow(2, _mlsOrder) - 1) / _currentSampleRate)));
		}
	}
	void MvTabMeasuring::sliderValueChanged(Slider * slider)
	{
		if (slider == mlsPlusMinus)
		{
			mlsOrderText->setText(String::formatted("%d", (int)slider->getValue()));
		}
		if (slider = repetitionsPlusMinus)
		{
			nOfRepetitionsText->setText(String::formatted("%d", (int)slider->getValue()));
		}
	}
	void MvTabMeasuring::currentTabChanged(int newCurrentTabIndex, const String & newCurrentTabName)
	{
		_plot.setCurrentTab(_datasetChooser->getCurrentTabIndex());
		switch (newCurrentTabIndex)
		{
		case IR:
			_plot.releaseAdditional();		//releases pointers to additional data, so they dont get drawn
			_plot.addToGraph(*_irDataPointer);
			break;
		case Lundeby:
			_plot.addToGraphAdditional(*_lundebyAdditionalDataPointer);		//first the additional data, bcs they decide, if noise and decay buttons are drawn
			_plot.addToGraph(*_lundebyDataPointer);
			break;
		case Schoeder:
			_plot.addToGraphAdditional(*_schroederAdditionalDataPointer);
			_plot.addToGraph(*_schroederDataPointer);
			break;
		case TF:
			_plot.releaseAdditional();
			_plot.addToGraph(*_recordedDataPointer);
			break;
		default:
			break;
		}

	}

	//==== Constructor related methods ====
	void MvTabMeasuring::initializeAllMyComponents()
	{
		//====TextEditors====
		amplitudeText = new TextEditor("AmpitudeText");
		amplitudeText->setInputRestrictions(6, "0123456789.-");		//sets the text editor so that is only accepts these chars and parser can cope with it
		amplitudeText->addListener(this);
		amplitudeText->setJustification(Justification::right);
		_amplitude = 0.5011872336f;

		startFrequencyText = new TextEditor("StartFrequencyText");
		startFrequencyText->setInputRestrictions(6, "0123456789.");
		startFrequencyText->addListener(this);
		startFrequencyText->setJustification(Justification::right);
		_startFrequency = 100;

		stopFrequencyText = new TextEditor("StopFrequencyText");
		stopFrequencyText->setInputRestrictions(6, "0123456789.");
		stopFrequencyText->addListener(this);
		stopFrequencyText->setText("20000", true);
		stopFrequencyText->setJustification(Justification::right);
		_stopFrequency = 20000;

		mlsOrderText = new TextEditor("nOrder");
		mlsOrderText->setInputRestrictions(2, "0123456789");
		mlsOrderText->addListener(this);
		mlsOrderText->setJustification(Justification::right);
		_mlsOrder = 16;

		lengthTimeText = new TextEditor("lengthTimeText");
		lengthTimeText->setInputRestrictions(5, "0123456789,.");
		lengthTimeText->addListener(this);
		lengthTimeText->setJustification(Justification::right);
		_lengthTime = 1000;

		nOfRepetitionsText = new TextEditor("nOfRepetitionsText");
		nOfRepetitionsText->setInputRestrictions(2, "0123456789");		//sets the text editor so that is only accepts these chars and parser can cope with it
		nOfRepetitionsText->addListener(this);
		nOfRepetitionsText->setJustification(Justification::right);
		_nOfRepetitions = 1;

		//====Combo boxes====
		signalTypeCombo = new ComboBox("signalType");
		signalTypeCombo->setEditableText(false);
		signalTypeCombo->addItemList(StringArray{ "Logarithmic Sweep","Linear Sweep","MLS" }, 1);
		signalTypeCombo->setSelectedId(1, false);
		signalTypeCombo->addListener(this);


		//====Labels====
		amplitudeLabel = new Label("AmplitudeLabel", "Amplitude");
		startFrequencyLabel = new Label("startFrequencyLabel", "Start frequency");
		stopFrequencyLabel = new Label("stopFrequencyLabel", "Stop frequency");
		lengthTimeLabel = new Label("lengthTimeLabel", "Length");
		mlsOrderLabel = new Label("mlsOrderLabel", "Order of signal");
		signalTypeLabel = new Label("signalTypeLabel", "Signal type");
		nOfRepetitionsLabel = new Label("nOfRepetitionsLabel", "Repetitions");
		dBFSUnit = new Label("dBFS", "dBFS");
		startHzUnit = new Label("Hz", "Hz");
		startHzUnit->setJustificationType(juce::Justification::right);
		stopHzUnit = new Label("Hz", "Hz");
		stopHzUnit->setJustificationType(juce::Justification::right);
		msUnit = new Label("ms", "ms");
		msUnit->setJustificationType(juce::Justification::right);

		//====The divider====
		divider.setBounds(5, 310, 130, 4);

		//====Buttons====
		startMeasuringButton = new ImageButton();
		startMeasuringButton->setImages(false, true, true,
			ImageCache::getFromMemory(MvButtons::recordButton_png, MvButtons::recordButton_pngSize), 1.0f, Colour(0x00000000),
			ImageCache::getFromMemory(MvButtons::recordButton_png, MvButtons::recordButton_pngSize), 1.0f, Colour(0x08000000),
			ImageCache::getFromMemory(MvButtons::recordButton_png, MvButtons::recordButton_pngSize), 1.0f, Colour(0x24000000));
		startMeasuringButton->addListener(this);

		testPlayInput = new ImageButton();
		testPlayInput->setImages(false, true, true,
			ImageCache::getFromMemory(MvButtons::playButton_png, MvButtons::playButton_pngSize), 1.0f, Colour(0x00000000),
			ImageCache::getFromMemory(MvButtons::playButton_png, MvButtons::playButton_pngSize), 1.0f, Colour(0x08000000),
			ImageCache::getFromMemory(MvButtons::playButton_png, MvButtons::playButton_pngSize), 1.0f, Colour(0x24000000));
		testPlayInput->addListener(this);

		testStopRecording = new ImageButton();
		testStopRecording->setImages(false, true, true,
			ImageCache::getFromMemory(MvButtons::stopButton_png, MvButtons::stopButton_pngSize), 1.0f, Colour(0x00000000),
			ImageCache::getFromMemory(MvButtons::stopButton_png, MvButtons::stopButton_pngSize), 1.0f, Colour(0x08000000),
			ImageCache::getFromMemory(MvButtons::stopButton_png, MvButtons::stopButton_pngSize), 1.0f, Colour(0x24000000));
		testStopRecording->addListener(this);

		testClearRecordedData = new ImageButton();
		testClearRecordedData->setImages(false, true, true,
			ImageCache::getFromMemory(MvButtons::clearButton_png, MvButtons::clearButton_pngSize), 1.0f, Colour(0x00000000),
			ImageCache::getFromMemory(MvButtons::clearButton_png, MvButtons::clearButton_pngSize), 1.0f, Colour(0x08000000),
			ImageCache::getFromMemory(MvButtons::clearButton_png, MvButtons::clearButton_pngSize), 1.0f, Colour(0x24000000));
		testClearRecordedData->addListener(this);

		//==== Sliders ====
		mlsPlusMinus = new Slider{};
		mlsPlusMinus->setRange(1, 18, 1);
		mlsPlusMinus->setValue(16);
		mlsPlusMinus->setSliderStyle(Slider::IncDecButtons);
		mlsPlusMinus->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		mlsPlusMinus->addListener(this);

		repetitionsPlusMinus = new Slider{};
		repetitionsPlusMinus->setRange(1, 99, 1);
		repetitionsPlusMinus->setValue(1);
		repetitionsPlusMinus->setSliderStyle(Slider::IncDecButtons);
		repetitionsPlusMinus->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		repetitionsPlusMinus->addListener(this);

		//==== MvTabbedComponent - the signal display ====
		_datasetChooser = new MvTabbedComponent(TabbedButtonBar::TabsAtTop);
		_datasetChooser->addTab("Impulse response", getLookAndFeel().findColour(ListBox::ColourIds::outlineColourId), OptionalScopedPointer<MvSignalPlot>{&_plot, false}, true);
		_datasetChooser->addTab("Lundeby's regression", getLookAndFeel().findColour(ListBox::ColourIds::outlineColourId), OptionalScopedPointer<MvSignalPlot>{&_plot, false}, true);
		_datasetChooser->addTab("Schroeder integral", getLookAndFeel().findColour(ListBox::ColourIds::outlineColourId), OptionalScopedPointer<MvSignalPlot>{&_plot, false}, true);
		_datasetChooser->addTab("Recorded signal", getLookAndFeel().findColour(ListBox::ColourIds::outlineColourId), OptionalScopedPointer<MvSignalPlot>{&_plot, false}, true);
		_datasetChooser->addListener(this);


		//==== Result table ====
		resultTable = new MvTable();
		
	}
	void MvTabMeasuring::addAndMakeVisibleAllMyComponents()
	{
		//labels
		addAndMakeVisible(signalTypeLabel);
		addAndMakeVisible(amplitudeLabel);
		addAndMakeVisible(startFrequencyLabel);
		addAndMakeVisible(stopFrequencyLabel);
		addAndMakeVisible(mlsOrderLabel);
		addAndMakeVisible(lengthTimeLabel);
		addAndMakeVisible(nOfRepetitionsLabel);
		addAndMakeVisible(dBFSUnit);
		addAndMakeVisible(startHzUnit);
		addAndMakeVisible(stopHzUnit);
		addAndMakeVisible(msUnit);
		//text editors
		addAndMakeVisible(signalTypeCombo);
		addAndMakeVisible(amplitudeText);
		addAndMakeVisible(startFrequencyText);
		addAndMakeVisible(stopFrequencyText);
		addAndMakeVisible(mlsOrderText);
		addAndMakeVisible(lengthTimeText);
		addAndMakeVisible(nOfRepetitionsText);

		//buttons
		addAndMakeVisible(startMeasuringButton);
		addAndMakeVisible(testPlayInput);
		addAndMakeVisible(testStopRecording);
		addAndMakeVisible(testClearRecordedData);

		//==== Sliders ====
		addAndMakeVisible(mlsPlusMinus);
		addAndMakeVisible(repetitionsPlusMinus);

		//==== Plotter ====
		addAndMakeVisible(_datasetChooser);

		//==== Result table ====
		addAndMakeVisible(resultTable);
	}
	void MvTabMeasuring::drawSignalSetupMenu()
	{
		int currentX = 10, currentY = 15, yToTextIteration = 22, yToLabelIteration = 27, textEdWidth = 80, labelWidth = 120, textEdHeigth = 22;

		signalTypeLabel->setBounds(currentX, currentY, labelWidth, textEdHeigth);
		currentY += yToTextIteration;
		signalTypeCombo->setBounds(currentX, currentY, labelWidth, textEdHeigth);
		currentY += yToLabelIteration;

		amplitudeLabel->setBounds(currentX, currentY, labelWidth, textEdHeigth);
		currentY += yToTextIteration;
		amplitudeText->setBounds(currentX, currentY, textEdWidth, textEdHeigth);
		dBFSUnit->setBounds(currentX + textEdWidth, currentY, labelWidth - textEdWidth, textEdHeigth);
		currentY += yToLabelIteration;

		if (signalTypeCombo->getSelectedId() != 3)	// not MLS
		{
			startFrequencyLabel->setBounds(currentX, currentY, labelWidth, textEdHeigth);
			currentY += yToTextIteration;
			startFrequencyText->setBounds(currentX, currentY, textEdWidth, textEdHeigth);
			startHzUnit->setBounds(currentX + textEdWidth, currentY, labelWidth - textEdWidth, textEdHeigth);
			currentY += yToLabelIteration;

			stopFrequencyLabel->setBounds(currentX, currentY, labelWidth, textEdHeigth);
			currentY += yToTextIteration;
			stopFrequencyText->setBounds(currentX, currentY, textEdWidth, textEdHeigth);
			stopHzUnit->setBounds(currentX + textEdWidth, currentY, labelWidth - textEdWidth, textEdHeigth);
			currentY += yToLabelIteration;
		}
		else	//MLS
		{
			mlsOrderLabel->setBounds(currentX, currentY, labelWidth, textEdHeigth);
			currentY += yToTextIteration;
			mlsOrderText->setBounds(currentX, currentY, textEdWidth, textEdHeigth);
			mlsPlusMinus->setBounds(currentX + textEdWidth + 3, currentY, labelWidth - ( textEdWidth + 3), textEdHeigth);
			currentY += 2 * yToLabelIteration + yToTextIteration;
		}
		lengthTimeLabel->setBounds(currentX, currentY, labelWidth, textEdHeigth);
		currentY += yToTextIteration;
		lengthTimeText->setBounds(currentX, currentY, textEdWidth, textEdHeigth);
		msUnit->setBounds(currentX + textEdWidth, currentY, labelWidth - textEdWidth, textEdHeigth);
		currentY += yToLabelIteration;

		nOfRepetitionsLabel->setBounds(currentX, currentY, textEdWidth, textEdHeigth);
		currentY += yToTextIteration;
		nOfRepetitionsText->setBounds(currentX, currentY, textEdWidth, textEdHeigth);
		repetitionsPlusMinus->setBounds(currentX + textEdWidth + 3, currentY, labelWidth - ( textEdWidth + 3), textEdHeigth);
		currentY += yToLabelIteration;


	}
	void MvTabMeasuring::drawButtons()
	{
		int currentX = 10, currentY = 320;

		startMeasuringButton->setBounds(currentX, currentY, 59, 30);
		testPlayInput->setBounds(currentX + 61, currentY, 59, 30);
		currentY += 33;
		testClearRecordedData->setBounds(currentX, currentY, 59, 30);
		testStopRecording->setBounds(currentX + 61, currentY, 59, 30);

	}
	void MvTabMeasuring::updateSignalSetupMenu()
	{
		if (signalTypeCombo->getSelectedId() == 3)	//MLS
		{
			startFrequencyLabel->setVisible(false);
			startFrequencyText->setVisible(false);
			stopFrequencyLabel->setVisible(false);
			stopFrequencyText->setVisible(false);
			lengthTimeText->setEnabled(false);
			mlsOrderLabel->setVisible(true);
			mlsOrderText->setVisible(true);
			startHzUnit->setVisible(false);
			stopHzUnit->setVisible(false);
			mlsPlusMinus->setVisible(true);
		}
		else		//not MLS
		{
			startFrequencyLabel->setVisible(true);
			startFrequencyText->setVisible(true);
			stopFrequencyLabel->setVisible(true);
			stopFrequencyText->setVisible(true);
			lengthTimeText->setEnabled(true);
			mlsOrderLabel->setVisible(false);
			mlsOrderText->setVisible(false);
			startHzUnit->setVisible(true);
			stopHzUnit->setVisible(true);
			mlsPlusMinus->setVisible(false);
		}
		drawSignalSetupMenu();
	}

#pragma endregion
