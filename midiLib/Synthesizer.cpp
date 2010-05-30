#include "Synthesizer.h"

#pragma comment(lib,"winmm.lib")

Synthesizer theSynthesizer;

DWORD Synthesizer::MidiOutMessage(int iStatus, int iChannel,
					 int iData1,int iData2){
	//        
	DWORD dwMessage = iStatus | iChannel | (iData1 << 8) | (iData2 << 16);

	return midiOutShortMsg (_hMidiOut, dwMessage);	
}  


Synthesizer::Synthesizer(){
	//
	if (midiOutOpen (&_hMidiOut, MIDIMAPPER, 0, 0, 0)) {
		//
		MessageBeep (MB_ICONEXCLAMATION);
		MessageBox(NULL, TEXT("Cannot open MIDI output device!"), NULL, MB_OK);		
	}
	/*
	if (midiInOpen (&_hMidiIn, MIDIMAPPER, 0, 0, 0)) {
		//
		MessageBeep (MB_ICONEXCLAMATION);
		MessageBox(NULL, "Cannot open MIDI input device!", NULL, MB_OK);		
	}
	*/
	unsigned long   i;

	MIDIOUTCAPS     moc;

	/* Get the number of MIDI Out devices in this  computer */
	_num_output = midiOutGetNumDevs();

	/* Go through all of those devices, displaying their names */
	for (i = 0; i < _num_output; i++)
	{
		/* Get info about the next device */
		if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS))){
			//
			_output_device_names.push_back(moc.szPname);
		}
	}

	MIDIINCAPS     mic;

	/* Get the number of MIDI In devices in this computer */
	_num_input = midiInGetNumDevs();

	/* Go through all of those devices, displaying their names */
	for (i = 0; i < _num_input; i++)
	{
		/* Get info about the next device */
		if (!midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS))){
			//
			_input_device_names.push_back(moc.szPname);
		}
	}

}


Synthesizer::~Synthesizer(){
	//
	midiOutReset (_hMidiOut) ;
	midiOutClose (_hMidiOut) ;
}


int Synthesizer::getValidChannel(void){
	//
	static int indexChannel = 0;
	int channel;
	if (indexChannel < MAX_CHANNEL){
		channel = indexChannel;
	}
	else{
		channel = -1;//invalid channel
	}
	indexChannel++;
	return(channel);
}


int Synthesizer::getNumOutputDevice(void){
	//
	return(_num_output);
}

int Synthesizer::getNumInputDevice(void){
	//
	return(_num_input);
}

const TCHAR* Synthesizer::getInputDeviceName(unsigned int idx){
	//
	if (idx >= _num_input)
		return TEXT("不存在该输入设备");
	else
		return _input_device_names[idx].c_str();
}



const TCHAR* Synthesizer::getOutputDeviceName(unsigned int idx){
	//
	if (idx >= _num_output)
		return TEXT("不存在该输出设备");
	else
		return _output_device_names[idx].c_str();
}