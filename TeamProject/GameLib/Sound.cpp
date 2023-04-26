#include "Sound.h"
#include <tchar.h>

bool		Sound::Load(FMOD::System* pSystem, std::wstring filename)
{
	m_pSystem = pSystem;
	FMOD_RESULT hr = m_pSystem->createSound(
		wtm(filename).c_str(), FMOD_DEFAULT, nullptr, &m_pSound);
	if (hr == FMOD_OK)
	{
		m_pSound->getLength(&m_TotalTime, FMOD_TIMEUNIT_MS);
	}
	return true;
}

bool Sound::Play(bool bLoop)
{
	if (IsPlay() == false)
	{
		FMOD_RESULT hr = m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
		if (hr == FMOD_OK)
		{
			//m_fVolume = 0.5f;
			m_pChannel->setVolume(m_fVolume);
			SetLoop(bLoop);
		}
	}
	return true;
}
bool Sound::PlayEffect()
{
	FMOD_RESULT hr = m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	if (hr == FMOD_OK)
	{
		//m_fVolume = 0.5f;
		m_pChannel->setVolume(m_fVolume);
		SetLoop();
	}
	return true;
}
void Sound::Stop()
{
	m_pChannel->stop();
}
void Sound::SetLoop(bool bLoop)
{
	if (bLoop)
		m_pSound->setMode(FMOD_LOOP_NORMAL);
	else
		m_pSound->setMode(FMOD_LOOP_OFF);
}
bool Sound::IsPlay()
{
	bool playing = false;
	if (m_pChannel != nullptr)
	{
		m_pChannel->isPlaying(&playing);
	}
	return playing;
}
void Sound::Paused()
{
	if (m_pChannel == nullptr) return;
	bool paused;
	m_pChannel->getPaused(&paused);
	m_pChannel->setPaused(!paused);
}
void Sound::VolumeUp(float fVolume)
{
	m_fVolume += fVolume;
	m_fVolume = min(1.0f, m_fVolume);
	m_pChannel->setVolume(m_fVolume);
}
void Sound::VolumeDown(float fVolume)
{
	m_fVolume -= fVolume;
	m_fVolume = max(0.0f, m_fVolume);
	m_pChannel->setVolume(m_fVolume);
}

void Sound::VolumeSetCompMin(float fVolume)
{
	m_fVolume = min(fVolume, m_fVolume);
	m_pChannel->setVolume(m_fVolume);
}

void Sound::VolumeSetCompMax(float fVolume)
{
	m_fVolume = max(fVolume, m_fVolume);
	m_pChannel->setVolume(m_fVolume);
}

void Sound::VolumeSet(float fVolume)
{
	m_fVolume = fVolume;
	m_pChannel->setVolume(m_fVolume);
}

bool    Sound::Init()
{	
	return true;
}
bool    Sound::Frame()
{
	m_pSystem->update();
	if (m_pChannel)
	{
		unsigned int ms = 0;
		m_pChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
		TCHAR szBuffer[256] = { 0, };
		_stprintf_s(szBuffer, _T("경과시간[%02d:%02d:%02d], 파일크기[%02d:%02d:%02d]"),
			ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 60,
			m_TotalTime / 1000 / 60,
			m_TotalTime / 1000 % 60,
			m_TotalTime / 10 % 60);
		m_szBuffer = szBuffer;
	}

	return true;
}
bool    Sound::Render()
{
	return true;
}
bool    Sound::Release()
{
	if (m_pSound) m_pSound->release();
	return true;
}