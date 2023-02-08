#pragma once
#include "Armature.h"
#include "ModelResource.h"
#include "AnimationResource.h"

Vec3 Lerp( Vec3 a, Vec3 b, float t )
{
	return a * (1.0f - t) + b * t;
}

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
	AnimatorCombinedChannel() = default;
	AnimatorCombinedChannel( std::string name ) : name( name ) {};
};


// TODO: Maybe add some consts here and there.
struct AnimatorAnimation // Holds the middleman data for the various playback times and interpolated values.
{
	Animation* sourceAnimation;
	cs::List<AnimatorChannel> animationChannels;
	float time;
	float speed;
	float influence;

	AnimatorAnimation() : time( 0.0f ), speed( 1.0f ), influence( 1.0f ) { sourceAnimation = nullptr;  }

	AnimatorAnimation( float initTime, float playbackSpeed, float influence, Animation* sourceAnimation, ModelRiggedResource* modelReference):
		time(initTime), speed(playbackSpeed), influence(influence), sourceAnimation(sourceAnimation) 
	{
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
		time += speed * deltaTime; // TODO: Maybe % in some way.
		if (time > 1) time = 0;

		// Interpolate and shit
		for (unsigned int i = 0; i < animationChannels.Size(); i++)
		{
			animationChannels[i].iPos = GetInterpolatedValue( sourceAnimation->channels[i].positionKeyFrames, time, sourceAnimation->duration );
			animationChannels[i].iRot = GetInterpolatedValue( sourceAnimation->channels[i].rotationKeyFrames, time, sourceAnimation->duration );
			animationChannels[i].iScl = GetInterpolatedValue( sourceAnimation->channels[i].scaleKeyFrames, time, sourceAnimation->duration );
		}
	}
	
	// Helper functions.
	Vec3 GetInterpolatedValue( cs::List<Vec3KeyFrame> keys, float time, float duration );
	Quaternion GetInterpolatedValue( cs::List<QuatKeyFrame> keys, float time, float duration );
};

// TODO: This class might be threadable.
class Animator
{
	ModelRiggedResource* modelReference;
	cs::List<AnimatorAnimation> loadedAnimations;
	float playbackSpeed;

	cs::List<AnimatorCombinedChannel> combinedChannels;


	Animator( ModelRiggedResource* modelReference ) : modelReference(modelReference)
	{
		// Add tracking channels for all bones (possible optimization target vector)
		for (unsigned int i = 0; i < modelReference->armature.bones.Size(); i++)
		{
			combinedChannels.Add( AnimatorCombinedChannel( modelReference->armature.bones[i].name ));
		}
	}




	// Per frame interpolation of all different animations that might be loaded. TODO: USE BIND POSE AS BASE
	void CombineAnimations()
	{
		for (unsigned int i = 0; i < loadedAnimations.Size(); i++)
		{
			for (unsigned int j = 0; j < loadedAnimations[i].animationChannels.Size(); j++)
			{
				int index = loadedAnimations[i].animationChannels[j].boneIndex;
				
				// Lerp from one to the other
				combinedChannels[index].collectivePos =
					Lerp( combinedChannels[index].collectivePos, loadedAnimations[i].animationChannels[j].iPos,
						loadedAnimations[i].influence );

				combinedChannels[index].collectiveRot = 
					Quaternion::GetSlerp( combinedChannels[index].collectiveRot, loadedAnimations[i].animationChannels[j].iRot,
						loadedAnimations[i].influence );

				combinedChannels[index].collectiveScl =
					Lerp( combinedChannels[index].collectiveScl, loadedAnimations[i].animationChannels[j].iScl,
						loadedAnimations[i].influence );
			}
		}
	}

	void UpdateArmature()
	{
		//for
		//std::vector<DirectX::XMFLOAT4X4> modelMatrixList;
		//for (size_t i = 0; i < modelReference->armature.bones.Size(); i++)
		//{
		//	modelMatrixList.push_back( DirectX::XMFLOAT4X4() );
		//}

		//Bone* rootBone = &modelReference->armature.bones[0];
		//AnimatorChannel* rootC = GetChannel( rootBone->name );

		//rootBone->localPos = rootC->iPos;
		//rootBone->localRot = rootC->iRot;
		//rootBone->localScale = rootC->iScl;

		////𝑴𝒐𝒅𝒆𝒍𝑻𝒙𝒊 = 𝑃𝑎𝑟𝑒𝑛𝑡𝑀𝑜𝑑𝑒𝑙𝑇𝑥𝑖∙𝐿𝑜𝑐𝑎𝑙𝑇𝑥𝑖
		////𝑭𝒊𝒏𝒂𝒍𝑻𝒙𝒊 = 𝑀𝑜𝑑𝑒𝑙𝑇𝑥𝑖∙𝐵𝑖𝑛𝑑𝑀𝑜𝑑𝑒𝑙𝐼𝑛𝑣𝑒𝑟𝑠𝑒𝑇𝑥𝑖
		//DXMAT giMatXM = DirectX::XMLoadFloat4x4( &armature->globalInverseTransform );

		//DXMAT rmMatXM = GetTransformationMatrix( rootBone->localPos, rootBone->localRot, rootBone->localScale );
		//DXMAT ribMatXM = DirectX::XMLoadFloat4x4( &rootBone->inverseBindMatrix );
		//DXMAT rfMatXM = DirectX::XMMatrixMultiply( ribMatXM, rmMatXM );
		//rfMatXM = DirectX::XMMatrixMultiply( giMatXM, rfMatXM );
		//DirectX::XMStoreFloat4x4( &rootBone->posedMatrix, rfMatXM );
		//DirectX::XMStoreFloat4x4( &modelMatrixList[0], rmMatXM );

		//for (size_t i = 1; i < armature->bones.size(); i++)
		//{
		//	Bone* bone = &armature->bones[i];
		//	AnimatorChannel* bC = GetChannel( bone->name );
		//	bone->localPos = bC->iPos;
		//	bone->localRot = bC->iRot;
		//	bone->localScale = bC->iScl;
		//	DXMAT lMatXM = GetTransformationMatrix( bone->localPos, bone->localRot, bone->localScale );
		//	DXMAT pMatXM = DirectX::XMLoadFloat4x4( &modelMatrixList[bone->parentIndex] );
		//	DXMAT mMatXM = DirectX::XMMatrixMultiply( lMatXM, pMatXM );
		//	DXMAT ibMatXM = DirectX::XMLoadFloat4x4( &bone->inverseBindMatrix );
		//	DXMAT fMatXM = DirectX::XMMatrixMultiply( ibMatXM, mMatXM );
		//	fMatXM = DirectX::XMMatrixMultiply( giMatXM, fMatXM );
		//	DirectX::XMStoreFloat4x4( &bone->posedMatrix, fMatXM );
		//	DirectX::XMStoreFloat4x4( &modelMatrixList[i], mMatXM );
		//}
	}

	void Update( float deltaTime )
	{
		if (modelReference && loadedAnimations.Size())
		{
			for (unsigned int i = 0; i < loadedAnimations.Size(); i++)
			{
				loadedAnimations[i].UpdateAnimation( deltaTime, playbackSpeed );
			}
			CombineAnimations();
			UpdateArmature();	
		}
	}
};