#include "Core.h"
#include "Animator.h"

Quaternion AnimatorAnimation::Squad( Quaternion q0, Quaternion q1, Quaternion q2, Quaternion q3, float t )
{
	DirectX::XMVECTOR Q0 = DirectX::XMVectorSet( (float)q0.x, (float)q0.y, (float)q0.z, (float)q0.w );
	DirectX::XMVECTOR Q1 = DirectX::XMVectorSet( (float)q1.x, (float)q1.y, (float)q1.z, (float)q1.w );
	DirectX::XMVECTOR Q2 = DirectX::XMVectorSet( (float)q2.x, (float)q2.y, (float)q2.z, (float)q2.w );
	DirectX::XMVECTOR Q3 = DirectX::XMVectorSet( (float)q3.x, (float)q3.y, (float)q3.z, (float)q3.w );
	DirectX::XMVECTOR pA = DirectX::XMVectorZero();
	DirectX::XMVECTOR pB = DirectX::XMVectorZero();
	DirectX::XMVECTOR pC = DirectX::XMVectorZero();
	DirectX::XMQuaternionSquadSetup( &pA, &pB, &pC, Q0, Q1, Q2, Q3 );
	DirectX::XMVECTOR OUTPUT = DirectX::XMQuaternionSquad( Q1, pA, pB, pC, t );
	DirectX::XMFLOAT4 FL4;
	DirectX::XMStoreFloat4( &FL4, OUTPUT );
	return Quaternion( FL4.x, FL4.y, FL4.z, FL4.w );
}

Vec3 AnimatorAnimation::GetInterpolatedValue( cs::List<Vec3KeyFrame> keys, float time, float duration )
{
	//int index = (int)((float)t * (float)keys.size());
	int index = 0;
	float actualTime = time * duration;

	for (int i = 0; i < keys.Size(); i++)
	{
		if (keys[i].time > actualTime)
			break;
		index = i;
	}

	if (keys.Size() <= 2)
	{
		return keys[0].value;
	}
	else
	{
		// if the chosen key is ahead of the real time
		if ((keys[index].time / duration) > time)
		{
			// interpolate with the previous key
			int pIndex = ((index - 1) % keys.Size());
			float fT = ((duration * time) - keys[pIndex].time) / (keys[index].time - keys[pIndex].time);
			return Lerp(keys[pIndex].value, keys[index].value, fT); // TODO: Replace with CHSL, when I figure out where it is
		}
		else if ((keys[index].time / duration) < time)
		{
			// Else interpolate with the next key
			int nIndex = ((index + 1) % keys.Size());
			float fT = ((duration * time) - keys[index].time) / (keys[nIndex].time - keys[index].time);
			return Lerp(keys[index].value, keys[nIndex].value, fT); // TODO: ditto
		}
		else
		{
			return keys[index].value;
		}
	}
}

Quaternion AnimatorAnimation::GetInterpolatedValue( cs::List<QuatKeyFrame> keys, float time, float duration )
{
	int index = 0;
	float actualTime = time * duration;
	for (int i = 0; i < keys.Size(); i++)
	{
		if (keys[i].time > actualTime)
			break;
		index = i;
	}
	if (keys.Size() >= 4) // if big enough keyset, use squad
	{
		float realTime = (keys[index].time / duration);
		if (realTime == time)
		{
			// If the chosen key is on time exactly return it
			return keys[index].value;
		}
		else if (realTime > time)
		{
			// if the chosen key is ahead of the real time blend with the last
			int size = keys.Size();
			int q0i = ((index + size - 2) % size);
			int q1i = ((index + size - 1) % size);
			int q2i = ((index + size) % size);
			int q3i = ((index + size + 1) % size);
			float fT = ((duration * time) - keys[q1i].time) / (keys[q2i].time - keys[q1i].time);
			return Squad( keys[q0i].value, keys[q1i].value, keys[q2i].value, keys[q3i].value, fT );
		}
		else
		{
			// if the chosen key is behind the real time blend with the next
			int size = keys.Size();
			int q0i = ((index + size - 1) % size);
			int q1i = ((index + size) % size);
			int q2i = ((index + size + 1) % size);
			int q3i = ((index + size + 2) % size);
			float fT = ((duration * time) - keys[q1i].time) / (keys[q2i].time - keys[q1i].time);
			return Squad( keys[q0i].value, keys[q1i].value, keys[q2i].value, keys[q3i].value, fT );
		}
	}
	else if (keys.Size() >= 3)// otherwise use slerp
	{
		// if the chosen key is ahead of the real time
		if ((keys[index].time / duration) > time)
		{
			// interpolate with the previous key
			int pIndex = ((index - 1) % keys.Size());
			float fT = ((duration * time) - keys[pIndex].time) / (keys[index].time - keys[pIndex].time);
			Quaternion a = keys[pIndex].value;
			Quaternion b = keys[index].value;
			Quaternion output = Quaternion::GetSlerp(a, b, fT);
			return output;
		}
		else if ((keys[index].time / duration) < time)
		{
			// Interpolate with key ahead
			int nIndex = ((index + 1) % keys.Size());
			float fT = ((duration * time) - keys[index].time) / (keys[nIndex].time - keys[index].time);
			Quaternion a = keys[index].value;
			Quaternion b = keys[nIndex].value;
			Quaternion output = Quaternion::GetSlerp(keys[index].value, keys[nIndex].value, fT);
			return output;
		}
		else
		{
			return keys[index].value;
		}
	}
	else
	{
		return keys[0].value;
	}
}

Animator::Animator(ModelRiggedResource* modelReference) : modelReference(modelReference)
{
	// Add tracking channels for all bones (possible optimization target vector)
	for (int i = 0; i < modelReference->armature.bones.Size(); i++)
	{
		combinedChannels.Add(AnimatorCombinedChannel(modelReference->armature.bones[i].name));
	}
	looping = true;
	minInfluenceThreshold = 0.1f;
	playbackSpeed = 1;
	globalTime = 0;
}

void Animator::AddAnimation(Animation* sourceAnimation, float startTime, float speed, float influence)
{
	AnimatorAnimation newAnimation(startTime, speed, influence, sourceAnimation, modelReference);
	loadedAnimations.Add(newAnimation);
}

Vec3 Animator::Lerp(Vec3 a, Vec3 b, float t)
{
	return a * (1.0f - t) + b * t;
}

float Animator::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + b * t;
}

Quaternion Animator::Lerp(Quaternion q0, Quaternion q1, float t)
{
	DirectX::XMVECTOR Q0 = DirectX::XMVectorSet((float)q0.x, (float)q0.y, (float)q0.z, (float)q0.w);
	DirectX::XMVECTOR Q1 = DirectX::XMVectorSet((float)q1.x, (float)q1.y, (float)q1.z, (float)q1.w);
	DirectX::XMVECTOR OUTPUT = DirectX::XMQuaternionSlerp(Q0, Q1, t);
	DirectX::XMFLOAT4 FL4;
	DirectX::XMStoreFloat4(&FL4, OUTPUT);
	return Quaternion(FL4.x, FL4.y, FL4.z, FL4.w);
}

void Animator::CombineAnimations()
{
	if (!loadedAnimations.Size()) return;

	// For first animation weight is always 1
	for (int j = 0; j < loadedAnimations[0].animationChannels.Size(); j++)
	{
		int index = loadedAnimations[0].animationChannels[j].boneIndex;
		if (index == -1) continue;
		// Lerp from one to the other
		combinedChannels[index].collectivePos = loadedAnimations[0].animationChannels[j].iPos;
		combinedChannels[index].collectiveRot = loadedAnimations[0].animationChannels[j].iRot;
		combinedChannels[index].collectiveScl = loadedAnimations[0].animationChannels[j].iScl;
	}

	for (int i = 1; i < loadedAnimations.Size(); i++)
	{
		if (loadedAnimations[i].influence < 0.05f) continue;
		for ( int j = 0; j < loadedAnimations[i].animationChannels.Size(); j++)
		{
		
			int index = loadedAnimations[i].animationChannels[j].boneIndex;
			if (index == -1) continue;
			// Lerp from one to the other
			combinedChannels[index].collectivePos =
				Lerp(combinedChannels[index].collectivePos, loadedAnimations[i].animationChannels[j].iPos,
					loadedAnimations[i].influence);

			combinedChannels[index].collectiveRot = Lerp(
				combinedChannels[index].collectiveRot,
				loadedAnimations[i].animationChannels[j].iRot,
				loadedAnimations[i].influence);

			combinedChannels[index].collectiveScl =
				Lerp(combinedChannels[index].collectiveScl, loadedAnimations[i].animationChannels[j].iScl,
					loadedAnimations[i].influence);
		}
	}
}

DirectX::XMMATRIX Animator::GetTransformationMatrix(Vec3 translation, Quaternion rotation, Vec3 scaling)
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

//Mat4 Animator::CalculateMatrix(Vec3 p_pos, Quaternion p_rotation, Vec3 p_scale)
//{
//	Mat4 rotationMatrix = p_rotation.Matrix();
//	Mat4 scalingMatrix = Mat::scale3(p_scale);
//	Mat4 translationMatrix = Mat::translation3(p_pos);
//	Mat4 identityMatrix = Mat4();
//	return identityMatrix * scalingMatrix * rotationMatrix * translationMatrix;
//}

void Animator::PlayAnimation(int index, float startTime, float influence, bool loop, bool stopOthers)
{
	if (index < loadedAnimations.Size())
	{
		if (stopOthers)
		{
			for (int i = 0; i < loadedAnimations.Size(); i++)
			{
				loadedAnimations[i].influence = 0;
				loadedAnimations[i].isActive = false;
			}
		}
		loadedAnimations[index].influence = influence;
		globalTime = startTime;
		looping = loop;
	}
}

void Animator::StopAnimation(int index)
{
	if (index < loadedAnimations.Size())
	{
		loadedAnimations[index].influence = 0;
		loadedAnimations[index].isActive = false;
	}
}


void Animator::UpdateArmature()
{
	cs::List<DirectX::XMFLOAT4X4> modelMatrixList;
	for (int i = 0; i < modelReference->armature.bones.Size(); i++)
	{
		modelMatrixList.Add(DirectX::XMFLOAT4X4());
	}

	modelReference->armature.boneMatricies.Clear();
	for (int i = 0; i < modelReference->armature.bones.Size(); i++)
	{
		DirectX::XMFLOAT4X4 mat;
		modelReference->armature.boneMatricies.Add(mat);
	}

	Bone* rootBone = &modelReference->armature.bones[0];
	AnimatorCombinedChannel* rootC = &combinedChannels[0];

	rootBone->localPos = rootC->collectivePos;
	rootBone->localRot = rootC->collectiveRot;
	rootBone->localScale = rootC->collectiveScl;

	// DX code
	DXMAT globalInverseXM = DirectX::XMLoadFloat4x4(&modelReference->armature.globalInverseTransform);
	DXMAT rootModelXM = GetTransformationMatrix(rootBone->localPos, rootBone->localRot, rootBone->localScale);
	DXMAT rootInverseBindXM = DirectX::XMLoadFloat4x4(&rootBone->inverseBindMatrix);
	DXMAT rootFinalXM = DirectX::XMMatrixMultiply(rootInverseBindXM, rootModelXM);
	rootFinalXM = DirectX::XMMatrixMultiply(globalInverseXM, rootFinalXM);
	DirectX::XMStoreFloat4x4(&rootBone->posedMatrix, rootFinalXM);
	DirectX::XMStoreFloat4x4(&modelMatrixList[0], rootModelXM);
	modelReference->armature.boneMatricies[0] = rootBone->posedMatrix;

	for (int i = 1; i < combinedChannels.Size(); i++)
	{
		Bone* bone = &modelReference->armature.bones[i];
		AnimatorCombinedChannel* bC = &combinedChannels[i];
		if (bone->parentIndex == -1) continue; // Disregard IK targets.
		bone->localPos = bC->collectivePos;
		bone->localRot = bC->collectiveRot;
		bone->localScale = bC->collectiveScl;

		// DX code
		DXMAT boneLocalXM = GetTransformationMatrix(bone->localPos, bone->localRot, bone->localScale);
		DXMAT boneParentXM = DirectX::XMLoadFloat4x4(&modelMatrixList[bone->parentIndex]);
		DXMAT boneModelXM = DirectX::XMMatrixMultiply(boneLocalXM, boneParentXM);
		DXMAT boneInverseBindXM = DirectX::XMLoadFloat4x4(&bone->inverseBindMatrix);
		DXMAT boneFinalXM = DirectX::XMMatrixMultiply(boneInverseBindXM, boneModelXM);
		boneFinalXM = DirectX::XMMatrixMultiply(globalInverseXM, boneFinalXM);
		DirectX::XMStoreFloat4x4(&bone->posedMatrix, boneFinalXM);
		DirectX::XMStoreFloat4x4(&modelMatrixList[i], boneModelXM);
		modelReference->armature.boneMatricies[i] = bone->posedMatrix;
	}
}

void Animator::Update(float deltaTime)
{
	if (modelReference && loadedAnimations.Size())
	{
		globalTime += playbackSpeed * deltaTime;
		if (globalTime > 1.0f) globalTime = (looping) ? 0.0f : 1.0f;
		if (globalTime < 0.0f) globalTime = (looping) ? 1.0f : 0.0f;

		for (int i = 0; i < loadedAnimations.Size(); i++)
		{
			if (loadedAnimations[i].influence < minInfluenceThreshold) continue;

			float combinedOtherInfluences = 0;
			for (int j = i + 1; j < loadedAnimations.Size(); j++)
			{
				combinedOtherInfluences = combinedOtherInfluences + (loadedAnimations[j].influence);
			}
			float effectiveInfluence = 1.0f - combinedOtherInfluences;

			if (effectiveInfluence > minInfluenceThreshold)
			{
				if (!loadedAnimations[i].isActive)
				{
					loadedAnimations[i].isActive = true;
				}
				loadedAnimations[i].time = globalTime;
				loadedAnimations[i].UpdateAnimation(deltaTime, playbackSpeed);
			}
			else
			{
				if (loadedAnimations[i].isActive)
				{
					loadedAnimations[i].isActive = false;
					loadedAnimations[i].time = 0;
				}
			}
		}

		CombineAnimations();
		UpdateArmature();
	}
}

bool Animator::IsPlaying(int index)
{
	if (loadedAnimations.Size() > index && loadedAnimations[index].isActive)
		return true;
	return false;
}

bool Animator::AnimationsFinished()
{
	if (!looping && ((globalTime >= 1 && playbackSpeed >= 0) || (globalTime <= 0 && playbackSpeed < 0)))
		return true;
	return false;
}