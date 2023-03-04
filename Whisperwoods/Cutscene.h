#pragma once
#include "CutsceneKey.h"

struct Cutscene
{
	float duration;
	bool isActive;
	cs::List<shared_ptr<CutsceneKey>> cutsceneKeys;

	void AddKey( shared_ptr<CutsceneKey> key );
	//void Update( float time );
	// list of keyframes...
};