#include "Core.h"
#include "CutsceneChannel.h"

Quaternion CutsceneChannel::Squad(Quaternion q0, Quaternion q1, Quaternion q2, Quaternion q3, float t)
{
	DirectX::XMVECTOR Q0 = DirectX::XMVectorSet((float)q0.x, (float)q0.y, (float)q0.z, (float)q0.w);
	DirectX::XMVECTOR Q1 = DirectX::XMVectorSet((float)q1.x, (float)q1.y, (float)q1.z, (float)q1.w);
	DirectX::XMVECTOR Q2 = DirectX::XMVectorSet((float)q2.x, (float)q2.y, (float)q2.z, (float)q2.w);
	DirectX::XMVECTOR Q3 = DirectX::XMVectorSet((float)q3.x, (float)q3.y, (float)q3.z, (float)q3.w);
	DirectX::XMVECTOR pA = DirectX::XMVectorZero();
	DirectX::XMVECTOR pB = DirectX::XMVectorZero();
	DirectX::XMVECTOR pC = DirectX::XMVectorZero();
	DirectX::XMQuaternionSquadSetup(&pA, &pB, &pC, Q0, Q1, Q2, Q3);
	DirectX::XMVECTOR OUTPUT = DirectX::XMQuaternionSquad(Q1, pA, pB, pC, t);
	DirectX::XMFLOAT4 FL4;
	DirectX::XMStoreFloat4(&FL4, OUTPUT);
	return Quaternion(FL4.x, FL4.y, FL4.z, FL4.w);
}

Vec3 CutsceneChannel::GetInterpolatedValue(cs::List<Vec3KeyFrame>& keys, float time, float duration)
{
	if (keys.Size() == 0) return Vec3();
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

Quaternion CutsceneChannel::GetInterpolatedValue(cs::List<QuatKeyFrame>& keys, float time, float duration)
{
	if (keys.Size() == 0) return Quaternion();
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
			return Squad(keys[q0i].value, keys[q1i].value, keys[q2i].value, keys[q3i].value, fT);
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
			return Squad(keys[q0i].value, keys[q1i].value, keys[q2i].value, keys[q3i].value, fT);
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

Vec3 CutsceneChannel::Lerp(Vec3 a, Vec3 b, float t)
{
	return a * (1.0f - t) + b * t;
}

float CutsceneChannel::Lerp( float a, float b, float t )
{
	return a * (1.0f - t) + b * t;
}

float CutsceneChannel::GetInterpolatedValue( cs::List<FloatKeyFrame>& keys, float time, float duration )
{
	if (keys.Size() == 0) return 0;
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
			return Lerp( keys[pIndex].value, keys[index].value, fT ); // TODO: Replace with CHSL, when I figure out where it is
		}
		else if ((keys[index].time / duration) < time)
		{
			// Else interpolate with the next key
			int nIndex = ((index + 1) % keys.Size());
			float fT = ((duration * time) - keys[index].time) / (keys[nIndex].time - keys[index].time);
			return Lerp( keys[index].value, keys[nIndex].value, fT ); // TODO: ditto
		}
		else
		{
			return keys[index].value;
		}
	}
}

void CutsceneCameraChannel::Update(float animationTime, float durationRef)
{
	cs::List<Vec3KeyFrame> positionFrames;
	cs::List<QuatKeyFrame> rotationFrames;

	for (int i = 0; i < keys.Size(); i++)
	{
		CutsceneCameraKey* key = &keys[i];
		Vec3KeyFrame newPosKey;
		newPosKey.time = (float)key->frame;
		newPosKey.value = key->pos;
		positionFrames.Add(newPosKey);

		QuatKeyFrame newRotKey;
		newRotKey.time = (float)key->frame;
		newRotKey.value = key->rot;
		rotationFrames.Add(newRotKey);
	}

	if (keys.Size() > 0)
	{
		Vec3 interpolatedPos = GetInterpolatedValue( positionFrames, animationTime, durationRef );
		Quaternion interpolatedRot = GetInterpolatedValue( rotationFrames, animationTime, durationRef );

		targetCamera->SetPosition( interpolatedPos );
		targetCamera->SetRotation( interpolatedRot );
	}
}

void CutsceneTransformChannel::Update(float animationTime, float durationRef)
{
	cs::List<Vec3KeyFrame> positionFrames;
	cs::List<QuatKeyFrame> rotationFrames;
	for (int i = 0; i < keys.Size(); i++)
	{
		CutsceneTransformKey* key = &keys[i];
		Vec3KeyFrame newPosKey;
		newPosKey.time = (float)key->frame;
		newPosKey.value = key->pos;
		positionFrames.Add(newPosKey);

		QuatKeyFrame newRotKey;
		newRotKey.time = (float)key->frame;
		newRotKey.value = key->rot;
		rotationFrames.Add(newRotKey);
	}
	if (keys.Size() > 0)
	{
		Vec3 interpolatedPos = GetInterpolatedValue( positionFrames, animationTime, durationRef );
		Quaternion interpolatedRot = GetInterpolatedValue( rotationFrames, animationTime, durationRef );

		targetTransform->position = interpolatedPos;
		targetTransform->rotation = interpolatedRot;
	}
}

void CutsceneAnimatorChannel::Update(float animationTime, float durationRef)
{
	int currentFrameInt = (int)(animationTime * (float)durationRef);
	for (int i = 0; i < keys.Size(); i++)
	{
		if (keys[i].frame == currentFrameInt)
		{
			CutsceneAnimationTriggerKey* key = &keys[i];
			if (!targetAnimator->IsPlaying(key->targetAnimation))
			{
				targetAnimator->playbackSpeed = key->animationSpeed;
				targetAnimator->PlayAnimation(key->targetAnimation, key->animationTime, 1.0f, key->loopAnimation, true);
				LOG_TRACE("Play: %d", key->targetAnimation);
			}
			break;
		}
	}
}

void CutsceneGUIChannel::Update( float animationTime, float durationRef )
{
	int currentFrameInt = (int)(animationTime * (float)durationRef);	
	cs::List<FloatKeyFrame> alphaFrames;
	cs::List<Vec3KeyFrame> colorFrames;

	for (int i = 0; i < keys.Size(); i++)
	{
		if (keys[i].frame == currentFrameInt)
		{
			if (targetGUI->GetElement(targetGUIElement)->indexValue != keys[i].active)
			{
				targetGUI->GetElement(targetGUIElement)->indexValue = keys[i].active;
				targetGUI->GetElement(targetGUIElement)->secondTexture = targetGUI->GetElement(targetGUIElement)->alternativeImages[(int)keys[i].active];
			}
			
		}
		CutsceneGUITriggerKey* key = &keys[i];

		FloatKeyFrame newAlphaKey;
		newAlphaKey.time = (float)key->frame;
		newAlphaKey.value = key->alpha;
		alphaFrames.Add( newAlphaKey );

		Vec3KeyFrame newColorKey;
		newColorKey.time = (float)key->frame;
		newColorKey.value = key->color;
		colorFrames.Add( newColorKey );
	}
	if (keys.Size() > 0)
	{
		Vec3 interpolatedColor = GetInterpolatedValue( colorFrames, animationTime, durationRef );
		float interpolatedAlpha = GetInterpolatedValue( alphaFrames, animationTime, durationRef );
		targetGUI->GetElement( targetGUIElement )->alpha = interpolatedAlpha;
		targetGUI->GetElement( targetGUIElement )->colorTint = interpolatedColor;
	}
}
