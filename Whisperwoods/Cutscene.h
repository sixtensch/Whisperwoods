#pragma once
#include "CutsceneChannel.h"


struct WhisperWoodsCutsceneHead
{
	char name[128] = { '\0' };
	int numChannels;
};

struct Cutscene
{
	std::string name;
	float duration;
	bool isActive;
	cs::List<shared_ptr<CutsceneChannel>> channels;

	Cutscene() = default;
	Cutscene(std::string name) : name(name), duration(0), isActive(false) {}

	void AddChannel( shared_ptr<CutsceneChannel> newChannel );
	//void AddKey( shared_ptr<CutsceneKey> key );
	//void Update( float time );
	// list of keyframes...
};