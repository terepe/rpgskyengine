#include "Sound.h"
#if defined(_DEBUG)
#pragma comment(lib, "fmodvc.lib")
#else
#pragma comment(lib, "fmodvc.lib")
#endif
#include "fmod.h"
//#include "Log.h"
#include "fmod_errors.h"
#include "windows.h"
/*
Oscilliscope stuff
*/

FSOUND_DSPUNIT      *OscUnit;
static signed short *OscBuffer;
static int           OscBlock;

CFmodSound& GetSound()
{
	static CFmodSound g_Sound;
	return g_Sound;
}

CFmodSound::CFmodSound(void)
{
	FSOUND_Init(44100, 32, 0);
	m_Music.mod = NULL;
	m_Music.stream = NULL;
	m_Music.nNowVolume = 0;
	m_Music.nVolume = 255;
	m_nBGMVolume = 255;
	m_nSeVolume = 255;

	DSP_Ready = false;
	OscUnit = NULL;
	OscBuffer = NULL;
	OscBlock = 0;

	InitDSP();
}

CFmodSound::~CFmodSound(void)
{
	CloseDSP();

	if (m_Music.mod)
	{
		FMUSIC_FreeSong(m_Music.mod);
		m_Music.mod = NULL;
	}
	if (m_Music.stream)
	{
		FSOUND_Stream_Close(m_Music.stream);
	}
	FSOUND_Close();
}

void CFmodSound::FrameMove(float fElapsedTime)
{
	m_Music.nNowVolume += int((m_Music.nVolume*1000 - m_Music.nNowVolume)*fElapsedTime);
	// 
	if (m_Music.mod)
	{
		FMUSIC_SetMasterVolume(m_Music.mod, m_Music.nNowVolume/1000);
	}
	else if (m_Music.stream)
	{
		FSOUND_SetVolume(m_Music.channel, m_Music.nNowVolume/1000);
	}
}

bool CFmodSound::LoadMusic(const std::string& strFilename)
{
	// 先清空
	if (m_Music.mod)
	{
		FMUSIC_FreeSong(m_Music.mod);
		m_Music.mod = NULL;
		m_Music.stream = NULL;
	}
	// 用MOD载入
	m_Music.mod=FMUSIC_LoadSong(strFilename.c_str());

	// 不能载入就用流载入
	if (!m_Music.mod)
	{
		m_Music.stream = FSOUND_Stream_Open(strFilename.c_str(), FSOUND_NORMAL | FSOUND_2D | FSOUND_MPEGACCURATE | FSOUND_NONBLOCKING, 0, 0);
	}
	if (!m_Music.mod && !m_Music.stream)
	{
		//MessageBoxA(0, FMOD_ErrorString(FSOUND_GetError()), "Loading a song", MB_ICONHAND|MB_OK|MB_SYSTEMMODAL);
		return false;
	}
	return true;
}

void CFmodSound::SetMusicVolume(int nVolume)
{
	m_Music.nVolume = nVolume*m_nBGMVolume/255;
}

void CFmodSound::PlayMusic(bool bLoop, int nVolume)
{
	SetMusicVolume(nVolume);
	if (m_Music.mod)
	{
		FMUSIC_SetLooping(m_Music.mod, bLoop);
		FMUSIC_PlaySong(m_Music.mod);
	}
	else if (m_Music.stream)
	{
		if (FSOUND_Stream_GetOpenState(m_Music.stream) != 0)
		{
			while (FSOUND_Stream_GetOpenState(m_Music.stream) == -2)
			{
				Sleep(100);
			}
		}
		FSOUND_Stream_SetMode(m_Music.stream, bLoop ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);
		m_Music.channel = FSOUND_Stream_Play(FSOUND_FREE, m_Music.stream);
	}
}

void CFmodSound::PlayBGM(const std::string& strFilename, bool bLoop, int nVolume)
{
	if (LoadMusic(strFilename))
	{
		PlayMusic(bLoop, nVolume);
	}
}

void CFmodSound::PlayMusicEx(const std::string& strFilename, bool bLoop, int nVolume)
{
}

void CFmodSound::ReplayMusic()
{
	if (m_Music.mod)
	{
		FMUSIC_SetPaused (m_Music.mod, false);
	}
	else if (m_Music.stream)
	{
		//FSOUND_Stream_Stop(m_mod);
	}
}

void CFmodSound::PausedMusic()
{
	if (m_Music.mod)
	{
		FMUSIC_SetPaused(m_Music.mod, true);
	}
	else if (m_Music.stream)
	{
		// FSOUND_Stream_Paused(m_mod, true);
	}
}

void CFmodSound::StopMusic()
{
	if (m_Music.mod)
	{
		FMUSIC_StopSong(m_Music.mod);
	}
	else if (m_Music.stream)
	{
		FSOUND_Stream_Stop(m_Music.stream);
	}
}

signed short* CFmodSound::GetOscBuffer()
{
	if (OscBuffer)
	{
		//int count, count2, offset;
		int offset;
		//float xoff, step;
		signed short *src;
		/*
		The next pcmblock (Oscblock + 1) is the one that is audible.
		*/
		offset = (OscBlock + 1) * FSOUND_DSP_GetBufferLength();
		if (offset >= FSOUND_DSP_GetBufferLengthTotal())
		{
			offset -= FSOUND_DSP_GetBufferLengthTotal();
		}

		src = &OscBuffer[offset];
		return src;
	}
	return 0;
}

int CFmodSound::GetBufferLength()
{
	return FSOUND_DSP_GetBufferLength();
}

void CFmodSound::InitDSP()
{
	int bytesperoutputsample;
	int mixertype = FSOUND_GetMixer();

	DSP_Ready = false;

	if (mixertype == FSOUND_MIXER_MMXP5 || mixertype == FSOUND_MIXER_MMXP6 || mixertype == FSOUND_MIXER_QUALITY_MMXP5 || mixertype == FSOUND_MIXER_QUALITY_MMXP6)
	{
		bytesperoutputsample = 4;   // 16bit stereo
	}
	else
	{
		bytesperoutputsample = 8;   // 32bit stereo
	}

	/*
	Initalize and create lowpass buffer and DSP unit 
	*/
	//LowPass_Init();
	//LowPassBuffer = calloc(FSOUND_DSP_GetBufferLength()+256, bytesperoutputsample);
	//LowPass_Update(LowPassResonance, LowPassCutoffFrequency, outputfreq);
	//LowPassUnit = FSOUND_DSP_Create(&LowPassCallback,	FSOUND_DSP_DEFAULTPRIORITY_USER+1,	LowPassBuffer);

	/*
	Create buffer and dsp unit for echo effect
	*/
	//EchoLen		= MAXECHOLEN;				/* 500ms */
	//EchoBuffer  = calloc(EchoLen, 4);	    /* The echo buff is always 16bit stereo int regardless of the mixer format, so * 4 */
	//EchoOffset	= 0;
	//EchoUnit	= FSOUND_DSP_Create(&EchoCallback, FSOUND_DSP_DEFAULTPRIORITY_USER+2, EchoBuffer);

	/*
	Create buffer and dsp unit for oscilliscope.
	*/
	OscUnit	    = FSOUND_DSP_Create(&OscCallback, FSOUND_DSP_DEFAULTPRIORITY_USER+3,	0);
	OscBuffer   = (short*)calloc(FSOUND_DSP_GetBufferLengthTotal() + 16, 2); /* *2 for mono 16bit buffer */

	 FSOUND_DSP_SetActive(OscUnit, true);
	/*
	Initialize reverb stuff
	*/
	//Reverb_Init();

	DSP_Ready = true;
}

void CFmodSound::CloseDSP()
{
	DSP_Ready = false;

	//if (LowPassUnit)
	//{
	//	FSOUND_DSP_Free(LowPassUnit);
	//}
	//LowPassUnit = NULL;

	//if (EchoUnit)
	//{
	//	FSOUND_DSP_Free(EchoUnit);
	//}
	//EchoUnit = NULL;

	if (OscUnit)
	{
		FSOUND_DSP_Free(OscUnit);
	}
	OscUnit = NULL;

	/*
	Free buffers
	*/
	//if (LowPassBuffer)
	//{
	//	free(LowPassBuffer);
	//}
	//LowPassBuffer = NULL;

	//if (EchoBuffer)
	//{
	//	free(EchoBuffer);
	//}
	//EchoBuffer = NULL;

	if (OscBuffer)
	{
		free(OscBuffer);
	}
	OscBuffer = NULL;

	//Reverb_Close();
	//LowPass_Close();
}

void * F_CALLBACKAPI CFmodSound::OscCallback(void *originalbuffer, void *newbuffer, int length, void *userdata)
{
	int             mixertype = FSOUND_GetMixer();
	int             count;
	int             totalblocks; 
	signed short    *dest;

	totalblocks = FSOUND_DSP_GetBufferLengthTotal() / FSOUND_DSP_GetBufferLength(); 

	/*
	Convert and downmix into a mono short int buffer.
	*/

	dest = &OscBuffer[OscBlock * FSOUND_DSP_GetBufferLength()];

	if (mixertype == FSOUND_MIXER_QUALITY_FPU)
	{
		float *src = (float *)newbuffer;

		for (count=0; count < length; count++)
		{
			dest[count] = (signed short)((src[count << 1] + src[(count << 1) + 1]) * 0.5f);
		}
	}
	else if (mixertype == FSOUND_MIXER_MMXP5 || mixertype == FSOUND_MIXER_MMXP6 || mixertype == FSOUND_MIXER_QUALITY_MMXP5 || mixertype == FSOUND_MIXER_QUALITY_MMXP6)
	{
		signed short *src = (signed short *)newbuffer;

		for (count=0; count < length; count++)
		{
			dest[count] = (signed short)(((int)src[count << 1] + (int)src[(count << 1) + 1]) >> 1);
		}
	}
	else
	{
		signed int *src = (signed int *)newbuffer;

		for (count=0; count < length; count++)
		{
			dest[count] = (signed short)((src[count << 1] + src[(count << 1) + 1]) >> 1);
		}
	}

	OscBlock++;
	if (OscBlock >= totalblocks)
	{
		OscBlock = 0;
	}

	return newbuffer;
}


void CFmodSound::LoadSound(const std::string& strFilename)
{

}
void CFmodSound::playSound(const std::string& strFilename)
{

}
void CFmodSound::Play3DSound(const std::string& strFilename, float x, float y, float fRadius, int nVolume)
{

}
void CFmodSound::PlaySoundEx(const std::string& strFilename, bool bLoop, int nVolume)
{

}

void CFmodSound::ReplaySound(int nChannel)
{

}
void CFmodSound::PausedSound(int nChannel)
{

}
void CFmodSound::StopSound(int nChannel)
{

}