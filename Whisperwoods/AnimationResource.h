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

struct Animation
{
	std::string name;
	cs::List<AnimationChannel> channels;
	float duration;
};

struct AnimationResource : public BasicResource // Set of animations
{
	AnimationResource() = default;

	AnimationResource(const std::string name) : BasicResource(name)  {}

	cs::List<Animation> animations;
};