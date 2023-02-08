#include "core.h"
#include "Resources.h"

#include "TextureResource.h"
#include "ShaderResource.h"
#include "SoundResource.h"
#include "ModelResource.h"
#include "FBXImporter.h"

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
	FBXImporter importer;
	AllocateResource(ResourceTypeTexture, "TestPath/Test", "Test name");
	ModelStaticResource* shadiiTestModel = static_cast<ModelStaticResource*>(AllocateResource(ResourceTypeModelStatic, "WWM/ShadiiTest.wwm", "Test name"));	
	ModelRiggedResource* shadiiTestModelRigged = static_cast<ModelRiggedResource*>(AllocateResource(ResourceTypeModelRigged, "WWM/Shadii_Animated.wwm", "Shadii rigged"));

	importer.LoadWWMStatic("Assets/Models/Static/ShadiiTest.wwm", shadiiTestModel);
	importer.LoadWWMRigged("Assets/Models/Rigged/Shadii_Animated.wwm", shadiiTestModelRigged);
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

BasicResource* Resources::GetResource(ResourceType resourceType, const std::string subPath)
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

	case ResourceTypeModelRigged:
		resource = make_shared<ModelRiggedResource>();
		break;

	default:
		return nullptr; // If not valid type, return nullptr.
		break;
	}

	auto& resourceMap = m_resourceMaps[resourceType]; 
	const auto& returnIt = resourceMap.insert({ subPath, std::move(resource) });

	bool isSuccessful = returnIt.second;
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



