
#include "MainComponent.h"

//==============================================================================

#pragma region Public
MainComponent::MainComponent()
{
	MvLaF = new MvLookAndFeel();
	initializeAllMyComponents();
	setUpGUI();

	setAudioChannels(2, 2);
	

	setSize(960, 640);
	
	myCalibrationTab->updateIndicators();
	myMeasuringTab->updateMainComponentReferences(recordedData,irData,lundebyData,schroederData,
		currentLundebyAdditionalData,currentSchroederAdditionalData);	 //set reference to data 
}
MainComponent::~MainComponent()
{
	menuBar->setLookAndFeel(nullptr);
	measuringThread.release();
	shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	// This function will be called when the audio device is started, or when
	// its settings (i.e. sample rate, block size, etc) are changed.

	// You can use this function to initialise any resources you might need,
	// but be careful - it will be called on the audio thread, not the GUI thread.

	// For more details, see the help for AudioProcessor::prepareToPlay()
	activeInputs.clear();
	activeOutputs.clear();
	activeInputs = MvTabCalibration::maskToActiveChannelsAr(deviceManager.getCurrentAudioDevice()->getActiveInputChannels());
	activeOutputs = MvTabCalibration::maskToActiveChannelsAr(deviceManager.getCurrentAudioDevice()->getActiveOutputChannels());

	myMeasuringTab->updateCurrentSampleRate(deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());		//notify measuring tab about sample rate
	myCalibrationTab->deleteIndicators();
	myCalibrationTab->updateIndicators();	//když to tady není tak se nìkde stane vector out of range, je možný že je to v timer callbacku v calibration tab
	myCalibrationTab->resetCalibrationSlider();

}
void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	AudioIODevice * device = deviceManager.getCurrentAudioDevice();

	auto inpLat = device->getInputLatencyInSamples();		//dust debug info
	auto outLat = device->getOutputLatencyInSamples();

	auto activeInputChannels = device->getActiveInputChannels();
	auto activeOutputChannels = device->getActiveOutputChannels();

	auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
	auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

	int maxChannels = maxInputChannels;
	if (maxOutputChannels > maxInputChannels)
	{
		maxChannels = maxOutputChannels;
	}
	
	int bufferChannelToReadFrom = 0;

	//==== Input channels loop ====
	for (int channel = 0; channel < maxChannels; ++channel)
	{
		
		if (activeInputs.contains(channel))					//there is an input channel, with this index
		{
			auto* inBuffer = bufferToFill.buffer->getReadPointer(bufferChannelToReadFrom, bufferToFill.startSample);

			HeapBlock<float> channelData(bufferToFill.numSamples);
			FloatVectorOperations::abs(channelData.getData(), inBuffer, bufferToFill.numSamples);
			myCalibrationTab->setCurrentInputValue(FloatVectorOperations::findMaximum(channelData.getData(), bufferToFill.numSamples), bufferChannelToReadFrom);

			if (isRecorded &&recordedData.getNumSamples()>0)			//i wanna record this channel, sometimes it happens, that data gets deleted faster, than flipping the bool flag (when cancel pressed)
			{
				for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
				{
					if (inputPosition + sample < inputSignal.getNumSamples())
					{	//lets put it into the memory
						//to apply the calibration constant, i have to power the sample to the calibration constant
						recordedData.addSample(bufferChannelToReadFrom, inputPosition + sample, inBuffer[sample] * powf(10, myCalibrationTab->getCurrentCalibrationConstant(bufferChannelToReadFrom)/20.0));
					}
					else
					{
						markToStopPlaying = true;
						markToStopRecording = true;
					}
				}
			}
			else					//i dont wanna record this channel
			{
				//well.. lets leave it as it is
			}
			++bufferChannelToReadFrom;		/*there's been a match, so i'll iterate this one, as we have already dealt 
											with the one, we needed, whether it was recorded or not*/
		}
	}
	
	//==== Output channels loop
	bufferChannelToReadFrom = 0;		//lets change it back to zero, because now i'm gonna be iterating through output channels
	for (int channel = 0; channel < maxChannels; ++channel)
	{
		if (activeOutputs.contains(channel))
		{
			//and we got a match here!
			auto* outBuffer = bufferToFill.buffer->getWritePointer(bufferChannelToReadFrom, bufferToFill.startSample);

			if (isPlaying)		//i wanna play the sound
			{
				for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
				{
					if (inputPosition + sample < inputSignal.getNumSamples())
					{
						outBuffer[sample] = inputSignal.getSample(0, inputPosition + sample);	//rewrite it to the test signal
					}
					else
					{
						outBuffer[sample] = 0;
						markToStopPlaying = true;		//i think these are redundant here, but let's leave it
						markToStopRecording = true;
					}

				}
			}
			else				//i don't wanna play anything now, so lets clear the buffer
			{
				bufferToFill.buffer->clear(bufferChannelToReadFrom, bufferToFill.startSample, bufferToFill.numSamples);		//buffer is cleared
			}
			++bufferChannelToReadFrom;		/*there's been a match, so i'll iterate this one, as we have already dealt
												with the one, we needed, whether it was played or not*/
		}
	}
	
	inputPosition += bufferToFill.numSamples;		//až když naplním oba kanály tím samým, tak až posunu pozici od které budu èíst

	if (isWaitingForNextMeasurement)	//we are between repetitions
	{
		if (currentWaitingLoop > 9)	//20 "quiet" loops have passed, let's go back to recording
		{
			inputPosition = 0;
			currentWaitingLoop = 0;
			isRecorded = isPlaying = true;
			isWaitingForNextMeasurement = false;
		}
		else
		{
			++currentWaitingLoop;
		}
	}
	if (markToStopPlaying)		//if we have reached the end of recording in this buffer, let's flip the global flags
	{
		isPlaying = false;
		markToStopPlaying = false;
		if (isRecorded && myMeasuringTab->getNOfRepetitions() > 1 && currentRepetition <myMeasuringTab->getNOfRepetitions())
		{
			isWaitingForNextMeasurement = true;
			++currentRepetition;
		}
	}
	if (markToStopRecording)
	{
		isRecorded = false;			//this will actually trigger timerCallback and draw the recorded and processed data
		markToStopRecording = false;
	}
	
}
void MainComponent::releaseResources()
{
	// This will be called when the audio device stops, or when it is being
	// restarted due to a setting change.

	// For more details, see the help for AudioProcessor::releaseResources()
}
//==== Standard methods ====
void MainComponent::paint(Graphics& g)
{
	//g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	g.fillAll(Colours::ghostwhite);
}
void MainComponent::resized()
{
	menuBar->setBounds(0, 0, getWidth(), LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight());
	myTabs->setBounds(getLocalBounds().removeFromBottom(getHeight()- LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));
}

//==== Callbacks ====
void MainComponent::buttonTriggered(int buttonId)
{
	//start measuring!
	if (buttonId == 1)	
	{
		if (activeInputs.isEmpty())
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "No input device selected!","Please select input device in Device settings tab.","OK");
			return;
		}
		if (activeOutputs.isEmpty())
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "No output device selected!", "Please select output device in Device settings tab.", "OK");
			return;
		}
		if (deviceManager.getCurrentAudioDevice() == nullptr)
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "No device selected!", "Please select device in Device settings tab.", "OK");
			return;
		}
		if (myMeasuringTab->getLengthTime()<1 && myMeasuringTab->getTypeOfSignal() != 3)	//something else than MLS, i dont send length to MLS signal constructor
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Signal length set to 0!", "Please set signal length at least 1 ms.", "OK");
			return;
		}

		
		generateInputSignal();

		//set the recorded data buffer to fit with input signal
		AudioIODevice * device = deviceManager.getCurrentAudioDevice();
		auto activeInputChannels = device->getActiveInputChannels();
		auto maxInputChannels = activeInputChannels.getHighestBit() + 1;

		recordedData.setSize(0, 0, false, true, false);
		irData.setSize(0, 0, false, true, false);
		lundebyData.setSize(0, 0, false, true, false);
		currentLundebyAdditionalData.clear();	//array
		schroederData.setSize(0, 0, false, true, false);
		currentSchroederAdditionalData.clear();	//array

		int latencyOffset = deviceManager.getCurrentAudioDevice()->getInputLatencyInSamples() + deviceManager.getCurrentAudioDevice()->getOutputLatencyInSamples() -
			deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples() / 2;			
		//same length as input - extra samples for decay and latency
		recordedData.setSize(getNumberOfBufferChannels(), inputSignal.getNumSamples(), false, true);	
		//original input length
		irData.setSize(getNumberOfBufferChannels(), inputSignal.getNumSamples()-latencyOffset - (int)(getOriginalInputLength() / 4), false, true);
		//original input length
		lundebyData.setSize(getNumberOfBufferChannels(), inputSignal.getNumSamples() - latencyOffset - (int)(getOriginalInputLength() / 4), false, true);
		currentLundebyAdditionalData.resize(getNumberOfBufferChannels());				//allocate size for results
		//original input length
		schroederData.setSize(getNumberOfBufferChannels(), inputSignal.getNumSamples() - latencyOffset - (int)(getOriginalInputLength() / 4), false, true);
		currentSchroederAdditionalData.resize(getNumberOfBufferChannels());

		currentWaitingLoop = inputPosition = 0;
		currentRepetition = 1;
		
		isPlaying = isRecorded = true;		//hopefully it will change at the same time
		startTimerHz(10);					/*this checks isRecorded value, as it is run after setting it to true, it will run, until it's false, 
											then it will draw data or call other methods for processing the recorded data*/
		measuringThread.release();
		measuringThread = new MvMeasuringThread{};
		measuringThread->addListener(this);
		measuringThread->launchThread(8);

		return;

	}
	//stop recording!
	if (buttonId == 2)	
	{
		if (isRecorded && isPlaying)
		{
			stopTimer();
			isRecorded = isPlaying = false;

		}
		if(isRecorded == true)
			isRecorded = false;
		if (isPlaying == true)
			isPlaying = false;
	}
	//create and play input signal
	if (buttonId == 3)	
	{
		if (activeOutputs.isEmpty())
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "No output device selected!", "Please select output device in Device settings tab.", "OK");
			return;
		}
		if (deviceManager.getCurrentAudioDevice() == nullptr)
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "No device selected!", "Please select device in Device settings tab.", "OK");
			return;
		}
		if (myMeasuringTab->getLengthTime()<1 && myMeasuringTab->getTypeOfSignal() != 3)	//something else than MLS, i dont send length to MLS signal constructor
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Signal length set to 0!", "Please set signal length at least 1 ms.", "OK");
			return;
		}

		generateInputSignal();

		//sendLogMessage();

		inputPosition = 0;
		isPlaying = !isPlaying;
		return;
	}
	//clear the recorded data
	if (buttonId == 4)	
	{
		if (isRecorded)
			return;
		
		recordedData.clear();
		irData.clear();
		lundebyData.clear();
		currentLundebyAdditionalData.clear();
		schroederData.clear();
		currentSchroederAdditionalData.clear();

		recordedData.setSize(0, 0, false, true, false);
		irData.setSize(0, 0, false, true, false);
		lundebyData.setSize(0, 0, false, true, false);
		schroederData.setSize(0, 0, false, true, false);

		myMeasuringTab->drawData();
	}
}
void MainComponent::timerCallback()
{
	if (!isRecorded)	//timer started when is recorded was flipped on, so check if the recording finished
	{
		if (isWaitingForNextMeasurement)
		{
			return;
		}
		stopTimer();
		
		//let's compensate back the smapes added due to latency. input signal is longer at the end, but recorded data have extra samples at the beginning

		//inputSignal.setSize(1, getOriginalInputLength(), true, true, true);
		
		int latencyOffset = deviceManager.getCurrentAudioDevice()->getInputLatencyInSamples() + deviceManager.getCurrentAudioDevice()->getOutputLatencyInSamples() -
			deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples() / 2 ;			//i'm reducing it by 100, for safety reasons, sometimes the recorded data starts even before this reported offset

		//arrayResize(recordedData, latencyOffset, recordedData.getNumSamples() - 1);
		
		myMeasuringTab->updateMainComponentReferences(recordedData,irData,lundebyData,schroederData,
			currentLundebyAdditionalData, currentSchroederAdditionalData);	//update the component, that there has been a change in recorded data

		//tady bych mohl nìjak udìlat, že se spustí to smaotný moje vlákno a tam by mohl bejt i ten progress bar a 
		//v tom celým vláknì by se udìlaly ty výpoèty, ono to asi bude chvíli trvat, tak by bylo dobrý na to upozornit, ale tøeba èasem...

		if (myMeasuringTab->getNOfRepetitions() > 1)		//make the cumulated data into actual average - so divide by number of repetitions
		{
			float repetitions = float(myMeasuringTab->getNOfRepetitions());
			for (int channel = 0; channel < recordedData.getNumChannels(); ++channel)
			{
				auto * recordedDataChannelReader = recordedData.getReadPointer(channel);
				auto * recordedDataChannelWriter = recordedData.getWritePointer(channel);
				for (int i = 0; i < recordedData.getNumSamples(); ++i)
				{
					float temp = recordedDataChannelReader[i];
					recordedDataChannelWriter[i] = temp / repetitions;
				}
			}
		}

		//nyní provedu výpoèty tady, takže všechny IR, lundeby, schroeder...
		if (myMeasuringTab->getTypeOfSignal() == 3)
		{
			Compute::IRMLS(irData, recordedData, myMeasuringTab->getMLSOrder());
		}
		else
		{
			Compute::IRSweep(inputSignal, recordedData, irData);		
			doTheMagic();
			Compute::FilterIR(irData, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate(), myMeasuringTab->getStartFrequency());
		}
		
		Compute::NormalizeBuffer(irData);

		Compute::LundebysMethod(irData, lundebyData, currentLundebyAdditionalData, (int)(deviceManager.getCurrentAudioDevice()->getCurrentSampleRate()));
		Compute::SchroederInt(irData, schroederData, currentSchroederAdditionalData, currentLundebyAdditionalData, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
		

		myMeasuringTab->drawData();

		measuringThread->shouldStop = true;	//after the drawing is done, let's remove the progress window, this will cause it to stop naturally
		measuringThread.release();			//and release the scoped pointer, so the object gets deleted automatically

	}
}
void MainComponent::measuringThreadCallback(bool processCancelled)
{
	{
		stopTimer();
		isRecorded = isPlaying = isWaitingForNextMeasurement=  false;
	}
	//buttonTriggered(2);	//cancel 
	buttonTriggered(4);	//clear whatever is in memory
	measuringThread.release();
}

//==== Other methods - Menu ====
StringArray MainComponent::getMenuBarNames()
{
	return { "Export ", "Info" };
}
PopupMenu MainComponent::getMenuForIndex(int menuIndex, const String& menuName)
{
	PopupMenu menu;
	if (menuIndex == 0)
	{
		menu.addItem(1, "Input signal (.wav)");
		menu.addItem(2, "Input signal (.csv)");
		menu.addItem(3, "Recorded data (.wav)");
		menu.addItem(4, "Recorded data (.csv)");
		menu.addItem(5, "Impulse response (.wav)");
		menu.addItem(6, "Impulse response (.csv)");
		menu.addItem(7, "Lundeby's regression (.csv)");
		menu.addItem(8, "Schroeder integral (.csv)");
	}
	else if (menuIndex == 1)
	{
		menu.addItem(1, "English");
		menu.addItem(2, "Czech");
	}
	return menu;
}
void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{

	switch (topLevelMenuIndex)
	{
	case MenuTopID::exportMenu:
		if (recordedData.getNumSamples()<1)
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "No data to export!","", "OK");
			return;
		}
		switch (menuItemID)
		{
		case exportInputWav:writeDataToFile(inputSignal, true, exportInputWav);
			break;
		case exportInputCsv:writeDataToFile(inputSignal, false, exportInputCsv);
			break;
		case exportRecordedWav:writeDataToFile(recordedData, true, exportRecordedWav);
			break;
		case exportRecordedCsv:writeDataToFile(recordedData, false, exportRecordedCsv);
			break;
		case exportIRWav:writeDataToFile(irData, true, exportIRWav);
			break;
		case exportIRCsv:writeDataToFile(irData, false, exportIRCsv);
			break;
		case exportLundeby:writeDataToFile(lundebyData, false, exportLundeby);
			break;
		case exportSchroeder:writeDataToFile(schroederData, false, exportSchroeder);
			break;
		default:
			break;
		};
		break;
	case MenuTopID::thirdMenu:
		switch (menuItemID)
		{
		case English:
			//AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, CharPointer_UTF8("N\xc3\xa1pov\xc4\x9b""da"), CharPointer_UTF8(
			//	"VST plugin modul s funkcemi compressor, expander, limiter a gate. Vstupn\xc3\xad sign\xc3\xa1l je z\xc3\xa1rove\xc5\x88 pou\xc5\xbeit i jako zdroj \xc5\x99\xc3\xad""d\xc3\xad""c\xc3\xadho sign\xc3\xa1lu.\n\n"
			//	"Sign\xc3\xa1l v \xc5\x99\xc3\xad""d\xc3\xad""c\xc3\xad v\xc4\x9btvi je zpracov\xc3\xa1n n\xc3\xa1sledovn\xc4\x9b:\n\n"
			//	"Odhad ob\xc3\xa1lky sign\xc3\xa1lu \xe2\x80\x93 peak nebo RMS\n"
			//	"V\xc3\xbdpo\xc4\x8d""et zesilovac\xc3\xadho \xc4\x8dinitele \xe2\x80\x93 compressor, expander, limiter nebo gate\n"
			//	"Vyhlazen\xc3\xad zesilovac\xc3\xadho \xc4\x8dinitele \xe2\x80\x93 attack/release nebo attack/release/hold filtr\n\n"
			//	"Visualis\xc3\xa9ry zobrazuj\xc3\xad v\xc5\xbe""dy sign\xc3\xa1l za p\xc5\x99\xc3\xadslu\xc5\xa1n\xc3\xbdm blokem, posledn\xc3\xad visualis\xc3\xa9r zobrazuje v\xc3\xbdstupn\xc3\xad sign\xc3\xa1l.\n\n"
			//	"Sign\xc3\xa1l jednotliv\xc3\xbd""ch blok\xc5\xaf je mo\xc5\xben\xc3\xa9 nahr\xc3\xa1t stiskem p\xc5\x99\xc3\xadslu\xc5\xa1""en\xc3\xa9ho tla\xc4\x8d\xc3\xadtka a v\xc3\xbd""b\xc4\x9brem souboru pro ulo\xc5\xbe""en\xc3\xad vzork\xc5\xaf. Data jsou ulo\xc5\xbe""ena ve form\xc3\xa1tu CSV, jednotliv\xc3\xa9 sloupce odpov\xc3\xad""daj\xc3\xad sign\xc3\xa1lu visualis\xc3\xa9r\xc5\xaf. Nahr\xc3\xa1v\xc3\xa1n\xc3\xad je mo\xc5\xben\xc3\xa9 takt\xc3\xa9\xc5\xbe spustit pomoc\xc3\xad automatizace, parametru recording. N\xc3\xa1zev souboru posledn\xc3\xadho nahr\xc3\xa1v\xc3\xa1n\xc3\xad se zobraz\xc3\xad vedle tla\xc4\x8d\xc3\xadtka nahr\xc3\xa1v\xc3\xa1n\xc3\xad. Soubory se ukl\xc3\xa1""daj\xc3\xad na plochu u\xc5\xbeivatele."
			//), "OK");
			AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, CharPointer_UTF8("About this application"), CharPointer_UTF8(
				"Application for room impulse response measuring.\n\n"
				"After setting the input device in Device settings tab, it is possible to calibrate individual input channels in Calibration and I/O tab.\n"
				"With acoustic calibrator set the desired level on one channel and then set calibration level with \"B\" button. Levels of other channels\n"
				"are set equal to global calibration level with \"M\" button. Purpose of that is to have equal level at all input channels.\n\n"
				
				"Measuring signal parameters are set in the Measuring tab. You can choose between linear and logarithmic sweep and MLS signal.\n"
				"After measuring is done, data are processed and acquired results are displayed in table below signal plot. You can choose to display\n"
				"impuse response, envelope of impulse response for Lundeby\'s method of analysis, Schroeder integral and recorded signal. You can\n"
				"choose which channel to display on the right, as well as T10, T20 and T30 regressions.\n\n"
				"Data can be exported in WAV or CSV format.\n\n\n"

				"Author:                    Matou\xC5\xA1 Vrb\xC3\xADk <xvrbik00@vutbr.cz>\n"
				"Supervisor:              doc. Ing. Ji\xc5\x99\xc3\xad Schimmel, Ph.D. <schimmel@feec.vutbr.cz>\n "

			), "OK");
			break;
		case Czech:
			AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, CharPointer_UTF8("N\xc3\xa1pov\xc4\x9b""da"), CharPointer_UTF8(
				"Aplikace pro m\xC4\x9B\xC5\x99""en\xC3\xAD impulsn\xC3\xAD""ch odezev prostor\xC5\xAF.\n\n"
				"Po nastaven\xC3\xAD vhodn\xC3\xA9ho vstupn\xC3\xADho za\xC5\x99\xC3\xADzen\xC3\xAD v z\xC3\xA1lo\xC5\xBE""ce Device settings je mo\xC5\xBEn\xC3\xA9 prov\xC3\xA9st kalibraci jednotliv\xC3\xBD""ch vstupn\xC3\xAD""ch kan\xC3\xA1l\xC5\xAF v z\xC3\xA1lo\xC5\xBE""ce\n"
				"Calibration and I/O. Za pou\xC5\xBEit\xC3\xAD akustick\xC3\xA9ho kalibr\xC3\xA1toru nastavte po\xC5\xBE""adovanou \xC3\xBArove\xC5\x88 sign\xC3\xA1lu na jednom ze vstupn\xC3\xAD""ch kan\xC3\xA1l\xC5\xAF a tuto\n"
				" \xC3\xBArove\xC5\x88 potvr\xC4\x8Fte stisknut\xC3\xADm tla\xC4\x8D\xC3\xADtka s ozna\xC4\x8D""en\xC3\xADm \"B\" na p\xC5\x99\xC3\xADslu\xC5\xA1n\xC3\xA9m kan\xC3\xA1lu. \xC3\x9Arovn\xC4\x9B ostatn\xC3\xAD""ch kan\xC3\xA1l\xC5\xAF je mo\xC5\xBEn\xC3\xA9 nastavit pomoc\xC3\xAD tla\xC4\x8D\xC3\xADtka\n"
				"s ozna\xC4\x8D""en\xC3\xADm \"M\". \xC3\x9A\xC4\x8D""elem kalibrace je m\xC3\xADt stejnou \xC3\xBArove\xC5\x88 vstupn\xC3\xAD""ch kan\xC3\xA1l\xC5\xAF.\n\n"

				"Parametry m\xC4\x9B\xC5\x99ic\xC3\xADho sign\xC3\xA1lu lze nastavit v z\xC3\xA1lo\xC5\xBE""ce Measuring. Je mo\xC5\xBEn\xC3\xA9 zvolit z line\xC3\xA1rn\xC4\x9B a logaritmicky p\xC5\x99""ela\xC4\x8Fovan\xC3\xA9ho harmonick\xC3\xA9ho\n"
				"sign\xC3\xA1lu a MLS sign\xC3\xA1lu. Po prob\xC4\x9Bhnut\xC3\xAD m\xC4\x9B\xC5\x99""en\xC3\xAD jsou data zpracov\xC3\xA1na a v\xC3\xBDsledky jsou zobrazeny v tabulce pod pr\xC5\xAF""b\xC4\x9Bhy sign\xC3\xA1l\xC5\xAF. M\xC5\xAF\xC5\xBE""ete\n"
				"vybrat mezi zobrazen\xC3\xADm impulsn\xC3\xAD odezvy, ob\xC3\xA1lky impulsn\xC3\xAD odezvy pro anal\xC3\xBDzu Lundebyho metodou, Schroederov\xC3\xBDm integr\xC3\xA1lem a nahra-\n"
				"n\xC3\xBDm sign\xC3\xA1lem. Sign\xC3\xA1ly k zobrazen\xC3\xAD je mo\xC5\xBEn\xC3\xA9 vybrat tla\xC4\x8D\xC3\xADtky napravo, stejn\xC4\x9B jako odpov\xC3\xAD""daj\xC3\xAD""c\xC3\xAD regrese k v\xC3\xBDpo\xC4\x8Dtu T10, T20 a T30.\n\n"

				"Data je mo\xC5\xBEn\xC3\xA9 vyexportovat ve form\xC3\xA1tu WAV nebo CSV.\n\n\n"
				
				"Autor:                  Matou\xC5\xA1 Vrb\xC3\xADk <xvrbik00@vutbr.cz>\n"
				"Vedouc\xc3\xad pr\xc3\xa1""ce:      doc. Ing. Ji\xc5\x99\xc3\xad Schimmel, Ph.D. <schimmel@feec.vutbr.cz>\n "


			), "OK");
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}
		
	return;
}

#pragma endregion

#pragma region Private

//======== My Methods ========

//==== Constructor related methods====
void MainComponent::initializeAllMyComponents()
{
	audioSetupComp = new AudioDeviceSelectorComponent(deviceManager,
		0,     // minimum input channels
		16,   // maximum input channels
		0,     // minimum output channels
		2,   // maximum output channels
		false, // ability to select midi inputs
		false, // ability to select midi output device
		false, // treat channels as stereo pairs
		false); // hide advanced options

	//audioSetupComp->setLookAndFeel(MvLaF);

	//====Tabs====

	myDeviceConfigTab = new MvTabDeviceConfig(audioSetupComp);
	myCalibrationTab = new MvTabCalibration(&deviceManager);
	myMeasuringTab = new MvTabMeasuring();
	//myDeviceConfigTab->setLookAndFeel(MvLaF);
	//myCalibrationTab->setLookAndFeel(MvLaF);
	//myMeasuringTab->setLookAndFeel(MvLaF);

	myMeasuringTab->addListener(this);
	myCalibrationTab->addListener(this);

	myTabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);

	myTabs->addTab("Device settings", getLookAndFeel().findColour(ListBox::ColourIds::outlineColourId), myDeviceConfigTab, false);	//false - delete components when not needed, rather false, bcs i'm passing audioSetupComp and i need it all the time, so i would not like it to be deleted
	myTabs->addTab("Calibration and I/O", getLookAndFeel().findColour(ListBox::ColourIds::outlineColourId), myCalibrationTab, false);
	myTabs->addTab("Measuring", getLookAndFeel().findColour(ListBox::ColourIds::outlineColourId), myMeasuringTab, false);

	//====Menu bar====
	menuBar = new MenuBarComponent(this);
	menuBar->setLookAndFeel(MvLaF);
	menuBar->setColour(TextButton::ColourIds::textColourOffId, Colours::black);
	menuBar->setColour(TextButton::buttonColourId, Colours::white);
}
void MainComponent::setUpGUI()
{
	addAndMakeVisible(myTabs);
	addAndMakeVisible(menuBar);
}

//===== Methods related to measuring ====
int MainComponent::getNumberOfBufferChannels()
{
	AudioIODevice * device = deviceManager.getCurrentAudioDevice();
	auto activeInputChannels = device->getActiveInputChannels();		//returns how manny inputs are turned on
	return activeInputChannels.countNumberOfSetBits();					//i'm kinda turned on right now...
}
void MainComponent::generateInputSignal()
{
	inputSignal.clear();
	inputSignal.setSize(0, 0, false, true, false);

	Signal *mySignal;
	switch (myMeasuringTab->getTypeOfSignal())
	{
	case 1: mySignal = new LogSweep(myMeasuringTab->getStartFrequency(), myMeasuringTab->getStopFrequency(),
		(int)(myMeasuringTab->getLengthTime()), myMeasuringTab->getAmplitude(), myMeasuringTab->getCurrentSampleRate());
		break;
	case 2: mySignal = new LinSweep(myMeasuringTab->getStartFrequency(), myMeasuringTab->getStopFrequency(),
		(int)(myMeasuringTab->getLengthTime()), myMeasuringTab->getAmplitude(), myMeasuringTab->getCurrentSampleRate());
		break;
	case 3: mySignal = new MLS(myMeasuringTab->getMLSOrder(), myMeasuringTab->getAmplitude(), myMeasuringTab->getCurrentSampleRate());
	default:
		break;
	}

	mySignal->GenerateSignal();
	inputSignal = mySignal->GetSignal();

	//important, if i dont do this, temp mySignal will be still allocated in memory and not freed!
	mySignal->~Signal();

	//Perform fade in and fade out
	if (inputSignal.getNumSamples() > 100)
	{
		inputSignal.applyGainRamp(0, 100, 0.0, 1.0);
		inputSignal.applyGainRamp(inputSignal.getNumSamples() - 50, 50, 1.0, 0.0);
	}
	int inLatency = deviceManager.getCurrentAudioDevice()->getInputLatencyInSamples();
	int outLatency = deviceManager.getCurrentAudioDevice()->getOutputLatencyInSamples();
	int currentBufferSize = deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples();
	int originalLength = inputSignal.getNumSamples();

	//lengthen the input signal by number of latency - inLatency + outLatency - currentBufferSize/2  -- the Juce method actually returns device latencies + currentBufferSize/4 so this compensates it
	//this only works with asio, i can't determine the behaviour of other devices or drivers
	inputSignal.setSize(1, originalLength + inLatency + outLatency - currentBufferSize/2 + (int)(getOriginalInputLength()/4), true, false, true);	//extra samles in length of latency are added at the end of signal
	for (int i = originalLength; i < inputSignal.getNumSamples(); ++i)
	{
		inputSignal.setSample(0, i, 0.0);
	}
}

//==== Other methods ====
int MainComponent::getOriginalInputLength()
{
	return (int)(((float)deviceManager.getCurrentAudioDevice()->getCurrentSampleRate()/ 1000.0)* myMeasuringTab->getLengthTime());
}
void MainComponent::doTheMagic()
{
	for (int channel = 0; channel < irData.getNumChannels(); ++channel)
	{
		auto * irDataReader = irData.getReadPointer(channel);
		auto * irdataWriter = irData.getWritePointer(channel);
		for (int sample = 0; sample < irData.getNumSamples()-1; ++sample)
		{
			irdataWriter[sample] = irDataReader[sample] + irDataReader[sample + 1];
		}
		for (int sample = 0; sample < irData.getNumSamples() - 1; ++sample)
		{
			irdataWriter[sample] = irDataReader[sample] + irDataReader[sample + 1];
		}
		irdataWriter[0] = 0.0;
		irdataWriter[irData.getNumSamples() - 1] = 0.0;
		for (int sample = 0; sample < irData.getNumSamples() - 1; ++sample)
		{
			irdataWriter[sample] /= 3.0;
		}
	}

}
void MainComponent::arrayResize(AudioBuffer<float> &bufferToResize, const int startSample, const int endSample)
{
	AudioBuffer<float> temp{ bufferToResize.getNumChannels(),endSample-startSample+1 };
	for (int channel = 0; channel < temp.getNumChannels(); ++channel)
	{ 
		temp.copyFrom(channel, 0, bufferToResize, channel, startSample, temp.getNumSamples());
	}
	bufferToResize.setSize(0, 0, false, true);	//might not be necessary
	bufferToResize = temp;
	temp.setSize(0, 0, false, true);			//free the temp
}
void MainComponent::writeDataToFile(AudioBuffer<float> &dataToSave, bool isWavFormat, MenuExportItemID dataSpecifier)
{
	ScopedPointer<FileChooser> fc;
	String fileName;
	int channels;
	switch (dataSpecifier)
	{
	case exportInputWav: fileName = "input_signal.wav";
		break;
	case exportInputCsv:fileName = "input_signal.csv";
		break;
	case exportRecordedWav:fileName = "recorded_signal.wav";
		break;
	case exportRecordedCsv:fileName = "recorded_signal.csv";
		break;
	case exportIRWav:fileName = "impulse_response.wav";
		break;
	case exportIRCsv:fileName = "impulse_response.csv";
		break;
	case exportLundeby:fileName = "lundebys_regression.csv";
		break;
	case exportSchroeder:fileName = "schroeder_integra.csv";
		break;
	default:
		break;
	}

	if (isWavFormat)
	{
		fc = new FileChooser{ "Select output directory",File::getSpecialLocation(File::SpecialLocationType::userDesktopDirectory).getChildFile(fileName),"*.wav",true };
		channels = dataToSave.getNumChannels();
	}
	else
	{
		fc = new FileChooser{ "Select output directory",File::getSpecialLocation(File::SpecialLocationType::userDesktopDirectory).getChildFile(fileName),"*.csv",true };
	}

	if (fc->browseForFileToSave(true))
	{
		File fileToWrite(fc->getResult());
		if (isWavFormat)
		{
			fileToWrite.deleteFile();

			FileOutputStream *fileStream= fileToWrite.createOutputStream() ;		//divný, když sem dám scoped pointer, tak to hodí chybu na konci, ale u toho csv je potøeba, jinak to soubor kloudnì nezavøe
			ScopedPointer<WavAudioFormat> wavFormat = new WavAudioFormat();
			ScopedPointer< AudioFormatWriter>audioWriter = wavFormat->createWriterFor(fileStream, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate(), channels, 24, NULL, 0);
			audioWriter->writeFromAudioSampleBuffer(dataToSave, 0, dataToSave.getNumSamples());
		}
		else
		{
			ScopedPointer<FileOutputStream > fileStream{ fileToWrite.createOutputStream() };
			fileToWrite.deleteFile();
			auto outputDataReaders = dataToSave.getArrayOfWritePointers();
			for (int sample = 0; sample < dataToSave.getNumSamples(); ++sample)
			{
				for (int channel = 0; channel < dataToSave.getNumChannels(); ++channel)
				{
					*fileStream << outputDataReaders[channel][sample] << ";";

				}
				*fileStream << "\n";
			}
		}
	}
}

#pragma endregion


