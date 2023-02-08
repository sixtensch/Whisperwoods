#pragma once

#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_errors.h>

class Sound sealed
{
public:
	Sound();
	~Sound();

	static Sound& Get();

	FMOD_VECTOR listenerPos;

	/// Initializes the sound.
	bool Init();

	/// Loads a sound and stores the pointer handle in the class vector.
	FMOD::Sound* LoadSound(std::string filePath);
	bool LoadSound(const std::string filePath, FMOD::Sound* sound);

	/// Plays the sound at index in the soundVector.
	FMOD::Channel* PlaySound(int index);

	/// Plays the sound matching the pointer provided if it exists
	//FMOD::Channel* PlaySound( FMOD::Sound* sound );

	/// Overload with some fine control :3
	FMOD::Channel* PlaySound(FMOD::Sound* sound, FMOD_VECTOR pos, FMOD_VECTOR vel, float maxRange);

	/// Overload with some finer control :3
	FMOD::Channel* PlaySound(FMOD::Sound* sound, FMOD::Channel* channel, FMOD_VECTOR pos, FMOD_VECTOR vel, float maxRange);

	/// Plays the sound matching the name provided if it exists
	//FMOD::Channel* PlaySound( std::string sound );

	/// Stops a sound playing at the index channel
	void StopSound(int index);

	/// Updates the FMOD System and stuff.
	void Update();

	/// Updates the FMOD system with given listener 3d params.
	void Update(FMOD_VECTOR listenerPos, FMOD_VECTOR listenerVel);

	/// stops all potential channels playing sounds.
	void StopAllSounds();


private:
	static Sound* s_singleton;
	FMOD_RESULT m_fmodResult;
	FMOD::System* m_system;
	std::vector<FMOD::Sound*> m_sounds; // TODO: CHANGE TO RESOURCE MANAGER? Move this out.
	std::vector<FMOD::Channel*> m_channels; // TODO: CHANGE TO RESOURCE MANAGER? GEt this shit away.
};

