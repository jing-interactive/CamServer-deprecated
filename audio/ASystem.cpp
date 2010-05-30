#include "ASystem.h"
#include <windows.h>

#pragma comment (lib, "fmodex_vc.lib")

void  ERRCHECK(FMOD_RESULT result){
	//
	static char buffer[80];
	if (result != FMOD_OK){
		sprintf(buffer,"FMOD error! (%d) %s", result, FMOD_ErrorString(result) );
		::MessageBoxA(NULL, buffer, "FMOD Error",MB_OK);
		exit(-1);
	}
}

ASystem the_ASystem;
BSystem the_BSystem;



void vSound::saveWav(char* filename)
{
	FILE *fp;
	int             channels, bits;
	float           rate;
	void           *ptr1, *ptr2;
	unsigned int    lenbytes, len1, len2;

	_snd->getFormat  (0, 0, &channels, &bits);
	_snd->getDefaults(&rate, 0, 0, 0);
	_snd->getLength  (&lenbytes, FMOD_TIMEUNIT_PCMBYTES);

	{
#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
#pragma pack(1)
#endif

		/*
		WAV Structures
		*/
		typedef struct
		{
			signed char id[4];
			int 		size;
		} RiffChunk;

		struct
		{
			RiffChunk       chunk           ;
			unsigned short	wFormatTag      ;    /* format type  */
			unsigned short	nChannels       ;    /* number of channels (i.e. mono, stereo...)  */
			unsigned int	nSamplesPerSec  ;    /* sample rate  */
			unsigned int	nAvgBytesPerSec ;    /* for buffer estimation  */
			unsigned short	nBlockAlign     ;    /* block size of data  */
			unsigned short	wBitsPerSample  ;    /* number of bits per sample of mono data */
		} FmtChunk  = { {{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits } ;

		struct
		{
			RiffChunk   chunk;
		} DataChunk = { {{'d','a','t','a'}, lenbytes } };

		struct
		{
			RiffChunk   chunk;
			signed char rifftype[4];
		} WavHeader = { {{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + lenbytes }, {'W','A','V','E'} };

#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
#pragma pack()
#endif

		fp = fopen(filename, "wb");

		/*
		Write out the WAV header.
		*/
		fwrite(&WavHeader, sizeof(WavHeader), 1, fp);
		fwrite(&FmtChunk, sizeof(FmtChunk), 1, fp);
		fwrite(&DataChunk, sizeof(DataChunk), 1, fp);

		/*
		Lock the sound to get access to the raw data.
		*/
		result =  _snd->lock(0, lenbytes, &ptr1, &ptr2, &len1, &len2);
		ERRCHECK(result);

		/*
		Write it to disk.
		*/
		fwrite(ptr1, len1, 1, fp);

		/*
		Unlock the sound to allow FMOD to use it again.
		*/
		_snd->unlock(ptr1, ptr2, len1, len2);

		fclose(fp);
	}
}


vSound::vSound(float buffer_seconds)//from mic
{
	FMOD_OUTPUTTYPE output;
	FMOD_CREATESOUNDEXINFO exinfo;

	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.length           = buffer_seconds *44100 * 2 * sizeof(short);
	exinfo.numchannels      = 2;
	exinfo.defaultfrequency = 44100;
	exinfo.format           = FMOD_SOUND_FORMAT_PCM16;

	CHECK_RUN(ASys->createSound(NULL, mode_mic, &exinfo, &_snd))
		CHECK_RUN(ASys->recordStart(0,_snd, true))

		ASys->getOutput(&output);
	if (output != FMOD_OUTPUTTYPE_ASIO)
	{
		::Sleep(100);
	}
}

vSound::vSound(int num_channel, int seconds, FMOD_SOUND_PCMREADCALLBACK cb)//diy
{
	FMOD_CREATESOUNDEXINFO  info;
	assert(num_channel >0 && seconds> 0);
	memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	info.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO);              /* required. */
	info.decodebuffersize  = 44100;                                       /* Chunk size of stream update in samples.  This will be the amount of data passed to the user callback. */
	info.length            = 44100 * num_channel * sizeof(signed short) * seconds; /* Length of PCM data in bytes of whole song (for Sound::getLength) */
	info.numchannels       = num_channel;                                    /* Number of channels in the sound. */
	info.defaultfrequency  = 44100;                                       /* Default playback rate of sound. */
	info.format            = FMOD_SOUND_FORMAT_PCM16;                     /* Data format of sound. */
	info.pcmreadcallback   = cb;                             /* User callback for reading. */
	info.pcmsetposcallback = NULL;                           /* User callback for seeking. */

	CHECK_RUN(ASys->createSound(0, mode_manual, &info, &_snd))
}
