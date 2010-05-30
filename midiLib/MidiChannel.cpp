#include "MidiChannel.h"


MidiChannel::MidiChannel(const TCHAR* inst_name)
{	
	int iInstrument = Instrument::getIndexFromName(inst_name);

	_channel = theSynthesizer.getValidChannel();
	setPatch(iInstrument);
}

MidiChannel::MidiChannel(int iInstrument){
	//
	_channel = theSynthesizer.getValidChannel();
	setPatch(iInstrument);
}


DWORD MidiChannel::noteOff (int iNote, int iVel){   
	//
	return theSynthesizer.MidiOutMessage(0x080, _channel, iNote, iVel) ;
        
}
        

DWORD MidiChannel::noteOff (){   
	//
	return theSynthesizer.MidiOutMessage(0x080, _channel, _note, _vel) ;        
}     

DWORD MidiChannel::noteOn ( int iNote, int iVel){
	//
	_note = iNote;
	_vel = iVel;
	return theSynthesizer.MidiOutMessage(0x090, _channel, iNote, iVel) ;      
}

DWORD MidiChannel::setPatch ( int iInstrument){
	// 
	return theSynthesizer.MidiOutMessage(0x0C0, _channel, iInstrument, 0) ;        
}
        

DWORD MidiChannel::pitchBend (int iBend){
	//
	return theSynthesizer.MidiOutMessage(0x0E0, _channel, iBend & 0x7F, iBend >> 7) ;        
}