#include "FloMusic.h"
#include <fmod_errors.h>
#include <stdio.h>

#pragma comment (lib, "fmodex_vc.lib")

FMOD::System* CMusic::system = NULL;
int CMusic::musicCount = 0;

void  ERRCHECK(FMOD_RESULT result){
	//
	static char buffer[80];
	if (result != FMOD_OK){
		sprintf(buffer,"FMOD error! (%d) %s", result, FMOD_ErrorString(result) );
		::MessageBox(NULL, buffer, "FMOD Error",MB_OK);
		exit(-1);
	}
}

bool CMusic::GetFinished(void){
	//
	bool IsNotFinished = false;
	channel->isPlaying(&IsNotFinished);
	return (!IsNotFinished);
}


int CMusic::SetPosition(UINT Milliseconds){
	//
	result = channel->setPosition(Milliseconds, FMOD_TIMEUNIT_MS);
	ERRCHECK(result);
	return(1);
}

int CMusic::SetPosition(UINT hours, UINT minutes, UINT seconds){
	UINT Milliseconds;
	Milliseconds = hours*60*60*1000 + minutes*60*1000 + seconds*1000;
	result = channel->setPosition(Milliseconds, FMOD_TIMEUNIT_MS);
	ERRCHECK(result);
	return(1);
}

int CMusic::IncreasePosition(UINT delta){
	//
	UINT ms;
	channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
	ms+=delta;
	channel->setPosition(ms, FMOD_TIMEUNIT_MS);
	return(1);
}	

int CMusic::DecreasePosition(UINT delta){
	//
	UINT ms;
	channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
	ms-=delta;
	channel->setPosition(ms, FMOD_TIMEUNIT_MS);
	return(1);
}

UINT CMusic::GetPosition(void){
	//
	UINT ms;
	channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
	return ms;
}

CMusic::CMusic(){
	//
	system = NULL;
	sound = NULL;
	channel = NULL;
	m_fVolume = 0.5f;
	m_nStatus = 0;

	if (musicCount == 0)
	{
		result = FMOD::System_Create(&system);		// Create the main system object.
		ERRCHECK(result);

		result = system->init(100, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
		ERRCHECK(result);
	}
}

int CMusic::LoadMusic(char* filename){
	//
	m_chFileName = filename;
	StopMusic();

	result = system->createStream(filename, FMOD_SOFTWARE, 0, &sound);
	ERRCHECK(result);
	result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
	ERRCHECK(result);
	result = channel->setVolume(m_fVolume);		// Set the volume while it is paused.
	ERRCHECK(result);
	result = sound->getLength(&m_nLength, FMOD_TIMEUNIT_MS);
	ERRCHECK(result);
	result = channel->getFrequency(&m_fFrequency);
	ERRCHECK(result);
	m_bMute = false;
	m_bPause = false;
	return(1);
}

int CMusic::IncreaseFrequency(UINT delta){	
	result = channel->getFrequency(&m_fFrequency);
	m_fFrequency += delta;
	if (m_fFrequency > FREQ_MAX) m_fFrequency = FREQ_NORMAL;
	result = channel->setFrequency(m_fFrequency);		// Set the volume while it is paused.
	ERRCHECK(result);
	return(1);
}

int CMusic::DecreaseFrequency(UINT delta){	
	result = channel->getFrequency(&m_fFrequency);
	m_fFrequency -= delta;
	if (m_fFrequency < FREQ_MIN) m_fFrequency = FREQ_NORMAL;
	result = channel->setFrequency(m_fFrequency);		// Set the volume while it is paused.
	ERRCHECK(result);
	return(1);
}

 int CMusic::DecreaseVolume(void){
	m_fVolume -= 0.1f;
	if (m_fVolume <= 0.0f ) m_fVolume = 0.0f;
	result = channel->setVolume(m_fVolume);		// Set the volume while it is paused.
	ERRCHECK(result);
	return(1);
}

int CMusic::IncreaseVolume(void){
	m_fVolume += 0.1f;
	if (m_fVolume >= 1.0f) m_fVolume = 1.0f;
	result = channel->setVolume(m_fVolume);		// Set the volume while it is paused.
	ERRCHECK(result);
	return(1);
} 

int CMusic::SetMute(bool mute){
	//
	m_bMute = mute;
	result = channel->setMute(mute);
	ERRCHECK(result);
	return(1);
}


int CMusic::SetVolume(float vol){
	//
	m_fVolume = vol;
	result = channel->setVolume(vol);		// Set the volume while it is paused.
	ERRCHECK(result);
	return(1);
}

int CMusic::SetPause(bool pause){
	//
	m_bPause = pause;
	result = channel->setPaused(pause);
	ERRCHECK(result);
	return(1);
}

CMusic::~CMusic(){
	//	
	if(sound)
		sound->release();

	musicCount --;
	if(musicCount == 0 && system)
		system->release();

	
}

void CMusic::StopMusic(void){
	//
	if (channel)
		channel->stop();
	if (sound)
		sound->release();
	sound = NULL;
}

float CMusic::GetFrequency()
{
	float freq;
	channel->getFrequency(&freq);
	
	return freq;
}

FMOD::Channel* CMusic::getChannel()
{
	return channel;
}