#include "Core.h"
#include "Resources.h"

#include <filesystem>

#include "TextureResource.h"
#include "ShaderResource.h"
#include "SoundResource.h"
#include "ModelResource.h"
#include "FBXImporter.h"

#define MODELS_PATH "Assets/Models/"
#define TEXTURE_PATH "Assets/Textures/"
#define SOUND_PATH "Assets/Sounds/"
#define MATERIAL_PATH "Assets/Materials/"
#define MAP_PATH "Assets/Maps/"

// TODO: Dudd includes. Remove later.
namespace fs = std::filesystem;
#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_errors.h>
#include "Sound.h"




Resources* Resources::s_singleton = nullptr;

Resources::Resources()
	: m_resourceMaps({})
{
	if (s_singleton != nullptr)
	{
		throw "Resources singleton re-initialization.";	// TODO: Proper exceptions
	}

	s_singleton = this;

	InitMapList();

	// TODO: Dudd code. Remove later.
	//FBXImporter importer;
	//AllocateResource(ResourceTypeTexture, "TestPath/Test", "Test name");
	//ModelStaticResource* shadiiTestModel = static_cast<ModelStaticResource*>(AllocateResource(ResourceTypeModelStatic, "Characters/ShadiiTest.fbx", "Test name"));
	//importer.ImportFBXStatic("Assets/Models/Characters/ShadiiTest.fbx", shadiiTestModel);
	//shadiiTestModel->CreateVertexBuffer()

	//ShaderResource* shader = (ShaderResource*)AllocateResource(ResourceTypeShader, "VSMesh.cso", "Test name");
	//shader->shaderVar
}

Resources::~Resources()
{
	s_singleton = nullptr;
}

Resources& Resources::Get()
{
#ifdef WW_DEBUG
	if (s_singleton == nullptr)
	{
		throw "Resources singleton not found.";	// TODO: Proper exceptions
	}
#endif

	return *s_singleton;
}

void Resources::InitMapList()
{
	for (int i = 0; i < ResourceTypeCount; i++)
	{
		m_resourceMaps.Add({});
	}
}

void Resources::LoadAssetDirectory()
{
	LoadSounds();

	int a = 0;
}

const BasicResource* Resources::GetResource(const ResourceType resourceType, const std::string subPath) const
{
	return (const BasicResource*)GetWritableResource(resourceType, subPath);
}

BasicResource* Resources::GetWritableResource(const ResourceType resourceType, std::string subPath) const
{
	auto& resourceMap = m_resourceMaps[resourceType];
	auto it = resourceMap.find(subPath);

	if (it != resourceMap.end())
	{
		return it->second.get();
	}

	return nullptr;
}

BasicResource* Resources::AllocateResource(ResourceType resourceType, const std::string subPath, const std::string resourceName)
{
	std::shared_ptr<BasicResource> resource = nullptr;

	switch (resourceType)
	{
	case ResourceTypeShader:
		resource = make_shared<ShaderResource>(resourceName);
		break;

	case ResourceTypeTexture:
		resource = make_shared<TextureResource>(resourceName);
		break;

	case ResourceTypeSound:
		resource = make_shared<SoundResource>(resourceName);
		break;

	case ResourceTypeModelStatic:
		resource = make_shared<ModelStaticResource>();
		break;

	default:
		return nullptr; // If not valid type, return nullptr.
		break;
	}

	auto& resourceMap = m_resourceMaps[resourceType]; 
	const auto& returnIt = resourceMap.insert({ subPath, std::move(resource) });

	bool isSuccessful = returnIt.second;
	// Some complicated type that boils down to a Pair structure as { key (string), value (shared ptr) }
	const auto& insertionIt = returnIt.first;

	if (!isSuccessful)
	{
		std::string blockerName = insertionIt->second.get()->name;
		std::string blockerPath = insertionIt->first;
		EXC("Failed to create resource of type '%s' with path '%s'. Prevented by resource '%s' with path '%s'", 
			resourceName.c_str(), subPath.c_str(), blockerName.c_str(), blockerPath.c_str());
	}
	
	return insertionIt->second.get();
}

void Resources::LoadSounds()
{
	auto sound = Sound::Get();
	std::string path = SOUND_PATH;

	for (const auto& file : fs::directory_iterator(path))
	{
		const fs::path& rawFilePath = file.path();

		if (fs::is_regular_file(rawFilePath))
		{
			std::string filePath = rawFilePath.string();
			SoundResource* soundResource = (SoundResource*)AllocateResource(ResourceTypeSound, filePath, rawFilePath.filename().string());
			
			if (!sound.LoadSound(filePath, soundResource))
			{
				EXC("Failed to load sound %s.", filePath.c_str());
			}
			
			// TODO: Dudd test code that works.
			//sound.PlaySoundA(soundResource->currentSound, {}, {}, 10.0f);
		}
	}
}

