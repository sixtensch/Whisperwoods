#pragma once
#include "Animator.h"
#include "GameObject.h"
#include "CutsceneKey.h"
#include "Camera.h"
#include "GUI.h"

enum CutsceneChannelType
{
	CutsceneTypeAnimator = 0,
	CutsceneTypeTransform,
	CutsceneTypeCamera,
	CutsceneTypeGUI,
	CutsceneTypeText,

	CutsceneTypeCount // Keep last
};

struct WhisperWoodsCutsceneChannelHead
{
	char name[128] = { '\0' };
	CutsceneChannelType channelType;
	int numKeys;
};

struct CutsceneChannel
{
	std::string name;
	int32_t sceneFrameDuration;
	//cs::List< shared_ptr<CutsceneKey>> keys;
	CutsceneChannelType channelType;
	int numKeys;

	// Helper functions.
	Quaternion Squad(Quaternion q0, Quaternion q1, Quaternion q2, Quaternion q3, float t);

	Vec3 Lerp(Vec3 a, Vec3 b, float t);

	float Lerp( float a, float b, float t );

	Vec3 GetInterpolatedValue(cs::List<Vec3KeyFrame>& keys, float time, float duration);

	float GetInterpolatedValue( cs::List<FloatKeyFrame>& keys, float time, float duration );

	Quaternion GetInterpolatedValue(cs::List<QuatKeyFrame>& keys, float time, float duration);

	virtual void Update(float animationTime, float durationRef) = 0;
};

struct CutsceneAnimatorChannel : CutsceneChannel
{
	Animator* targetAnimator;
	cs::List<CutsceneAnimationTriggerKey> keys;
	//cs::List< shared_ptr<CutsceneAnimationTriggerKey>> keys;
	CutsceneAnimatorChannel()
	{
		targetAnimator = nullptr;
		keys = {  };
		channelType = CutsceneTypeAnimator;
		numKeys = 0;
	}
	CutsceneAnimatorChannel( std::string name, Animator* targetAnimator ) : 
		targetAnimator( targetAnimator ) {
		keys = {  };
		this->name = name; 
		channelType = CutsceneTypeAnimator;
		numKeys = 0;
	}

	void AddKey( CutsceneAnimationTriggerKey key )
	{
		this->keys.Add( key );
		//channelType = CutsceneTypeAnimator;
		numKeys = keys.Size();
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
	void Update(float animationTime, float durationRef) override;
};

struct CutsceneTransformChannel : CutsceneChannel
{
	Transform* targetTransform;
	//cs::List< shared_ptr<CutsceneTransformKey>> keys;
	cs::List<CutsceneTransformKey> keys;
	CutsceneTransformChannel()
	{
		targetTransform = nullptr;
		keys = { };
		channelType = CutsceneTypeTransform;
		numKeys = 0;
	}
	CutsceneTransformChannel( std::string name, Transform* targetTransform ) : targetTransform( targetTransform ) 
	{ 
		keys = {  };
		this->name = name;
		channelType = CutsceneTypeTransform;
		numKeys = 0;
	}

	void AddKey( CutsceneTransformKey key )
	{
		this->keys.Add( key );
		//channelType = CutsceneTypeTransform;
		numKeys = keys.Size();
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
	}

	void Update(float animationTime, float durationRef) override;
};

struct CutsceneCameraChannel : CutsceneChannel
{
	Camera* targetCamera;
	cs::List<CutsceneCameraKey> keys;
	CutsceneCameraChannel()
	{
		targetCamera = nullptr;
		keys = {  };
		channelType = CutsceneTypeCamera;
		numKeys = 0;
	}
	CutsceneCameraChannel( std::string name, Camera* targetCamera ) : 
		targetCamera( targetCamera ) 
	{ 
		keys = {  };
		this->name = name; 
		channelType = CutsceneTypeCamera;
		numKeys = 0;
	}

	void AddKey( CutsceneCameraKey key )
	{
		this->keys.Add( key );
		//channelType = CutsceneTypeCamera;
		numKeys = keys.Size();
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
	}

	void Update(float animationTime, float durationRef) override;
};

struct CutsceneGUIChannel : CutsceneChannel
{
	// target textbox
	cs::List<CutsceneGUITriggerKey> keys;
	GUI* targetGUI;
	int targetGUIElement;
	CutsceneGUIChannel()
	{
		targetGUI = nullptr;
		keys = {  };
		channelType = CutsceneTypeGUI;
		numKeys = 0;
		targetGUIElement = 0;
	}
	CutsceneGUIChannel(std::string name, GUI* targetGUI) :
		targetGUI(targetGUI) 
	{
		keys = {  };
		this->name = name;
		channelType = CutsceneTypeGUI;
		numKeys = 0;
	}

	void AddKey(CutsceneGUITriggerKey key)
	{
		this->keys.Add(key);
		//channelType = CutsceneTypeGUI;
		numKeys = keys.Size();
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
	void Update(float animationTime, float durationRef) override;
};

struct CutsceneTextChannel : CutsceneChannel
{
	// target textbox
	cs::List<CutsceneTextTriggerKey> keys;

	CutsceneTextChannel()
	{
		keys = {  };
		channelType = CutsceneTypeText;
		numKeys = 0;
	}
	/*CutsceneGUIChannel(std::string name, GUI* targetGUI) :
		targetGUI(targetGUI) {
		this->name = name;
	}*/
	void AddKey( CutsceneTextTriggerKey key )
	{
		keys = {  };
		this->keys.Add( key );
		//channelType = CutsceneTypeText;
		numKeys = keys.Size();
		//this->keys[this->keys.Size() - 1]->parentChannel = this;
	}
	void Update(float animationTime, float durationRef) override {};
};

