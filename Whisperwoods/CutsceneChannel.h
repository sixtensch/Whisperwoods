#pragma once
#include "Animator.h"
#include "GameObject.h"
#include "CutsceneKey.h"
#include "Camera.h"

struct CutsceneChannel
{
	std::string name;
	virtual void AddKey( shared_ptr<CutsceneKey> key ) = 0;
};

struct CutsceneAnimatorChannel : CutsceneChannel
{
	Animator* targetAnimator;
	cs::List< shared_ptr<CutsceneAnimationTriggerKey>> keys;
	void AddKey( shared_ptr<CutsceneAnimationTriggerKey> key )
	{
		keys.Add( key );
	}
};

struct CutsceneTransformChannel : CutsceneChannel
{
	Transform* targetTransform;
	cs::List< shared_ptr<CutsceneTransformKey>> keys;
	void AddKey( shared_ptr<CutsceneTransformKey> key )
	{
		keys.Add( key );
	}
};

struct CutsceneCameraChannel : CutsceneChannel
{
	Camera* targetCamera;
	cs::List< shared_ptr<CutsceneCameraKey>> keys;
	void AddKey( shared_ptr<CutsceneCameraKey> key )
	{
		keys.Add( key );
	}
};

struct CutsceneTextChannel : CutsceneChannel
{
	// target textbox
	cs::List< shared_ptr<CutsceneTextTriggerKey>> keys;
	void AddKey( shared_ptr<CutsceneTextTriggerKey> key )
	{
		keys.Add( key );
	}
};