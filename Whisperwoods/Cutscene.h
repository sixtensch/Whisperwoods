#pragma once
#include "CutsceneChannel.h"

struct Cutscene
{
	std::string name;
	float duration;
	bool isActive;
	cs::List<shared_ptr<CutsceneChannel>> channels;

	Cutscene( std::string name ) : name( name ) {}

	void AddChannel( shared_ptr<CutsceneChannel> newChannel );
	//void AddKey( shared_ptr<CutsceneKey> key );
	//void Update( float time );
	// list of keyframes...
};