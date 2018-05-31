

#include "../JuceLibraryCode/JuceHeader.h"

class MvMeasuringThread  : public ThreadWithProgressWindow
{
public:
    MvMeasuringThread()
        : ThreadWithProgressWindow ("Measuring in progress", true, true)
    {
        setStatusMessage ("Getting ready...");
    }

    void run() override
    {
        setProgress (-1.0); 
        setStatusMessage ("Please wait...");
        
		while (!shouldStop)
		{
			wait(200);
			if (threadShouldExit())
				       return;
		}
    }

    // This method gets called on the message thread once our thread has finished..
    void threadComplete (bool userPressedCancel) override
    {
        if (userPressedCancel)
        {
			listeners.call(&Listener::measuringThreadCallback, true);
			AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                              "Measuring stopped!",
                                              "Process cancelled!");
        }
        else
        {
            // thread finished normally..
            //AlertWindow::showMessageBoxAsync (AlertWindow::InfoIcon,
            //                                  "Progress window",
            //                                  "Thread finished ok!");
        }

        // ..and clean up by deleting our thread object..
        delete this;
    }
	bool shouldStop = false;

	class Listener
	{
	public:
		virtual void measuringThreadCallback(bool processCancelled) = 0;
	};
	void addListener(Listener* listener) { listeners.add(listener); };
	void removeListener(Listener* listener) { listeners.remove(listener); };

private:
	ListenerList<Listener> listeners;

};