#pragma once
#include <string>
#include <CHSL/List.h>
#include "KeyFrame.h"

struct AnimationChannel
{
	std::string channelName;
	cs::List<Vec3KeyFrame> positionKeyFrames;
	cs::List<QuaternionKeyFrame> rotationKeyFrames;
	cs::List<Vec3KeyFrame> scaleKeyFrames;
};

struct Animation
{
	std::string name;
	cs::List<AnimationChannel> channels;
	float duration;
};