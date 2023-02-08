#include "Core.h"
#include "Resources.h"

#include <WICTextureLoader.h>

#include "TextureResource.h"
#include "ShaderResource.h"
#include "SoundResource.h"
#include "ModelResource.h"
#include "FBXImporter.h"


// TODO: Can move this into an array that indexes with resource type as we want to have one path per resource type(?)
#define MODELS_PATH "Assets/Models/"
#define TEXTURE_PATH "Assets/Textures/"
#define SOUND_PATH "Assets/Sounds/"
#define MATERIAL_PATH "Assets/Materials/"
#define MAP_PATH "Assets/Maps/"
#define STATIC_MODEL_PATH "Assets/Models/WWM/"
#define RIGGED_MODEL_PATH "Assets/Models/WWM/"

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

#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	if (FAILED(initialize))
		EXC("Could not initialize WIC texture loader requirements.");
#else
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
		EXC("Could not initialize WIC texture loader requirements.");
#endif
	// TODO: Dudd code. Remove later.
	FBXImporter importer;
	AllocateResource(ResourceTypeTexture, "TestPath/Test", "Test name");
	ModelStaticResource* shadiiTestModel = static_cast<ModelStaticResource*>(AllocateResource(ResourceTypeModelStatic, "WWM/ShadiiTest.wwm", "Test name"));
	
	importer.LoadWWMStatic("Assets/Models/WWM/ShadiiTest.wwm", shadiiTestModel);
	
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

void Resources::LoadAssetDirectory(RenderCore* const renderCore)
{
	LoadBaseResources(renderCore);

	LoadCompositeResources();
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
	case ResourceTypeTexture:
		resource = make_shared<TextureResource>(resourceName);
		break;

	case ResourceTypeSound:
		resource = make_shared<SoundResource>(resourceName);
		break;

		//case ResourceTypeMaterial:
		//	resource = make_shared<MaterialResource>(resourceName);
		//	break;

	
	case ResourceTypeModelStatic:
		resource = make_shared<ModelStaticResource>(resourceName);
		break;

	case ResourceTypeModelRigged:
		resource = make_shared<ModelRiggedResource>(resourceName);
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

cs::List<fs::path> Resources::CollectFilePaths(const std::string& assetDirPath)
{
	cs::List<fs::path> filePaths = {};

	for (const auto& file : fs::directory_iterator(assetDirPath))
	{
		if (fs::is_regular_file(file))
		{
			filePaths.Add(file.path());
		}
	}

	return filePaths;
}

void Resources::LoadBaseResources(RenderCore* const renderCore)
{
	LoadTextures(renderCore);
	LoadSounds();
}

void Resources::LoadSounds()
{
	// TODO: The code below will call the destructor of sound when the function is done, as if it hijacked its instance.
	// Singletons should return pointers instead.
	//auto sound = Sound::Get();

	cs::List<fs::path> soundPaths = CollectFilePaths(SOUND_PATH);

	for (fs::path& path : soundPaths)
	{
		std::string filePath = path.string();
		SoundResource* soundResource = (SoundResource*)AllocateResource(ResourceTypeSound, filePath, path.filename().string());

		if (!Sound::Get().LoadSound(filePath, soundResource->currentSound))
		{
			EXC("Failed to load sound '%s'.", filePath.c_str());
		}

		// TODO: Dudd test code that works. Remove later.
		//sound.PlaySoundA(soundResource->currentSound, {}, {}, 10.0f);
	}
}

void Resources::LoadTextures(RenderCore* renderCore)
{
	cs::List<fs::path> texturePaths = CollectFilePaths(TEXTURE_PATH);

	for (fs::path& path : texturePaths)
	{
		std::string filePath = path.string();
		TextureResource* textureResource = (TextureResource*)AllocateResource(ResourceTypeTexture, filePath, path.filename().string());
	
		//if (!renderCore->LoadTexture(filePath, textureResource))
		//{
		//	EXC("Failed to load texture '%s'.", filePath.c_str());
		//}
	}
}

void Resources::LoadCompositeResources()
{
	LoadMaterialResources();
	LoadModelStaticResources();
	LoadModelRiggedResources();
}

void Resources::LoadMaterialResources()
{

}

void Resources::LoadModelStaticResources()
{
	FBXImporter importer;

	cs::List<fs::path> staticModelPaths = CollectFilePaths(STATIC_MODEL_PATH);
	
	for (fs::path& path : staticModelPaths)
	{
		std::string filePath = path.string();
		ModelStaticResource* modelStaticResource = (ModelStaticResource*)AllocateResource(ResourceTypeModelStatic, filePath, path.filename().string());
	
		FBXImporter::LoadWWMStatic(filePath, modelStaticResource);
	}

}

void Resources::LoadModelRiggedResources()
{
	FBXImporter importer;

	cs::List<fs::path> staticModelPaths = CollectFilePaths(STATIC_MODEL_PATH);

	for (fs::path& path : staticModelPaths)
	{
		std::string filePath = path.string();
		ModelRiggedResource* modelRiggedResource = (ModelRiggedResource*)AllocateResource(ResourceTypeModelRigged, filePath, path.filename().string());

		FBXImporter::LoadWWMRigged(filePath, modelRiggedResource);
	}
}

