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
};

