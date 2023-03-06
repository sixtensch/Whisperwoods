#pragma once
#include "Animator.h"
#include "GameObject.h"

struct CutsceneKey
{
	CutsceneChannel* parentChannel;
	int32_t frame;
	float time;
};

struct CutsceneCameraKey : CutsceneKey
{
	Vec3 pos;
	Quaternion rot;
	float fov;
	float lerpTime; // how fast the camera should assume the key
	// TODO: Add more camera things.
	CutsceneCameraKey( float time, Vec3 pos, Quaternion rot, float fov, float lerpTime ) : pos( pos ), rot( rot ), fov( fov ), lerpTime( lerpTime ) {
		this->time = time;
	};
};

struct CutsceneAnimationTriggerKey : CutsceneKey
{
	//Animator* targetAnimator;
	int targetAnimation;
	float animationTime;
	float animationSpeed;
	bool loopAnimation;
	CutsceneAnimationTriggerKey( float time, Animator* targetAnimator, int targetAnimation, float animationTime, float animationSpeed, bool loopAnimation ) :
		/*targetAnimator( targetAnimator ),*/ targetAnimation( targetAnimation ), animationTime( animationTime ), animationSpeed( animationSpeed ), loopAnimation( loopAnimation ) {
		this->time = time;
	};
};

struct CutsceneTransformKey : CutsceneKey
{
	//GameObject* targetObject;
	Vec3 pos;
	Quaternion rot;
	Vec3 scale;
	CutsceneTransformKey( float time, GameObject* targetObject, Vec3 pos, Quaternion rot, Vec3 scale ) : /*targetObject( targetObject ), */pos( pos ), rot( rot ), scale( scale ) {
		this->time = time;
	};
};

struct CutsceneTextTriggerKey : CutsceneKey
{
	// some target text box...
	std::string text;
	float duration; // needed?
	CutsceneTextTriggerKey( float time, std::string text, float duration ) : text( text ), duration( duration ) {
		this->time = time;
	};
};