#pragma once
#include <string>
#include "Mesh.h"

class FBXImporter
{
public:
	static bool ImportFBXStatic(std::string filePath, unique_ptr<MeshStatic>& outMesh);
	static bool ImportFBXRigged(std::string filePath, unique_ptr<MeshRigged>& outMesh);
};

