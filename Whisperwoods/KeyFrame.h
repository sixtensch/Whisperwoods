#pragma once
#include <CHSL/Vector.h>
#include <CHSL/Quaternion.h>

struct KeyFrame
{
	float time;
};

struct Vec3KeyFrame : KeyFrame
{
	cs::Vec3 value;
};

struct QuatKeyFrame : KeyFrame
{
	cs::Quaternion value;
};