#pragma once
#include <string>
#include <CHSL/List.h>
#include "KeyFrame.h"

struct AnimationChannel
{
	std::string channelName;
	cs::List<Vec3KeyFrame> positionKeyFrames;
	cs::List<QuatKeyFrame> rotationKeyFrames;
	cs::List<Vec3KeyFrame> scaleKeyFrames;
};

struct AnimationResource // TODO: Derive from basic resource
{
	std::string name;
	cs::List<AnimationChannel> channels;
	float duration;
};