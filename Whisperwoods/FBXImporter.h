#pragma once
#include <string>
#include "Mesh.h"
#include "Animation.h"

class FBXImporter
{
public:
	static bool ImportFBXStatic(std::string filePath, unique_ptr<MeshStatic>& outMesh);
	static bool ImportFBXRigged(std::string filePath, unique_ptr<MeshRigged>& outMesh);
	static bool ImportFBXAnimations(std::string filePath, cs::List<Animation>& outAnimations);
};

