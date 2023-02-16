#pragma once
#include <string>
#include "ModelResource.h"
#include "AnimationResource.h"

template<class T>
T base_name(T const& path, T const& delims = "/\\")
{
	return path.substr(path.find_last_of(delims) + 1);
}
template<class T>
T remove_extension(T const& filename)
{
	typename T::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}

class FBXImporter
{
public:
	static bool ImportFBXStatic(std::string filePath, ModelStaticResource* const outMesh);
	static bool ImportFBXRigged(std::string filePath, ModelRiggedResource* const outMesh);
	static bool ImportFBXAnimations(std::string filePath, AnimationResource* const outAnimations);

	std::string SaveWMM(ModelStaticResource* inMesh, std::string subDir);
	std::string SaveWMM(ModelRiggedResource* inMesh, std::string subDir);
	static bool LoadWWMStatic(std::string filePath, ModelStaticResource* const outMesh);
	static bool LoadWWMRigged(std::string filePath, ModelRiggedResource* const outMesh);

	std::string SaveWWA( AnimationResource* inAnimations, std::string subDir );
	static bool LoadWWA( std::string filePath, AnimationResource* const outAnimations );
};

