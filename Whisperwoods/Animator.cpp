#include "Core.h"
#include "Animator.h"

Vec3 AnimatorAnimation::GetInterpolatedValue( cs::List<Vec3KeyFrame> keys, float time, float duration )
{
	//int index = (int)((float)t * (float)keys.size());
	int index = 0;
	float actualTime = time * duration;

	for (size_t i = 0; i < keys.Size(); i++)
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
	for (size_t i = 0; i < keys.Size(); i++)
	{
		if (keys[i].time > actualTime)
			break;
		index = i;
	}
	if (keys.Size() >= 4) // if big enough keyset, use squad
	{
		// if the chosen key is ahead of the real time
		if ((keys[index].time / duration) > time)
		{
			int q0i = ((index - 2) % keys.Size());
			int q1i = ((index - 1) % keys.Size());
			int q2i = ((index) % keys.Size());
			int q3i = ((index + 1) % keys.Size());
			float fT = ((duration * time) - keys[q1i].time) / (keys[q2i].time - keys[q1i].time);
			//return Quaternion::Squad( keys[q0i].value, keys[q1i].value, keys[q2i].value, keys[q3i].value, fT ); // Wheres my squad at >:V *angry quack*
			return Quaternion::GetSlerp( keys[q1i].value, keys[q2i].value, fT );

		}
		else if ((keys[index].time / duration) < time)
		{
			int q0i = ((index - 1) % keys.Size());
			int q1i = ((index) % keys.Size());
			int q2i = ((index + 1) % keys.Size());
			int q3i = ((index + 2) % keys.Size());
			float fT = ((duration * time) - keys[q1i].time) / (keys[q2i].time - keys[q1i].time);
			//return Quaternion::Squad( keys[q0i].value, keys[q1i].value, keys[q2i].value, keys[q3i].value, fT ); // ditto
			return Quaternion::GetSlerp( keys[q1i].value, keys[q2i].value, fT );
		}
		else
		{
			return keys[index].value;
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