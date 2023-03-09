#include "Core.h"
#include "WWCBuilder.h"
#include <fstream>
#include <filesystem>
#include <direct.h>

std::string SaveWWC( Cutscene* cutScene )
{
	std::string subDir = "Assets/Cutscenes/";
	// Use model name as base for out name
	std::string outName = subDir + cutScene->name + ".wwc";
	_mkdir(subDir.c_str());
	// Open the writer
	std::ofstream writer(outName, std::ios::out | std::ios::binary);

	if (!writer)
	{
		LOG_WARN("WWC Writer failed to open file: %s", outName.c_str());
		return "fail";
	}

	WhisperWoodsCutsceneHead bundleHead;
	size_t bundleNameLength = (cutScene->name.length() <= 128) ? cutScene->name.length() : 128;
	for (size_t j = 0; j < bundleNameLength; j++)
	{
		bundleHead.name[j] = cutScene->name[j];
	}
	bundleHead.numChannels = cutScene->channels.Size();

	// Write the bundle head.
	writer.write((char*)&bundleHead, sizeof(WhisperWoodsCutsceneHead));

	for (int i = 0; i < bundleHead.numChannels; i++)
	{
		WhisperWoodsCutsceneChannelHead channelHead;
		CutsceneChannel* channel = cutScene->channels[i].get();
		if (!channel) return "fail, channel null";

		size_t channelNameLength = (cutScene->channels[i]->name.length() <= 128) ? cutScene->channels[i]->name.length() : 128;
		for (size_t j = 0; j < channelNameLength; j++)
		{
			channelHead.name[j] = cutScene->channels[i]->name[j];
		}
		channelHead.channelType = channel->channelType;
		channelHead.numKeys = channel->numKeys;

		// Write the channel head information
		writer.write((char*)&channelHead, sizeof(WhisperWoodsCutsceneChannelHead));

		// Write the channel keys depending on the type.
		if (channelHead.channelType == CutsceneTypeAnimator)
		{
			CutsceneAnimatorChannel* cChannel = dynamic_cast<CutsceneAnimatorChannel*>(channel);
			writer.write((char*)cChannel->keys.Data(), sizeof(CutsceneAnimationTriggerKey) * channelHead.numKeys);
		}
		else if (channelHead.channelType == CutsceneTypeTransform)
		{
			CutsceneTransformChannel* cChannel = dynamic_cast<CutsceneTransformChannel*>(channel);
			writer.write((char*)cChannel->keys.Data(), sizeof(CutsceneTransformKey) * channelHead.numKeys);
		}
		else if (channelHead.channelType == CutsceneTypeCamera)
		{
			CutsceneCameraChannel* cChannel = dynamic_cast<CutsceneCameraChannel*>(channel);
			writer.write((char*)cChannel->keys.Data(), sizeof(CutsceneCameraKey) * channelHead.numKeys);
		}
		else if (channelHead.channelType == CutsceneTypeGUI)
		{
			CutsceneGUIChannel* cChannel = dynamic_cast<CutsceneGUIChannel*>(channel);
			writer.write((char*)cChannel->keys.Data(), sizeof(CutsceneGUITriggerKey) * channelHead.numKeys);
		}
		else if (channelHead.channelType == CutsceneTypeText)
		{
			CutsceneTextChannel* cChannel = dynamic_cast<CutsceneTextChannel*>(channel);
			writer.write((char*)cChannel->keys.Data(), sizeof(CutsceneTextTriggerKey) * channelHead.numKeys);
		}

		//for (int j = 0; j < channel->keys.Size(); j++)
		//{
		//	CutsceneKey* key = channel->keys[j].get();
		//	if (channelHead.channelType == CutsceneTypeAnimator)
		//		writer.write((char*)&key, sizeof(CutsceneAnimationTriggerKey));
		//	else if (channelHead.channelType == CutsceneTypeTransform)
		//		writer.write((char*)&key, sizeof(CutsceneTransformKey));
		//	else if (channelHead.channelType == CutsceneTypeCamera)
		//		writer.write((char*)&key, sizeof(CutsceneCameraKey));
		//	else if (channelHead.channelType == CutsceneTypeGUI)
		//		writer.write((char*)&key, sizeof(CutsceneGUITriggerKey));
		//	else if (channelHead.channelType == CutsceneTypeText)
		//		writer.write((char*)&key, sizeof(CutsceneTextTriggerKey));
		//}
	}
	
	// Close the writer
	writer.close();

	// If an error occured
	if (!writer.good()) {
		LOG_WARN("WWC Writer error occurred at writing time! , EOF: %d, FAILBIT: %d, BADBIT: %d", writer.eof(), writer.fail(), writer.bad());
		return "fail";
	}

	return outName;
}

bool LoadWWC(Cutscene* outScene, std::string filePath)
{
	LOG_TRACE("Loading WWC: %s ...", filePath.c_str());
	std::ifstream reader(filePath, std::ios::out | std::ios::binary);
	if (!reader)
	{
		LOG_WARN("WWC Reader failed to open file: %s", filePath.c_str());
		return false;
	}
	//Animation readAnimation;

	// Read the head data.
	WhisperWoodsCutsceneHead cutsceneHead;
	reader.read((char*)&cutsceneHead, sizeof(WhisperWoodsCutsceneHead));
	outScene->name = std::string(cutsceneHead.name);

	for (int i = 0; i < cutsceneHead.numChannels; i++)
	{
		WhisperWoodsCutsceneChannelHead channelHead;
		reader.read((char*)&channelHead, sizeof(WhisperWoodsCutsceneChannelHead));
		
		if (channelHead.channelType == CutsceneTypeAnimator)
		{
			//outScene->AddChannel(shared_ptr<CutsceneAnimatorChannel>(new CutsceneAnimatorChannel(channelHead.name, nullptr)));
			outScene->AddChannel(make_shared<CutsceneAnimatorChannel>(channelHead.name, nullptr));
			CutsceneAnimatorChannel* channel = (CutsceneAnimatorChannel*)(outScene->channels[outScene->channels.Size() - 1].get());

			reader.read((char*)channel->keys.MassAdd(channelHead.numKeys), sizeof(CutsceneAnimationTriggerKey) * channelHead.numKeys);
			channel->numKeys = channelHead.numKeys;
		}
		else if (channelHead.channelType == CutsceneTypeTransform)
		{
			outScene->AddChannel(make_shared<CutsceneTransformChannel>(channelHead.name, nullptr));
			CutsceneTransformChannel* channel = (CutsceneTransformChannel*)outScene->channels[outScene->channels.Size() - 1].get();

			reader.read((char*)channel->keys.MassAdd(
				channelHead.numKeys), sizeof(CutsceneTransformKey) * channelHead.numKeys);
			channel->numKeys = channelHead.numKeys;
		}
		else if (channelHead.channelType == CutsceneTypeCamera)
		{
			outScene->AddChannel(make_shared<CutsceneCameraChannel>(channelHead.name, nullptr));
			CutsceneCameraChannel* channel = (CutsceneCameraChannel*)outScene->channels[outScene->channels.Size() - 1].get();

			reader.read((char*)channel->keys.MassAdd(
				channelHead.numKeys), sizeof(CutsceneCameraKey) * channelHead.numKeys);
			channel->numKeys = channelHead.numKeys;
		}
		else if (channelHead.channelType == CutsceneTypeGUI)
		{
			outScene->AddChannel(make_shared<CutsceneGUIChannel>(channelHead.name, nullptr));
			CutsceneGUIChannel* channel = (CutsceneGUIChannel*)outScene->channels[outScene->channels.Size() - 1].get();

			reader.read((char*)channel->keys.MassAdd(
				channelHead.numKeys), sizeof(CutsceneGUITriggerKey) * channelHead.numKeys);
			channel->numKeys = channelHead.numKeys;
		}
		else if (channelHead.channelType == CutsceneTypeText)
		{
			outScene->AddChannel(make_shared<CutsceneTextChannel>());
			CutsceneTextChannel* channel = (CutsceneTextChannel*)outScene->channels[outScene->channels.Size() - 1].get();

			channel->name = std::string(channelHead.name);
			reader.read((char*)channel->keys.MassAdd(
				channelHead.numKeys), sizeof(CutsceneTextTriggerKey) * channelHead.numKeys);
			channel->numKeys = channelHead.numKeys;
		}

	}

	// Close reader
	reader.close();

	if (!reader.good())
	{
		LOG_WARN("WWC Rigged reader, error found at close, data might be garbled.");
		return false;
	}
	LOG_TRACE("DONE");

	return true;
}

/*
bool FBXImporter::LoadWWA(std::string filePath, AnimationResource* const outAnimations)
{
	LOG_TRACE("Loading WWA: %s ...", filePath.c_str());
	std::ifstream reader(filePath, std::ios::out | std::ios::binary);
	if (!reader)
	{
		LOG_WARN("WWA Reader failed to open file: %s", filePath.c_str());
		return false;
	}

	// Read the head data.
	WhisperWoodsAnimationsHead bundleHead;
	reader.read((char*)&bundleHead, sizeof(WhisperWoodsAnimationsHead));
	outAnimations->name = std::string(bundleHead.name);

	// Read the animations
	for (int i = 0; i < bundleHead.numAnimations; i++)
	{
		WhisperWoodsAnimationHead animationHead;
		Animation readAnimation;
		reader.read((char*)&animationHead, sizeof(WhisperWoodsAnimationHead));
		readAnimation.name = std::string(animationHead.name);
		readAnimation.duration = animationHead.duration;

		// Read the channels
		for (int j = 0; j < animationHead.numChannels; j++)
		{
			WhisperWoodsAnimationChannelHead channelHead;
			AnimationChannel readChannel;
			reader.read((char*)&channelHead, sizeof(WhisperWoodsAnimationChannelHead));
			readChannel.channelName = std::string(channelHead.name);

			// read the pos keyframes.
			reader.read((char*)readChannel.positionKeyFrames.MassAdd(
				channelHead.numPositionKeyFrames), sizeof(Vec3KeyFrame) * channelHead.numPositionKeyFrames);

			reader.read((char*)readChannel.rotationKeyFrames.MassAdd(
				channelHead.numRotationKeyFrames), sizeof(QuatKeyFrame) * channelHead.numRotationKeyFrames);

			reader.read((char*)readChannel.scaleKeyFrames.MassAdd(
				channelHead.numScaleKeyFrames), sizeof(Vec3KeyFrame) * channelHead.numScaleKeyFrames);

			readAnimation.channels.Add(readChannel);
		}
		outAnimations->animations.Add(readAnimation);
	}

	// Close reader
	reader.close();

	if (!reader.good())
	{
		LOG_WARN("WMM Rigged reader, error found at close, data might be garbled.");
		return false;
	}
	LOG_TRACE("DONE");

	return true;
}
*/




/*
std::string FBXImporter::SaveWWA(AnimationResource* inAnimations, std::string subDir)
{
	// Use model name as base for out name
	std::string outName = subDir + inAnimations->name + ".wwa";
	_mkdir(subDir.c_str());
	// Open the writer
	std::ofstream writer(outName, std::ios::out | std::ios::binary);

	if (!writer)
	{
		LOG_WARN("WMA Writer failed to open file: %s", outName.c_str());
		return "fail";
	}

	WhisperWoodsAnimationsHead bundleHead;
	size_t bundleNameLength = (inAnimations->name.length() <= 128) ? inAnimations->name.length() : 128;
	for (size_t j = 0; j < bundleNameLength; j++)
	{
		bundleHead.name[j] = inAnimations->name[j];
	}
	bundleHead.numAnimations = inAnimations->animations.Size();

	// Write the bundle head.
	writer.write((char*)&bundleHead, sizeof(WhisperWoodsAnimationsHead));

	// Loop Through all animations.
	for (int i = 0; i < bundleHead.numAnimations; i++)
	{
		WhisperWoodsAnimationHead animationHead;
		Animation* animation = &inAnimations->animations[i];
		size_t animationNameLength = (animation->name.length() <= 128) ? animation->name.length() : 128;
		for (size_t j = 0; j < animationNameLength; j++)
		{
			animationHead.name[j] = animation->name[j];
		}
		animationHead.duration = animation->duration;
		animationHead.numChannels = animation->channels.Size();

		// Write the animation head
		writer.write((char*)&animationHead, sizeof(WhisperWoodsAnimationHead));

		// Loop and write all the channels.
		for (int j = 0; j < animationHead.numChannels; j++)
		{
			WhisperWoodsAnimationChannelHead channelHead;
			AnimationChannel* channel = &animation->channels[j];
			size_t channelNameLength = (channel->channelName.length() <= 128) ? channel->channelName.length() : 128;
			for (size_t k = 0; k < channelNameLength; k++)
			{
				channelHead.name[k] = channel->channelName[k];
			}
			channelHead.numPositionKeyFrames = channel->positionKeyFrames.Size();
			channelHead.numRotationKeyFrames = channel->rotationKeyFrames.Size();
			channelHead.numScaleKeyFrames = channel->scaleKeyFrames.Size();

			// Write the channel head
			writer.write((char*)&channelHead, sizeof(WhisperWoodsAnimationChannelHead));
			// Write all position keyframes.
			writer.write((char*)channel->positionKeyFrames.Data(), sizeof(Vec3KeyFrame) * channelHead.numPositionKeyFrames);
			// Write all rotation keyframes.
			writer.write((char*)channel->rotationKeyFrames.Data(), sizeof(QuatKeyFrame) * channelHead.numRotationKeyFrames);
			// Write all position keyframes.
			writer.write((char*)channel->scaleKeyFrames.Data(), sizeof(Vec3KeyFrame) * channelHead.numScaleKeyFrames);
		}
	}

	// Close the writer
	writer.close();

	// If an error occured
	if (!writer.good()) {
		LOG_WARN("WWA Writer error occurred at writing time! , EOF: %d, FAILBIT: %d, BADBIT: %d", writer.eof(), writer.fail(), writer.bad());
		return "fail";
	}

	// Return the path
	return outName;
}
*/