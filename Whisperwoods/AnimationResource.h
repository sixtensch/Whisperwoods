#pragma once
#include <string>
#include <CHSL/List.h>
#include "KeyFrame.h"
#include "BasicResource.h"

struct AnimationChannel
{
	std::string channelName;
	cs::List<Vec3KeyFrame> positionKeyFrames;
	cs::List<QuatKeyFrame> rotationKeyFrames;
	cs::List<Vec3KeyFrame> scaleKeyFrames;
};

struct AnimationResource : public BasicResource
{
	AnimationResource() = default;

	AnimationResource(const std::string name) : BasicResource(name)  {
		duration = 0;
	}

	cs::List<AnimationChannel> channels;
	float duration;
};