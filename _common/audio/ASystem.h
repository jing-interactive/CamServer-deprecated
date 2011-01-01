#pragma once

//A stands for artificial here/Audio : )
//AKA

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <vector>
#include <assert.h>

#pragma warning( disable: 4244 )
#pragma warning( disable: 4996 )

typedef std::vector<FMOD::Sound*> vec_sound;

void  ERRCHECK(FMOD_RESULT result);

static FMOD_RESULT result;

#define CHECK_RUN(func)\
	result = func;\
	ERRCHECK(result);\

struct ISystem//normal system
{	
	FMOD::System* _sys;
	vec_sound _snds;

	ISystem(){

		CHECK_RUN(FMOD::System_Create(&_sys));	
		unsigned int version;
		CHECK_RUN(_sys->getVersion(&version));
	}


	~ISystem(){
		CHECK_RUN(_sys->close());
		CHECK_RUN(_sys->release());
	}

	void getSpectrum(float* spec, int count)//count must be a power of 2. (ie 128/256/512 etc). Min = 64. Max = 8192. 
	{
		CHECK_RUN(_sys->getSpectrum(spec, count, 0, FMOD_DSP_FFT_WINDOW_TRIANGLE));
	}
};

struct ASystem : public ISystem//normal system
{
	ASystem()
	{
		CHECK_RUN(_sys->init(100, FMOD_INIT_NORMAL, 0));
	}
};

struct BSystem : public ISystem//silent & offline
{
	BSystem()
	{
		CHECK_RUN(_sys->setOutput(FMOD_OUTPUTTYPE_NOSOUND_NRT));
		CHECK_RUN(_sys->init(2, FMOD_INIT_NORMAL, 0));
	}
};


extern ASystem the_ASystem;
extern BSystem the_BSystem;
#define ASys the_ASystem._sys
#define BSys the_BSystem._sys

struct vSound
{
	FMOD::Sound *_snd; 
	FMOD::Channel* _chnl;

	enum
	{
		mode_manual = FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_HARDWARE | FMOD_CREATESTREAM,
		mode_loadfile = FMOD_2D | FMOD_SOFTWARE | FMOD_LOOP_OFF,
		mode_mic =  FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_SOFTWARE,
	};

	vSound(char* filename)//from file
	{
		CHECK_RUN(ASys->createStream(filename, mode_loadfile, NULL, &_snd));
	}

	vSound(char* filename, void* ptr)//offline fft
	{
		CHECK_RUN(BSys->createSound(filename, mode_loadfile, NULL, &_snd));
	}

	vSound(float buffer_seconds = 0.5);//from mic

	vSound(int num_channel, int seconds, FMOD_SOUND_PCMREADCALLBACK cb);//diy

	void play(bool isPlay = true)
	{
		CHECK_RUN(ASys->playSound(FMOD_CHANNEL_FREE, _snd, !isPlay, &_chnl));
	}

	void getSpectrum(float* spec, int count)//count must be a power of 2. (ie 128/256/512 etc). Min = 64. Max = 8192. 
	{
		CHECK_RUN(_chnl->getSpectrum(spec, count, 0, FMOD_DSP_FFT_WINDOW_TRIANGLE));
	}

	void saveWav(char* filename);

};