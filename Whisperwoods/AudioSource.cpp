#include "Core.h"
#include "AudioSource.h"

AudioSource::AudioSource(Vec3 pos, float vol, float pch, float minr, float maxr, std::string sound)
{
	transform.position = pos;
	volume = vol;
	pitch = pch;
	minRange = minr;
	maxRange = maxr;
	m_currentSound = Sound::Get().LoadSound(sound);
	m_channel = nullptr;
}

AudioSource::AudioSource(Vec3 pos, float vol, float pch, float minr, float maxr, FMOD::Sound* snd)
{
	transform.position = pos;
	volume = vol;
	pitch = pch;
	minRange = minr;
	maxRange = maxr;
	m_currentSound = snd;
	m_channel = nullptr;
}

AudioSource::AudioSource(Vec3 pos, float vol, float pch, float minr, float maxr, FMOD::Sound* snd, FMOD::Channel* ch)
{
	transform.position = pos;
	volume = vol;
	pitch = pch;
	minRange = minr;
	maxRange = maxr;
	m_currentSound = snd;
	m_channel = ch;
}

void AudioSource::Play()
{
	m_channel->stop();
	Vec3 worldPos = transform.GetWorldPosition();

	FMOD_VECTOR pos = { worldPos.x, worldPos.y, 0};
	FMOD_VECTOR vel = { 0, 0, 0 };

	if (m_channel == nullptr)
		m_channel = Sound::Get().PlaySound(m_currentSound, pos, vel, maxRange);
	else
		m_channel = Sound::Get().PlaySound(m_currentSound, m_channel, pos, vel, maxRange);
	m_channel->setVolume(volume);
	m_channel->setPitch(pitch);

	//std::cout << m_channel << std::endl;
	if (loop)
		m_channel->setMode(FMOD_LOOP_NORMAL + FMOD_3D);
	else
		m_channel->setMode(FMOD_LOOP_OFF + FMOD_3D);
}

void AudioSource::Stop()
{
	m_channel->stop();
}

bool AudioSource::IsPlaying()
{
	bool returnValue;
	m_channel->isPlaying(&returnValue);
	return returnValue;
}

void AudioSource::SetSound(FMOD::Sound* sound)
{
	m_channel->stop();
	m_currentSound = sound;
}