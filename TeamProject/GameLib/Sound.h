#pragma once

#include "std.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment(lib, "fmod_vc.lib")

class Sound
{
public:
	std::wstring m_szName;
public:
	FMOD::System* m_pSystem = nullptr;
	FMOD::Sound* m_pSound = nullptr;
	FMOD::Channel* m_pChannel = nullptr;
	float	m_fVolume;
	unsigned int	m_TotalTime;
	std::wstring m_szBuffer;

public:
	bool Play(bool bLoop = false);
	bool PlayEffect();
	void Stop();
	void SetLoop(bool bLoop = false);
	bool IsPlay();
	void Paused();
	void VolumeUp(float fVolume = 0.1f);
	void VolumeDown(float fVolume = 0.1f);

public:
	virtual bool		Load(FMOD::System* pSystem, std::wstring filename);
	virtual bool		Init();
	virtual bool		Frame();
	virtual bool		Render();
	virtual bool		Release();
};

