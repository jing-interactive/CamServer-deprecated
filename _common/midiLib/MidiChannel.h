#pragma once

#include "Synthesizer.h"
#include "Instrument.h"
#include "KeyNote.h"

class MidiChannel{
	//
public:
	MidiChannel(int iInstrument);
	MidiChannel(const TCHAR* inst_name);

	DWORD noteOff(int iNote, int iVel = 0);
	DWORD noteOff();
	DWORD noteOn(int iNote, int iVel);       
	DWORD setPatch(int iInstrument);        
	DWORD pitchBend(int iBend);
     
private:
	int _note,_vel;
	int _channel;
};