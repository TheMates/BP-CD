#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "MvLookAndFeel.h"
#include "Compute.h"
#include "LinSweep.h"
#include "LogSweep.h"
#include "MLS.h"
#include "MvTabCalibration.h"
#include "MvTabDeviceConfig.h"
#include "MvTabMeasuring.h"
#include "MvMeasuringThread.h"
#include <iostream>
#include <fstream>


//==============================================================================
enum MenuTopID
{
	exportMenu,
	thirdMenu
};
enum MenuExportItemID
{
	exportInputWav = 1,
	exportInputCsv,
	exportRecordedWav,
	exportRecordedCsv,
	exportIRWav,
	exportIRCsv,
	exportLundeby,
	exportSchroeder
};
enum MenuInfoItemID
{
	English = 1,
	Czech
};

class MainComponent   : public AudioAppComponent, public MvTabMeasuring::Listener, public MenuBarModel, public Timer, public MvTabCalibration::Listener,
	public MvMeasuringThread::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

	//==== Callbacks ====
	void buttonTriggered(int buttonId) override;
	void timerCallback() override;
	void measuringThreadCallback(bool processCancelled) override;

	//==== Other methods - Menu ====
	StringArray getMenuBarNames() override;
	PopupMenu getMenuForIndex(int menuIndex, const String& menuName) override;
	void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

	//==== public variables to read from or use ====
	AudioBuffer<float> recordedData{};
	AudioBuffer<float> irData{};
	AudioBuffer<float> lundebyData{};
	AudioBuffer<float> schroederData{};

	Array<LundebyReturn> currentLundebyAdditionalData;
	Array<SchroederReturn> currentSchroederAdditionalData;
	


private:
    //==== My components ====
	ScopedPointer< AudioDeviceSelectorComponent> audioSetupComp;
	ScopedPointer<MenuBarComponent> menuBar;
	ScopedPointer<MvMeasuringThread> measuringThread;
	ScopedPointer<MvLookAndFeel> MvLaF;
	//==== Tabs ====
	ScopedPointer<MvTabCalibration> myCalibrationTab;
	ScopedPointer<MvTabDeviceConfig> myDeviceConfigTab;
	ScopedPointer<MvTabMeasuring> myMeasuringTab;
	//==== TabbedComponent ====
	ScopedPointer<TabbedComponent> myTabs;


	//==== My variables ====
	Array<int> activeInputs, activeOutputs;
	//==== Input signal====
	AudioBuffer<float> inputSignal;	
	int inputPosition = 0, currentRepetition = 1,currentWaitingLoop = 0;
	//==== Bool flags ====
	bool isRecorded = false, isPlaying = false, markToStopPlaying = false, markToStopRecording = false, isWaitingForNextMeasurement = false;
	

	//==== My methods =====
	/**
	Initializes all components, adds listeners.
	*/
	void initializeAllMyComponents();
	/**
	Makes visible all components.
	*/
	void setUpGUI();
	/**
	Returns how many input channels are switched on.
	*/
	int getNumberOfBufferChannels();
	/**
	Reads signal parameters from text boxes and combo boxes and creates an input signal. It is stored
	in memory in AudioBuffer<float> and only has one channel.
	When the signal is created, the tail is appended with as many 0.0 samples, as the device latency is.
	After recording, it gets trimmed.
	*/
	void generateInputSignal();
	/**
	Returns original length of input signal before appending sapmles due to latency.
	*/
	int getOriginalInputLength();
	/**
	Method, that reduces the error of spectral division of 2 noisy signals.
	There is very high level of fs/2 error, so what i do is, that i shift the original signal one forward, sum it with nonshifted.
	Than shift the shifted one forward and sum with previous. The error gets reduced almost 100 times, changing the character
	of original impulse response minimally.
	*/
	void doTheMagic();
	/**
	Resizes AudioBuffer<float> with given parameters.
	P:
	@param &bufferToResize - AudioBuffer<float>, reference to AudioBuffer to be trimmed.
	@param startSample - int, first sample of new trimmed array.
	@param endSample - int, last sample that will be copied from the old array.
	*/
	void arrayResize(AudioBuffer<float> &bufferToResize,const int startSample, const int endSample);
	/**
	Writes data to .wav or .csv file.
	P:
	@param &dataToSave - AudioBuffer<float>, reference to the buffer to read from.
	@param isWavFormat - bool, specifies whether to write .wav or .csv file.
	@param dataSpecifier - MenuExportItemID, specifies data to write
	*/
	void writeDataToFile(AudioBuffer<float> &dataToSave, bool isWavFormat, MenuExportItemID dataSpecifier);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
