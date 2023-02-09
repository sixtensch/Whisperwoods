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
#define STATIC_MODEL_PATH "Assets/Models/Statics/"
#define RIGGED_MODEL_PATH "Assets/Models/Rigged/"

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

void Resources::LoadAssetDirectory(const RenderCore* const renderCore)
{
	// Should always be called first.
	// Ideally this should be in constructor but cant as loading depends on proper inits of other singletons and such.
	LoadDefaultResources();

	LoadBaseResources(renderCore);

	LoadCompositeResources(renderCore);
}

const BasicResource* Resources::GetResource(const ResourceType resourceType, const std::string subPath) const
{
	return (const BasicResource*)GetWritableResource(resourceType, subPath);
}

BasicResource* Resources::GetWritableResource(const ResourceType resourceType, std::string subPath) const
{
	auto& resourceMap = m_resourceMaps[resourceType];
	auto it = resourceMap.find(subPath);

	if (it == resourceMap.end())
	{
		EXC("Resource '%s' does not exist. Called with ResourceType = %d.", subPath.c_str(), resourceType)
			return nullptr;
	}

	return it->second.get();
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

void Resources::LoadDefaultResources()
{


}

void Resources::LoadBaseResources(const RenderCore* const renderCore)
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
	}
}

void Resources::LoadTextures(const RenderCore* const renderCore)
{
	cs::List<fs::path> texturePaths = CollectFilePaths(TEXTURE_PATH);

	for (fs::path& path : texturePaths)
	{
		std::string filePath = path.string();
		TextureResource* textureResource = (TextureResource*)AllocateResource(ResourceTypeTexture, filePath, path.filename().string());
		
		// Will throw exception if failed.
		renderCore->LoadImageTexture(wstring(filePath.begin(), filePath.end()), textureResource->texture2D);
	}
}

void Resources::LoadCompositeResources(const RenderCore* const renderCore)
{
	LoadMaterialResources();
	LoadModelStaticResources(renderCore);
	LoadModelRiggedResources(renderCore);
}

void Resources::LoadMaterialResources()
{

}

void Resources::LoadModelStaticResources(const RenderCore* const renderCore)
{
	cs::List<fs::path> staticModelPaths = CollectFilePaths(STATIC_MODEL_PATH);
	
	for (fs::path& path : staticModelPaths)
	{
		std::string filePath = path.string();
		ModelStaticResource* modelStaticResource = (ModelStaticResource*)AllocateResource(ResourceTypeModelStatic, filePath, path.filename().string());

		if (!FBXImporter::LoadWWMStatic(filePath, modelStaticResource))
		{
			EXC("Failed to load static resource '%s'.", filePath.c_str());
		}

		const UINT indeciesByteWidth = sizeof(UINT) * modelStaticResource->indicies.Size();
		const UINT verteciesByteWidth = modelStaticResource->GetVertexByteWidth();
		renderCore->CreateIndexBuffer(modelStaticResource->indicies.Data(), indeciesByteWidth, modelStaticResource->indexBuffer.GetAddressOf());
		renderCore->CreateVertexBuffer(modelStaticResource->verticies.Data(), verteciesByteWidth, modelStaticResource->vertexBuffer.GetAddressOf());
	}
}

void Resources::LoadModelRiggedResources(const RenderCore* const renderCore)
{
	cs::List<fs::path> riggedModelPaths = CollectFilePaths(RIGGED_MODEL_PATH);

	for (fs::path& path : riggedModelPaths)
	{
		std::string filePath = path.string();
		ModelRiggedResource* modelRiggedResource = (ModelRiggedResource*)AllocateResource(ResourceTypeModelRigged, filePath, path.filename().string());

		if (!FBXImporter::LoadWWMRigged(filePath, modelRiggedResource))
		{
			EXC("Failed to load rigged resource '%s'.", filePath.c_str());
		}

		const UINT indeciesByteWidth = sizeof(UINT) * modelRiggedResource->indicies.Size();
		const UINT verticesByteWidth = modelRiggedResource->GetVertexByteWidth();
		renderCore->CreateIndexBuffer(modelRiggedResource->indicies.Data(), indeciesByteWidth, modelRiggedResource->indexBuffer.GetAddressOf());
		renderCore->CreateVertexBuffer(modelRiggedResource->verticies.Data(), verticesByteWidth, modelRiggedResource->vertexBuffer.GetAddressOf());
	}
}

