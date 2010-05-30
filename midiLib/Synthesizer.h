#pragma once

#include <windows.h>
#include <vector>
#include <string>

typedef std::vector<std::wstring> NameContainer;

class Synthesizer{
	//
	enum{
		MAX_CHANNEL = 16,
	};
public:
	Synthesizer();
	~Synthesizer();
	HMIDIIN getHMidiIn() const{return _hMidiIn;}
	HMIDIOUT getHMidiOut() const{return _hMidiOut;}	

	int getNumInputDevice(void);
	const TCHAR* getInputDeviceName(unsigned int  idx);
	int getNumOutputDevice(void);
	const TCHAR* getOutputDeviceName(unsigned int  idx);

	DWORD MidiOutMessage(int iStatus, int iChannel,
						 int iData1,int iData2);
	int getValidChannel(void);

private:

	NameContainer _input_device_names;	
	unsigned long  _num_input;

	NameContainer _output_device_names;	
	unsigned long  _num_output;
	HMIDIOUT _hMidiOut;
	HMIDIIN  _hMidiIn;
};


extern Synthesizer theSynthesizer;