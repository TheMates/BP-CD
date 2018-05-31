/*
  ==============================================================================

	MvTabCalibration.cpp
	Created: 16 May 2018 8:14:01pm
	Author:  Matouš

  ==============================================================================
*/

#include "MvTabCalibration.h"
#include "MvStructs.h"
//==============================================================================
#pragma region Public

MvTabCalibration::MvTabCalibration(AudioDeviceManager *deviceManager)
{
	myDeviceManager = deviceManager;

	fillFadeColours();

	startTimerHz(30);

	testLvlButton = new ImageButton{};
	testLvlButton->setImages(false, true, true,
		ImageCache::getFromMemory(MvButtons::playButton_png, MvButtons::playButton_pngSize), 1.0f, Colour(0x00000000),
		ImageCache::getFromMemory(MvButtons::playButton_png, MvButtons::playButton_pngSize), 1.0f, Colour(0x08000000),
		ImageCache::getFromMemory(MvButtons::playButton_png, MvButtons::playButton_pngSize), 1.0f, Colour(0x24000000));
	testLvlButton->setBounds(45, 10, 59, 30);
	testLvlButton->addListener(this);

	addAndMakeVisible(testLvlButton);

	calibrationSlider = new Slider();
	calibrationSlider->setRange(-60, 0, 0);
	calibrationSlider->setNumDecimalPlacesToDisplay(2);
	calibrationSlider->setSliderStyle(Slider::LinearVertical);
	calibrationSlider->setTextBoxStyle(Slider::TextBoxAbove,false,60,22);
	calibrationSlider->addListener(this);
	addAndMakeVisible(calibrationSlider);



	setSize(700, 500);
}
MvTabCalibration::~MvTabCalibration()
{
	myDeviceManager = nullptr;
}
void MvTabCalibration::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}
void MvTabCalibration::resized()
{
	calibrationSlider->setBounds(2, getHeight()*0.2f , 41, getHeight()*0.8f - 35);
	updateIndicatorBounds();
}


//==== My public methods ====
void MvTabCalibration::updateIndicatorBounds()
{
	for (int i = 0; i < _myIndicators.size(); ++i)
	{
		_myIndicators[i]->setBounds(45 + 102 * i, getHeight()*0.2f, 100, getHeight()*0.8f);
		//_myIndicators[i]->setChannelColours(fadeColours[i][1], fadeColours[i][0]);
		_myIndicators[i]->setChannelColours(0xffcefffa, 0xff97dfdf);
	}
}
void MvTabCalibration::resetCalibrationSlider()
{
	calibrationSlider->setValue(0.0, juce::dontSendNotification);
}
void MvTabCalibration::setCurrentInputValue(float value, int channel)
{
	if (_currentInputValues.size() != 0)
	{
		_currentInputValues[channel] = value;
	}
}
float MvTabCalibration::getCurrentCalibrationConstant(int channel)
{
	return _myIndicators[channel]->getCurrentCalibrationConstant();
}
void MvTabCalibration::updateIndicators()
{
	stopTimer();
	device = myDeviceManager->getCurrentAudioDevice();
	if (device == NULL)		//the device is broken or something
	{
		deleteIndicators();
		return;
	}

	_activeInputs = maskToActiveChannels(device->getActiveInputChannels());	//změní indexy který budou použitý pro kanály
																			/*
																			ODEBÍRÁNÍ- dostnau novej _activeInputs a budu checkovat vektor s indikátorama, jestli je tam ten index, kterej je v tomto novym vektoru
																			jestli jo, tak ho nesmažu, když tam není, tak ho smažu
																			PŘIDÁVÁNÍ - mám novej vektor _activeInputs, zkontroluju, jestli odpovídají jednotlivý buňky _activeInputs a indexy v _myIndicators,
																			když si odpovídá, jde se dál, když si neodpovídaj - přidáváme, tak přidám na pozici v _myIndicators tam, kde právě jsem a nesedí to
																			takže když budu mít indikátory 1,2,4 a inputy 1,2,3,4, tak 1,2 to přeskočí a na 3, který nebude sedět se 4 se prostě vytvoří novej indikátor
																			kterej bude na týto pozici takže asi nějakej vektor insert a bude mít tento (3) index
																			*/
	if (_myIndicators.size() == 0) //je to poprvé
	{
		for (int i = 0; i < _activeInputs.size(); ++i)
		{

			_myIndicators.push_back(ScopedPointer<MvMicrophoneIndicator>{new MvMicrophoneIndicator(String::formatted("Channel %d", i + 1), _activeInputs[i])});
			_myIndicators[i]->addListener(this);
			addAndMakeVisible(_myIndicators[i]);
		}
	}

	else
	{
		//odebírám
		if (_myIndicators.size() > _activeInputs.size())
		{
			if (_activeInputs.size() == 0)	//smazal jsem všechno
			{
				_myIndicators.clear();
			}
			for (int i = 0; i < _activeInputs.size(); ++i)
			{
				if (_activeInputs[i] != _myIndicators[i]->getChannelIndex())	//indexy nejsou stejný
				{
					std::vector<ScopedPointer<MvMicrophoneIndicator>>::iterator it = _myIndicators.begin() + i;
					//_myIndicators[i]->removeListener(this);
					_myIndicators.erase(it);
					--i;			//neinkrementovat hned, mohly by nastat 2 mazání (což se asi nestane) ale takhle se to zkontroluje pro jistotu ještě jednou, jestli další indexy se shodují
				}
			}
			if (_myIndicators.size() > _activeInputs.size())	//znova poronvám, jeslti je to pořád větší, jestli ano,potom smažu ten poslední
			{

				_myIndicators.pop_back();
			}


		}

		//přidávám
		else
		{
			for (int i = 0; i < _activeInputs.size(); ++i)
			{
				if (i == _myIndicators.size())	//přidávám na poslední pozici 
				{
					_myIndicators.push_back(ScopedPointer<MvMicrophoneIndicator>{new MvMicrophoneIndicator(String::formatted("Channel %d", _activeInputs[i]), _activeInputs[i])});
					addAndMakeVisible(_myIndicators[i]);
					//_myIndicators[i]->addListener(this);
					continue;
				}

				if (_activeInputs[i] != _myIndicators[i]->getChannelIndex())	//indexy nejsou stejný
				{
					std::vector<ScopedPointer<MvMicrophoneIndicator>>::iterator it = _myIndicators.begin() + i;
					_myIndicators.insert(it, (ScopedPointer<MvMicrophoneIndicator>{new MvMicrophoneIndicator(String::formatted("Channel %d", _activeInputs[i]), _activeInputs[i])}));
					addAndMakeVisible(_myIndicators[i]);
					//_myIndicators[i]->addListener(this);
					//--i;			//tady se podle mě inkrementova může vždycky, to co tam vložím, bude většinou tam, kde to má být
				}
			}
		}
	}

	if (_activeInputs.size() != _currentInputValues.size())
	{
		_currentInputValues.clear();
		for (int i = 0; i < _activeInputs.size(); ++i)	//hodim tam tolik buněk, kolik je aktivních input channelů a pak tam budu zapisovat current values
		{
			_currentInputValues.push_back(0.0);
			addAndMakeVisible(_myIndicators[i]);
		}
	}


	updateIndicatorBounds();

	updateCalibrationSliderVisibility();
	startTimerHz(30);
}
std::vector<int> MvTabCalibration::maskToActiveChannels(juce::BigInteger mask)
{
	//dá mi to vektor, ve kterejch je napsaný, který signály jsou aktivní, to pak zapíšu jako index kanálu do jednotlivejch indikátorů

	//tady se to možná sere když všechny kanály odstraním - že vrátm prázdnej vektor a pak to hodí tu chybu

	std::vector<int> result;
	auto maxInputChannels = mask.getHighestBit() + 1;
	for (int i = 0; i < maxInputChannels; ++i)
	{
		if (mask[i])
		{
			result.push_back(i + 1);
		}
	}
	return result;
}
Array<int> MvTabCalibration::maskToActiveChannelsAr(juce::BigInteger mask)
{
	//dá mi to Array, ve kterejch je napsaný, který signály jsou aktivní, to pak zapíšu jako index kanálu do jednotlivejch indikátorů

	//tady se to možná sere když všechny kanály odstraním - že vrátm prázdnej vektor a pak to hodí tu chybu

	Array<int> result;
	auto maxInputChannels = mask.getHighestBit() + 1;
	for (int i = 0; i < maxInputChannels; ++i)
	{
		if (mask[i])
		{
			result.add(i);
		}
	}
	return result;
}
void MvTabCalibration::deleteIndicators()
{
	_myIndicators.clear();
	_activeInputs.clear();
}

//==== Callbacks ====
void MvTabCalibration::timerCallback()
{
	for (int i = 0; i < _activeInputs.size(); ++i)
	{
		_myIndicators[i]->updateSignalLevel(20 * log10(_currentInputValues[i]));
		_myIndicators[i]->repaint(_myIndicators[i]->getRepaintArea());
	}
}
void MvTabCalibration::buttonClicked(Button* button)
{
	if (button == testLvlButton)
	{
		listeners.call(&MvTabCalibration::Listener::buttonTriggered, 3);
	}

}
void MvTabCalibration::sliderValueChanged(Slider* slider)
{
	_globalCalibrationLevel = slider->getValue();
	for (int i = 0; i < _myIndicators.size(); ++i)
	{
		_myIndicators[i]->setCurrentCalibrationLevel(slider->getValue());
	}
}
void MvTabCalibration::globalCalibrationLevelChanged(float vewValue)
{
	_globalCalibrationLevel = vewValue;
	calibrationSlider->setValue(vewValue);
}
void MvTabCalibration::giveMeGlobalCalibrationLevel(MvMicrophoneIndicator *indicator)
{
	indicator->setCurrentCalibrationLevel(_globalCalibrationLevel);
}

//==== Listener ====
void MvTabCalibration::addListener(Listener* listener)
{
	listeners.add(listener);
}
void MvTabCalibration::removeListener(Listener* listener)
{
	listeners.remove(listener);
}

#pragma endregion

#pragma region Private

void MvTabCalibration::updateCalibrationSliderVisibility()
{
	if (_myIndicators.size() == 0)
		calibrationSlider->setVisible(false);
	else
		calibrationSlider->setVisible(true);
}
void MvTabCalibration::fillFadeColours()
{
	fadeColours[0][0] = 0xffff0000;
	fadeColours[0][1] = 0xfffb7b7b;
	fadeColours[1][0] = 0xffff5c00;
	fadeColours[1][1] = 0xfffba37b;
	fadeColours[2][0] = 0xffffa000;
	fadeColours[2][1] = 0xfffbc67b;
	fadeColours[3][0] = 0xffffff00;
	fadeColours[3][1] = 0xffffff7b;
	fadeColours[4][0] = 0xffa7ff00;
	fadeColours[4][1] = 0xffd1ff7b;
	fadeColours[5][0] = 0xff4bff00;
	fadeColours[5][1] = 0xffa9ff7b;
	fadeColours[6][0] = 0xff00ff00;
	fadeColours[6][1] = 0xff7bff7b;
	fadeColours[7][0] = 0xff00ff58;
	fadeColours[7][1] = 0xff7bffa5;
	fadeColours[8][0] = 0xff00ffa7;
	fadeColours[8][1] = 0xff7bffc4;
	fadeColours[9][0] = 0xff00ffff;
	fadeColours[9][1] = 0xff7bffff;
	fadeColours[10][0] = 0xff00abff;
	fadeColours[10][1] = 0xff7bd1ff;
	fadeColours[11][0] = 0xff005cff;
	fadeColours[11][1] = 0xff7baeff;
	fadeColours[12][0] = 0xff0000ff;
	fadeColours[12][1] = 0xff7b7bff;
	fadeColours[13][0] = 0xff5800ff;
	fadeColours[13][1] = 0xffa57bff;
	fadeColours[14][0] = 0xff9e00ff;
	fadeColours[14][1] = 0xffc87bff;
	fadeColours[15][0] = 0xffff00ff;
	fadeColours[15][1] = 0xffff7bff;

}

#pragma endregion
