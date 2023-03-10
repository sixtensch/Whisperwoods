#pragma once
#include "Animator.h"
#include "GameObject.h"

struct CutsceneKey
{
	//CutsceneChannel* parentChannel;
	int32_t frame;
	float time;
	CutsceneKey() = default;
	//virtual float GetTime() = 0;
};

struct CutsceneCameraKey : CutsceneKey
{
	Vec3 pos;
	Quaternion rot;
	float fov;
	float lerpTime; // how fast the camera should assume the key
	// TODO: Add more camera things.
	CutsceneCameraKey() = default;
	CutsceneCameraKey( float time, Vec3 pos, Quaternion rot, float fov, float lerpTime ) : pos( pos ), rot( rot ), fov( fov ), lerpTime( lerpTime ) {
		this->time = time;
	};
	//float GetTime() override { return time; };
};

struct CutsceneAnimationTriggerKey : CutsceneKey
{
	//Animator* targetAnimator;
	int targetAnimation;
	float animationTime;
	float animationSpeed;
	bool loopAnimation;
	CutsceneAnimationTriggerKey() = default;
	CutsceneAnimationTriggerKey( float time, int targetAnimation, float animationTime, float animationSpeed, bool loopAnimation ) :
		/*targetAnimator( targetAnimator ),*/ targetAnimation( targetAnimation ), animationTime( animationTime ), animationSpeed( animationSpeed ), loopAnimation( loopAnimation ) {
		this->time = time;
	};
	//float GetTime() override { return time; };
};

struct CutsceneTransformKey : CutsceneKey
{
	//GameObject* targetObject;
	Vec3 pos;
	Quaternion rot;
	Vec3 scale;
	CutsceneTransformKey() = default;
	CutsceneTransformKey( float time, Vec3 pos, Quaternion rot, Vec3 scale ) : pos( pos ), rot( rot ), scale( scale ) {
		this->time = time;
	};
	//float GetTime() override { return time; };
};

struct CutsceneGUITriggerKey : CutsceneKey
{
	float alpha;
	bool active;
	Vec3 color;
	CutsceneGUITriggerKey() = default;
	CutsceneGUITriggerKey( float time, float alpha, bool active, Vec3 color)
		: alpha( alpha ), active( active ), color( color ) {
		this->time = time;
	}
	// some target text box...
	//std::string text;
	//float duration; // needed?
	/*CutsceneTextTriggerKey(float time, std::string text, float duration) : text(text), duration(duration) {
		this->time = time;
	};*/
	//float GetTime() override { return time; };
};

struct CutsceneTextTriggerKey : CutsceneKey
{
	// some target text box...
	std::string text;
	float duration; // needed?
	CutsceneTextTriggerKey() = default;
	CutsceneTextTriggerKey( float time, std::string text, float duration ) : text( text ), duration( duration ) {
		this->time = time;
	};
	//float GetTime() override { return time; };
};