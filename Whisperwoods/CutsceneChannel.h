#pragma once
#include "Animator.h"
#include "GameObject.h"
#include "CutsceneKey.h"
#include "Camera.h"

struct CutsceneChannel
{
	std::string name;
	int32_t sceneFrameDuration;
	cs::List< shared_ptr<CutsceneKey>> keys;
};

struct CutsceneAnimatorChannel : CutsceneChannel
{
	Animator* targetAnimator;
	//cs::List< shared_ptr<CutsceneAnimationTriggerKey>> keys;

	CutsceneAnimatorChannel( std::string name, Animator* targetAnimator ) : targetAnimator( targetAnimator ) { this->name = name; }

	void AddKey( shared_ptr<CutsceneAnimationTriggerKey> key )
	{
		this->keys.Add( key );
		this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
};

struct CutsceneTransformChannel : CutsceneChannel
{
	Transform* targetTransform;
	//cs::List< shared_ptr<CutsceneTransformKey>> keys;

	CutsceneTransformChannel( std::string name, Transform* targetTransform ) : targetTransform( targetTransform ) { this->name = name; }

	void AddKey( shared_ptr<CutsceneTransformKey> key )
	{
		this->keys.Add( key );
		this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
};

struct CutsceneCameraChannel : CutsceneChannel
{
	Camera* targetCamera;
	//cs::List< shared_ptr<CutsceneCameraKey>> keys;

	CutsceneCameraChannel( std::string name, Camera* targetCamera ) : targetCamera( targetCamera ) { this->name = name; }

	void AddKey( shared_ptr<CutsceneCameraKey> key )
	{
		this->keys.Add( key );
		this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
};

struct CutsceneTextChannel : CutsceneChannel
{
	// target textbox
	//cs::List< shared_ptr<CutsceneTextTriggerKey>> keys;

	CutsceneTextChannel() = default;

	void AddKey( shared_ptr<CutsceneTextTriggerKey> key )
	{
		this->keys.Add( key );
		this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
};