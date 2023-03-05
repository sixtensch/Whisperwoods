#pragma once
#include "CutsceneChannel.h"

struct Cutscene
{
	float duration;
	bool isActive;
	cs::List<shared_ptr<CutsceneChannel>> channels;

	void AddChannel( shared_ptr<CutsceneChannel> newChannel );
	//void AddKey( shared_ptr<CutsceneKey> key );
	//void Update( float time );
	// list of keyframes...
};