#pragma once
#include "Animator.h"
#include "GameObject.h"

struct CutsceneKey
{
	float time;
};

struct CutsceneCameraKey : CutsceneKey
{
	Vec3 pos;
	Quaternion rot;
	float fov;
	float lerpTime; // how fast the camera should assume the key
	// TODO: Add more camera things.
	CutsceneCameraKey( Vec3 pos, Quaternion rot, float fov, float lerpTime ) : pos( pos ), rot( rot ), fov( fov ), lerpTime( lerpTime ) {};
};

struct CutsceneAnimationTriggerKey : CutsceneKey
{
	Animator* targetAnimator;
	int targetAnimation;
	float animationTime;
	float animationSpeed;
	bool loopAnimation;
	CutsceneAnimationTriggerKey( Animator* targetAnimator, int targetAnimation, float animationTime, float animationSpeed, bool loopAnimation ) :
		targetAnimator( targetAnimator ), targetAnimation( targetAnimation ), animationTime( animationTime ), animationSpeed( animationSpeed ), loopAnimation( loopAnimation ) {};
};

struct CutsceneTransformKey : CutsceneKey
{
	GameObject* targetObject;
	Vec3 pos;
	Quaternion rot;
	Vec3 scale;
	CutsceneTransformKey( GameObject* targetObject, Vec3 pos, Quaternion rot, Vec3 scale ) : targetObject( targetObject ), pos( pos ), rot( rot ), scale( scale ) {};
};

struct CutsceneTextTriggerKey : CutsceneKey
{
	// some target text box...
	std::string text;
	float duration; // needed?
	CutsceneTextTriggerKey( std::string text, float duration ) : text( text ), duration( duration ) {};
};