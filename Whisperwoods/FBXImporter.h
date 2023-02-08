#pragma once
#include <string>
#include "ModelResource.h"
#include "AnimationResource.h"

class FBXImporter
{
public:
	static bool ImportFBXStatic(std::string filePath, ModelStaticResource* const outMesh);
	static bool ImportFBXRigged(std::string filePath, ModelRiggedResource* const outMesh);
	static bool ImportFBXAnimations(std::string filePath, AnimationResource* const outAnimations);

	std::string SaveWMM(ModelResource* p_inMesh, bool rigged);
	std::string SaveWMM(ModelStaticResource* inMesh);
	std::string SaveWMM(ModelRiggedResource* inMesh);
	static bool LoadWWMStatic(std::string filePath, ModelStaticResource* outMesh);
	static bool LoadWWMRigged(std::string filePath, ModelRiggedResource* outMesh);
};

