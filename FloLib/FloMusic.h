#include <fmod.hpp>

#include <windows.h>
void  ERRCHECK(FMOD_RESULT result);


class CMusic{
//
public:
	//操作
	enum{ 
		FREQ_NORMAL = 44100,
		FREQ_MIN = 14100,
		FREQ_MAX = 84100,
	};
	CMusic();
	~CMusic();
	int LoadMusic(char* filename);//载入文件但处于暂停状态
	
	char* GetFileName(void){return m_chFileName;}//得到文件名

	//音量相关
	int SetVolume(float vol);
	int IncreaseVolume(void);
	int DecreaseVolume(void);
	float GetVolume(void){return m_fVolume;}
	void SetLoop(bool isLoop){
		channel->setLoopCount(isLoop? -1 : 0);
	}
	
	//播放进度相关
	int SetPosition(UINT Milliseconds);
	int SetPosition(UINT hours, UINT minutes, UINT seconds);
	int IncreasePosition(UINT delta = 2000);//一次增加2000 Milliseconds
	int DecreasePosition(UINT delta = 2000);//一次减少2000 Milliseconds

	UINT GetNumChannels(void){
		int numchannels;
		system->getSoftwareFormat(0, 0, &numchannels, 0, 0, 0);
		return(numchannels);
	}
	UINT GetPosition(void);//得到进度
	UINT GetLength(void){return m_nLength;}//得到文件总长度
	bool GetFinished(void);//是否当前文件已播放完毕

	//频率相关  暂时可以不用
 	int IncreaseFrequency(UINT delta = 2000);
 	int DecreaseFrequency(UINT delta = 2000);
	float GetFrequency(void);

	int SetMute(bool mute);//true 则静音  false则正常播放
	bool GetMute(void){return m_bMute;}//返回是否处于静音状态
	int SetPause(bool pause);//true则暂停
	bool GetPaused(void){return m_bPause;}//返回是否暂停	

	void TogglePlay(void){
		if(sound){
			m_bPause = !m_bPause;
			SetPause(m_bPause);
		}
	}
	void StopMusic(void);
	void getWaveData(float* buffer, int buffer_size, int index){
		//
		result =  system->getWaveData(buffer, buffer_size, index);
		ERRCHECK(result);
	}
	void getSpectrum(float* buffer, int buffer_size, int index,FMOD_DSP_FFT_WINDOW windowtype){
		//
		result =  system->getSpectrum(buffer, buffer_size, index,windowtype);
		ERRCHECK(result);
	}
	FMOD::Channel* getChannel();
	static FMOD::System* getSystem()
	{
		return system;
	}
	void update(){
		system->update();
	}
private:
	static int musicCount;
	static FMOD::System *system;
protected:
	FMOD_RESULT result;

	FMOD::Sound *sound;
	FMOD::Channel *channel;
	float m_fVolume;
	float m_fFrequency;
	int m_nStatus;
	char* m_chFileName;
	bool m_bPause;
	bool m_bMute;
	UINT m_nLength;
};