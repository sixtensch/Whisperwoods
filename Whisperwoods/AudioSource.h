#pragma once
#include "GameObject.h"
#include "Sound.h"


class AudioSource : public GameObject
{
	FMOD::Sound* m_currentSound;
	FMOD::Channel* m_channel;
public:
	AudioSource(Vec3 pos, float vol, float pch, float minr, float maxr, std::string sound);
	AudioSource(Vec3 pos, float vol, float pch, float minr, float maxr, FMOD::Sound* snd);
	AudioSource(Vec3 pos, float vol, float pch, float minr, float maxr, FMOD::Sound* snd, FMOD::Channel* ch);

	float mix2d3d;
	float volume;
	float pitch;
	float minRange;
	float maxRange;
	bool loop;
	void Play();
	void Stop();
	bool IsPlaying();
	void SetSound(FMOD::Sound* sound);
	void SetVolume(float volume);
	void SetPitch(float pitch);
	
	void Update(float delta_time) override;
};