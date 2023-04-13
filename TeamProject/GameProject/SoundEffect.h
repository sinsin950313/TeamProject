#pragma once

#include "Sound.h"

namespace SSB
{
	class SoundEffect
	{
	private:
		Sound* _sound = nullptr;
		bool _isPlayed = false;
		bool _isLoop = false;

	public:
		void SetSound(Sound* sound, bool isLoop);
		void Play();
		void Stop();
	};
}
