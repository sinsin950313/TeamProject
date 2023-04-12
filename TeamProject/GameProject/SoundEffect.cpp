#include "SoundEffect.h"

namespace SSB
{
	void SoundEffect::SetSound(Sound* sound, bool isLoop)
	{
		_sound = sound;
		_isLoop = isLoop;
		_isPlayed = false;

		if (_sound != nullptr)
		{
			_sound->VolumeDown(100);
			_sound->Play(_isLoop);
			_sound->Stop();
		}
	}
	void SoundEffect::Play()
	{
		if (_sound != nullptr)
		{
			if (!_isPlayed)
			{
				_sound->Play(_isLoop);
				_isPlayed = true;
			}
			else
			{
				if (!_isLoop)
				{
					_sound->Play();
				}
			}
		}
	}
	void SoundEffect::Stop()
	{
		if (_isLoop)
		{
			_sound->Stop();
		}
	}
}
