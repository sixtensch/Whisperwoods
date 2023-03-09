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

	// Helper functions.
	Quaternion Squad(Quaternion q0, Quaternion q1, Quaternion q2, Quaternion q3, float t);
	Vec3 Lerp(Vec3 a, Vec3 b, float t);
	Vec3 GetInterpolatedValue(cs::List<Vec3KeyFrame> keys, float time, float duration);
	Quaternion GetInterpolatedValue(cs::List<QuatKeyFrame> keys, float time, float duration);
};

struct CutsceneAnimatorChannel : CutsceneChannel
{
	Animator* targetAnimator;
	//cs::List< shared_ptr<CutsceneAnimationTriggerKey>> keys;

	CutsceneAnimatorChannel( std::string name, Animator* targetAnimator ) : targetAnimator( targetAnimator ) { this->name = name; }

	void AddKey( shared_ptr<CutsceneAnimationTriggerKey> key )
	{
		this->keys.Add( key );
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
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
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
};

struct CutsceneCameraChannel : CutsceneChannel
{
	Camera* targetCamera;
	//cs::List< shared_ptr<CutsceneCameraKey>> keys;

	CutsceneCameraChannel( std::string name, Camera* targetCamera ) : 
		targetCamera( targetCamera ) { this->name = name; }

	void AddKey( shared_ptr<CutsceneCameraKey> key )
	{
		this->keys.Add( key );
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
	}

	void Update(float animationTime, float durationRef);
};

struct CutsceneTextChannel : CutsceneChannel
{
	// target textbox
	//cs::List< shared_ptr<CutsceneTextTriggerKey>> keys;

	CutsceneTextChannel() = default;

	void AddKey( shared_ptr<CutsceneTextTriggerKey> key )
	{
		this->keys.Add( key );
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
};