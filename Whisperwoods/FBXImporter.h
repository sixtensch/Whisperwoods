#pragma once
#include <string>
#include "Mesh.h"

class FBXImporter
{
public:
	static void ImportFBXStatic(std::string filePath, MeshStatic* outMesh);
	static void ImportFBXRigged(std::string filePath, MeshRigged* outMesh);
};

