#include "core.h"
#include "Sound.h"

#include "core.h"
#include "Input.h"

#include "Debug.h"

Sound* Sound::s_singleton = nullptr;

// Error logging function.
bool succeededOrWarn(const std::string& message, FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::string errorString = message + ": " + FMOD_ErrorString(result);
		LOG_ERROR(errorString.c_str());
		return false;
	}
	return true;
}

Sound::Sound()
{
	if (s_singleton != nullptr)
	{
		EXC("Sound singleton re-initialization.");
	}
	s_singleton = this; 

	listenerPos = FMOD_VECTOR();
	m_fmodResult = FMOD_RESULT();
	m_system = nullptr;

	this->Init();
}

Sound::~Sound()
{
	s_singleton = nullptr;
}

Sound& Sound::Get()
{
#ifdef WW_DEBUG
	if (s_singleton == nullptr)
	{
		EXC("Sound singleton not found (is nullptr).");
	}
#endif

	return *s_singleton;
}


bool Sound::Init()
{
	succeededOrWarn("Failed to create FMOD system", FMOD::System_Create(&this->m_system));
	succeededOrWarn("Failed to init FMOD system", m_system->init(512, FMOD_INIT_NORMAL, 0));
	succeededOrWarn("Failed to set spatial settings", m_system->set3DSettings(1, 1000, 1));
	LOG("Initialized Sound");
	return true;
}

FMOD::Sound* Sound::LoadSound(std::string filePath)
{
	FMOD::Sound* newSound;
	bool success = succeededOrWarn("Failed to create new sound", m_system->createSound(filePath.c_str(), FMOD_LOOP_OFF + FMOD_3D, nullptr, &newSound));
	if (success)
		m_sounds.push_back(newSound);
	return newSound;
}

bool Sound::LoadSound(const std::string filePath, FMOD::Sound*& sound)
{
	FMOD_RESULT result = m_system->createSound(filePath.c_str(), FMOD_LOOP_OFF + FMOD_3D, nullptr, &sound);
	return result == FMOD_OK;
}

FMOD::Channel* Sound::PlaySound(int index)
{
	//if (soundVector.size() < index)
	std::cout << "Size: " << m_sounds.size() << std::endl;
	if (m_sounds.size() < index + 1) return nullptr;
	FMOD::Channel* ch;
	succeededOrWarn("Failed to play sound", m_system->playSound(m_sounds[index], nullptr, false, &ch));

	if (m_channels.size() < index + 1)
		m_channels.push_back(ch);
	else
		m_channels[index] = ch;
	FMOD_VECTOR pos = { 0,0,0 };
	FMOD_VECTOR vel = { 0,0,0 };
	succeededOrWarn("Failed to set channel properties", ch->set3DAttributes(&pos, &vel));
	return ch;
}


FMOD::Channel* Sound::PlaySound(FMOD::Sound* sound, FMOD_VECTOR pos, FMOD_VECTOR vel, float maxRange)
{
	//bool exists = false;
	//for (int i = 0; i < m_sounds.size(); i++)
	//{
	//	if (m_sounds[i] == sound) exists = true;
	//}
	//if (!exists) return nullptr;

	FMOD::Channel* ch;
	m_channels.push_back(ch);

	succeededOrWarn("Failed to play sound", m_system->playSound(sound, nullptr, false, &ch));
	succeededOrWarn("Failed to set channel 3D Attributes", ch->set3DAttributes(&pos, &vel));
	succeededOrWarn("Failed to set channel 3D Range", ch->set3DMinMaxDistance(1.0f, maxRange));
	return ch;
}


FMOD::Channel* Sound::PlaySound(FMOD::Sound* sound, FMOD::Channel* channel, FMOD_VECTOR pos, FMOD_VECTOR vel, float maxRange)
{
	/*bool exists = false;
	for (int i = 0; i < m_sounds.size(); i++)
	{
		if (m_sounds[i] == sound) exists = true;
	}
	if (!exists) return channel;*/

	succeededOrWarn("Failed to play sound", m_system->playSound(sound, nullptr, false, &channel));
	succeededOrWarn("Failed to set channel 3D Attributes", channel->set3DAttributes(&pos, &vel));
	succeededOrWarn("Failed to set channel 3D Range", channel->set3DMinMaxDistance(1.0f, maxRange));
	return channel;
}

void Sound::Update(FMOD_VECTOR lPos, FMOD_VECTOR listenerVel)
{
	FMOD_VECTOR forw = { 0,0,1 };
	FMOD_VECTOR up = { 0,1,0 };
	succeededOrWarn("Failed when trying to set 3d attributes", this->m_system->set3DListenerAttributes(0, &lPos, &listenerVel, &forw, &up));
	succeededOrWarn("Failed when trying to tick FMOD", this->m_system->update());
}

void Sound::Update()
{
	FMOD_VECTOR vel = { 0,0,0 };
	this->Update(listenerPos, vel);
}

void Sound::StopAllSounds()
{
	for (int i = 0; i < m_channels.size(); i++)
	{
		m_channels[i]->stop();
	}
}

void Sound::StopSound(int index)
{
	if (m_channels.size() < index)
	{
		bool isP = false;
		m_channels[index]->isPlaying(&isP);
		if (isP)
			m_channels[index]->stop();
	}
}
