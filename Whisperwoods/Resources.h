#pragma once
#include "Core.h"

#include <filesystem>
#include <string.h>
#include <unordered_map>

#include "BasicResource.h"

#include "Sound.h"
#include "RenderHandler.h"

// Resource headers
struct TextureResource;
struct SoundResource;
struct MaterialResource;
struct ModelStaticResource;
struct ModelRiggedResource;
struct AnimationResource;


enum ResourceType 
{
	ResourceTypeTexture,
	ResourceTypeSound,
	ResourceTypeMaterial,
	ResourceTypeModelStatic,
	ResourceTypeModelRigged,
	ResourceTypeMap,
	ResourceTypeAnimations,

	ResourceTypeCount // <-- Keep last
};

typedef std::unordered_map<std::string, shared_ptr<BasicResource>> ResourceMap;

namespace fs = std::filesystem;

class Resources sealed
{
public:
	Resources();
	~Resources();

	static Resources& Get();

	// This should be how resources are acquired. Writable pointers are used when allocating or when having to write for a special reason.
	BasicResource* GetResource(const ResourceType resourceType, std::string filename) const;

	// This is supposed to be called after all singletons are initialized.
	void LoadAssetDirectory(const RenderCore* const renderCore);


	// Getter Functions
	TextureResource* GetTexture(std::string filename);
	SoundResource* GetSound(std::string filename);
	MaterialResource* GetMaterial(std::string filename);
	ModelStaticResource* GetModelStatic(std::string filename);
	ModelRiggedResource* GetModelRigged(std::string filename);
	AnimationResource* GetAnimation(std::string filename);

	TextureResource* CreateTextureUnorm(RenderCore* renderCore, std::string name, uint8_t* data, uint width, uint height);

private:
	void InitMapList();

	// Allocates a specific resource type in its specific map and returns a pointer to the allocated memory.
	BasicResource* AllocateResource(ResourceType resourceType, const std::string filename, const std::string resourceName);

	// As this searches recursively in directories; make sure that all files in the subdirectories are the correct match with its resource.
	cs::List<fs::path> CollectFilePaths(const std::string& assetDirPath);

	// TODO: Add the option for this. If a file is not found later, its referenced to its default.
	void LoadDefaultResources();

	void LoadBaseResources(const RenderCore* const renderCore);
	void LoadSounds();
	void LoadTextures(const RenderCore* const renderCore);


	void LoadCompositeResources(const RenderCore* const renderCore);
	void LoadMaterialResources();
	void LoadModelStaticResources(const RenderCore* const renderCore);
	void LoadModelRiggedResources(const RenderCore* const renderCore);
	void LoadAnimationsResources();

private:
	static Resources* s_singleton;

	cs::List<ResourceMap> m_resourceMaps;
};

