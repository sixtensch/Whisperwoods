#pragma once
#include "Armature.h"
#include "ModelResource.h"
#include "AnimationResource.h"
#include "Transform.h"


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

	Vec3 Lerp(Vec3 a, Vec3 b, float t)
	{
		return a * (1.0f - t) + b * t;
	}

	Quaternion Squad( Quaternion q0, Quaternion q1, Quaternion q2, Quaternion q3, float t );

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
public:
	ModelRiggedResource* modelReference;
	cs::List<AnimatorAnimation> loadedAnimations;
	float playbackSpeed;
	cs::List<AnimatorCombinedChannel> combinedChannels;

	Animator( ModelRiggedResource* modelReference ) : modelReference(modelReference)
	{
		// Add tracking channels for all bones (possible optimization target vector)
		for (unsigned int i = 0; i < modelReference->armature.bones.Size(); i++)
		{
			combinedChannels.Add(AnimatorCombinedChannel( modelReference->armature.bones[i].name ));
		}
	}

	void AddAnimation(Animation* sourceAnimation)
	{
		AnimatorAnimation newAnimation(0, 1, 1, sourceAnimation, modelReference);
		loadedAnimations.Add(newAnimation);
	}

	Vec3 Lerp(Vec3 a, Vec3 b, float t)
	{
		return a * (1.0f - t) + b * t;
	}

	Quaternion Lerp(Quaternion a, Quaternion b, float t)
	{
		return a * (1.0f - t) + b * t;
	}


	// Per frame interpolation of all different animations that might be loaded. TODO: USE BIND POSE AS BASE
	void CombineAnimations()
	{
		for (unsigned int i = 0; i < loadedAnimations.Size(); i++)
		{
			for (unsigned int j = 0; j < loadedAnimations[i].animationChannels.Size(); j++)
			{
				
				int index = loadedAnimations[i].animationChannels[j].boneIndex;
				if (index == -1) continue;
				// Lerp from one to the other
				combinedChannels[index].collectivePos =
					Lerp( combinedChannels[index].collectivePos, loadedAnimations[i].animationChannels[j].iPos,
						loadedAnimations[i].influence );

				combinedChannels[index].collectiveRot = 
					Lerp(combinedChannels[index].collectiveRot, loadedAnimations[i].animationChannels[j].iRot,
						loadedAnimations[i].influence );

				combinedChannels[index].collectiveScl =
					Lerp( combinedChannels[index].collectiveScl, loadedAnimations[i].animationChannels[j].iScl,
						loadedAnimations[i].influence );
			}
		}
	}


	DirectX::XMMATRIX GetTransformationMatrix(Vec3 translation, Quaternion rotation, Vec3 scaling)
	{
		DXMAT output = XMIDENT;
		DXMAT s = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);
		DirectX::XMFLOAT4 float4quat = {
					(float)rotation.x,
					(float)rotation.y,
					(float)rotation.z,
					(float)rotation.w };
		DirectX::XMVECTOR quatRot = DirectX::XMLoadFloat4(&float4quat);
		DXMAT r = DirectX::XMMatrixRotationQuaternion(quatRot);
		DXMAT t = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
		output = DirectX::XMMatrixMultiply(output, s);
		output = DirectX::XMMatrixMultiply(output, r);
		output = DirectX::XMMatrixMultiply(output, t);
		return output;
	}

	Mat4 CalculateMatrix(Vec3 p_pos, Quaternion p_rotation, Vec3 p_scale)
	{
		Mat4 rotationMatrix = p_rotation.Matrix();
		Mat4 scalingMatrix = Mat::scale3(p_scale);
		Mat4 translationMatrix = Mat::translation3(p_pos);
		Mat4 identityMatrix = Mat4();
		return identityMatrix * scalingMatrix * rotationMatrix * translationMatrix;
	}

	void UpdateArmature()
	{
		cs::List<DirectX::XMFLOAT4X4> modelMatrixList;
		for (size_t i = 0; i < modelReference->armature.bones.Size(); i++)
		{
			modelMatrixList.Add( DirectX::XMFLOAT4X4() );
		}

		modelReference->armature.boneMatricies.Clear();
		for (size_t i = 0; i < modelReference->armature.bones.Size(); i++)
		{
			DirectX::XMFLOAT4X4 mat;
			modelReference->armature.boneMatricies.Add(mat);
		}

		Bone* rootBone = &modelReference->armature.bones[0];
		AnimatorCombinedChannel* rootC = &combinedChannels[0];

		rootBone->localPos = rootC->collectivePos;
		rootBone->localRot = rootC->collectiveRot;
		rootBone->localScale = rootC->collectiveScl;

		////𝑴𝒐𝒅𝒆𝒍𝑻𝒙𝒊 = 𝑃𝑎𝑟𝑒𝑛𝑡𝑀𝑜𝑑𝑒𝑙𝑇𝑥𝑖∙𝐿𝑜𝑐𝑎𝑙𝑇𝑥𝑖
		////𝑭𝒊𝒏𝒂𝒍𝑻𝒙𝒊 = 𝑀𝑜𝑑𝑒𝑙𝑇𝑥𝑖∙𝐵𝑖𝑛𝑑𝑀𝑜𝑑𝑒𝑙𝐼𝑛𝑣𝑒𝑟𝑠𝑒𝑇𝑥𝑖

		//Mat4 globalInverse = modelReference->armature.globalInverseTransform;
		//Mat4 rootModelTx = CalculateMatrix(rootBone->localPos, rootBone->localRot, rootBone->localScale);
		////Mat4 rootModelTx = Mat4();
		//Mat4 rootInverseBindTx = rootBone->inverseBindMatrix;
		//Mat4 rootFinalTx = rootModelTx * rootInverseBindTx;
		//rootFinalTx =  rootFinalTx * globalInverse;
		//rootBone->posedMatrix = rootFinalTx;
		//modelReference->armature.boneMatricies[0] = rootFinalTx;
		//modelMatrixList[0] = rootModelTx;
		//rootBone->posedMatrix = Mat4(rfMatXM).Transpose();
		//modelReference->armature.boneMatricies[0] = rootBone->posedMatrix;
		//modelMatrixList[0] = Mat4(rmMatXM).Transpose();

		// DX code
		DXMAT globalInverseXM = DirectX::XMLoadFloat4x4(&modelReference->armature.globalInverseTransform);
		DXMAT rootModelXM = GetTransformationMatrix( rootBone->localPos, rootBone->localRot, rootBone->localScale );
		DXMAT rootInverseBindXM = DirectX::XMLoadFloat4x4( &rootBone->inverseBindMatrix );
		DXMAT rootFinalXM = DirectX::XMMatrixMultiply( rootInverseBindXM, rootModelXM );
		rootFinalXM = DirectX::XMMatrixMultiply( globalInverseXM, rootFinalXM );
		DirectX::XMStoreFloat4x4(&rootBone->posedMatrix, rootFinalXM);
		DirectX::XMStoreFloat4x4(&modelMatrixList[0], rootModelXM);
		modelReference->armature.boneMatricies[0] = rootBone->posedMatrix;
	
		for (size_t i = 1; i < combinedChannels.Size(); i++)
		{
			Bone* bone = &modelReference->armature.bones[i];
			AnimatorCombinedChannel* bC = &combinedChannels[i];
			if (bone->parentIndex == -1) continue; // Disregard IK targets.
			bone->localPos = bC->collectivePos;
			bone->localRot = bC->collectiveRot;
			bone->localScale = bC->collectiveScl;

			// DX code
			DXMAT boneLocalXM = GetTransformationMatrix( bone->localPos, bone->localRot, bone->localScale );
			DXMAT boneParentXM = DirectX::XMLoadFloat4x4( &modelMatrixList[bone->parentIndex] );
			DXMAT boneModelXM = DirectX::XMMatrixMultiply( boneLocalXM, boneParentXM);
			DXMAT boneInverseBindXM = DirectX::XMLoadFloat4x4(&bone->inverseBindMatrix);
			DXMAT boneFinalXM = DirectX::XMMatrixMultiply( boneInverseBindXM, boneModelXM );
			boneFinalXM = DirectX::XMMatrixMultiply( globalInverseXM, boneFinalXM );
			DirectX::XMStoreFloat4x4(&bone->posedMatrix, boneFinalXM);
			DirectX::XMStoreFloat4x4(&modelMatrixList[i], boneModelXM);
			modelReference->armature.boneMatricies[i] = bone->posedMatrix;

			//bone->posedMatrix = Mat4(fMatXM).Transpose();
			//modelReference->armature.boneMatricies[i] = bone->posedMatrix;
			//modelMatrixList[i] = Mat4(mMatXM).Transpose();
			//Mat4 boneLocalTx = CalculateMatrix( bone->localPos, bone->localRot, bone->localScale );
			////Mat4 boneLocalTx = Mat4();
			//Mat4 boneModelTx = boneLocalTx * modelMatrixList[bone->parentIndex];
			//Mat4 boneInverseBindTx = bone->inverseBindMatrix;
			//Mat4 boneFinalTx =  boneModelTx * boneInverseBindTx;
			//bone->posedMatrix = boneFinalTx;
			//modelReference->armature.boneMatricies[i] = bone->posedMatrix;
			//modelMatrixList[i] = boneModelTx;
		}
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