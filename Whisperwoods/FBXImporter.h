#pragma once
#include <string>
#include "ModelResource.h"
#include "Animation.h"

class FBXImporter
{
public:
	static bool ImportFBXStatic(std::string filePath, unique_ptr<ModelStaticResource>& outMesh);
	static bool ImportFBXRigged(std::string filePath, unique_ptr<ModelRiggedResource>& outMesh);
	static bool ImportFBXAnimations(std::string filePath, cs::List<Animation>& outAnimations);
};

