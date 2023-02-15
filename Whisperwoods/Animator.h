#pragma once
#include "Armature.h"
#include "ModelResource.h"
#include "AnimationResource.h"
#include "Transform.h"
#include <thread>
#include <mutex>

#define NUM_THREADS 4;

struct AnimatorChannel // Datastore for the interpolated animation channel values
{
	std::string name; // redundant unless for re-aquire of bone index reference at runtime.
	int boneIndex; // Might be useful
	Vec3 iPos; // interpolated pos
	Quaternion iRot; // interpolated rot
	Vec3 iScl; // interpolated scale
	AnimatorChannel() = default;
	AnimatorChannel( std::string name, int boneIndex ) : name( name ), boneIndex( boneIndex ) {};
};

struct AnimatorCombinedChannel // Datastore for all interpolated values affecting the same bone (to be blended) TODO: USE BIND POSE AS BASE
{
	std::string name;
	Vec3 collectivePos; // collective pos
	Quaternion collectiveRot; // collective rot
	Vec3 collectiveScl; // collective scale
	AnimatorCombinedChannel() 
	{
		collectivePos = Vec3();
		collectiveRot = Quaternion::GetIdentity();
		collectiveScl = Vec3();
	};
	AnimatorCombinedChannel( std::string name ) : name( name ) 
	{
		collectivePos = Vec3();
		collectiveRot = Quaternion::GetIdentity();
		collectiveScl = Vec3();
	};
};

// TODO: Maybe add some consts here and there.
struct AnimatorAnimation // Holds the middleman data for the various playback times and interpolated values.
{
	Animation* sourceAnimation;
	cs::List<AnimatorChannel> animationChannels;
	float time;
	float speed;
	float influence;
	float offset;
	bool isActive;

	Vec3 Lerp(Vec3 a, Vec3 b, float t)
	{
		return a * (1.0f - t) + b * t;
	}

	Quaternion Squad( Quaternion q0, Quaternion q1, Quaternion q2, Quaternion q3, float t );

	AnimatorAnimation() : time(0.0f), speed(1.0f), influence(1.0f) { sourceAnimation = nullptr; isActive = false; offset = 0; }

	AnimatorAnimation( float initTime, float playbackSpeed, float influence, Animation* sourceAnimation, ModelRiggedResource* modelReference):
		time(initTime), speed(playbackSpeed), influence(influence), sourceAnimation(sourceAnimation) 
	{
		offset = 0;
		isActive = false;
		// Initialize animator channels.
		for (unsigned int i = 0; i < sourceAnimation->channels.Size(); i++)
		{
			std::string cName = sourceAnimation->channels[i].channelName;
			int boneIndex = -1;
			// Resolve potenially broken bone order
			for (unsigned int j = 0; j < modelReference->armature.bones.Size(); j++)
			{
				if (cName == modelReference->armature.bones[j].name)
				{
					boneIndex = j;
					break;
				}
			}
			animationChannels.Add( AnimatorChannel( sourceAnimation->channels[i].channelName, boneIndex) ); // TODO: USE BIND POSE AS BASE
		}
	}

	void UpdateAnimation( float deltaTime, float playSpeed )
	{
		// Adjust the time acording to local speed
		float localTime = (time + offset)*speed;
		if (localTime > 1) localTime = localTime - (int)localTime;
		if (localTime < 0) localTime = localTime + (int)abs(localTime);

		// Interpolate and shit
		for (unsigned int i = 0; i < animationChannels.Size(); i++)
		{
			animationChannels[i].iPos = GetInterpolatedValue( sourceAnimation->channels[i].positionKeyFrames, localTime, sourceAnimation->duration );
			animationChannels[i].iRot = GetInterpolatedValue( sourceAnimation->channels[i].rotationKeyFrames, localTime, sourceAnimation->duration );
			animationChannels[i].iScl = GetInterpolatedValue( sourceAnimation->channels[i].scaleKeyFrames, localTime, sourceAnimation->duration );
		}
	}
	
	// Helper functions.
	Vec3 GetInterpolatedValue( cs::List<Vec3KeyFrame> keys, float time, float duration );
	Quaternion GetInterpolatedValue( cs::List<QuatKeyFrame> keys, float time, float duration );
};

// TODO: This class might be threadable.
class Animator
{
public:
	ModelRiggedResource* modelReference;
	cs::List<AnimatorAnimation> loadedAnimations;
	float minInfluenceThreshold;
	float globalTime;
	float playbackSpeed;
	bool looping;
	cs::List<AnimatorCombinedChannel> combinedChannels;
	
	// Initialization stuff
	Animator(ModelRiggedResource* modelReference);
	void AddAnimation(Animation* sourceAnimation, float startTime, float speed, float influence);

	Vec3 Lerp(Vec3 a, Vec3 b, float t);
	float Lerp(float a, float b, float t);	
	Quaternion Lerp(Quaternion q0, Quaternion q1, float t);

	// Functions for calculating matricies from the interpolated individual channel keyframes (pos, rot, scale)
	DirectX::XMMATRIX GetTransformationMatrix(Vec3 translation, Quaternion rotation, Vec3 scaling);
	//Mat4 CalculateMatrix(Vec3 p_pos, Quaternion p_rotation, Vec3 p_scale);

	void PlayAnimation(int index, float startTime, float influence, bool loop, bool stopOthers);
	void StopAnimation(int index);
	bool IsPlaying(int index);


	// Per frame interpolation of all different animations that might be loaded. TODO: USE BIND POSE AS BASE
	void CombineAnimations();
	void UpdateArmature();

	void Update(float deltaTime);
};