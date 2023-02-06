#include "core.h"
#include "Resources.h"

#include "TextureResource.h"
#include "ShaderResource.h"
#include "SoundResource.h"



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
	AllocateResource(TEXTURE, "TestPath/Test", "Test name");
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
	for (int i = 0; i < RESOURCE_COUNT; i++)
	{
		m_resourceMaps.Add({});
	}
}

const BasicResource* Resources::GetResource(const RESOURCE_TYPES resourceType, const std::string subPath) const
{
	auto& resourceMap = m_resourceMaps[resourceType];
	auto it = resourceMap.find(subPath);

	if (it != resourceMap.end())
	{
		return it->second.get();
	}

	return nullptr;
}

BasicResource* Resources::AllocateResource(const RESOURCE_TYPES resourceType, const std::string subPath, const std::string resourceName)
{
	std::shared_ptr<BasicResource> resource = nullptr;

	switch (resourceType)
	{
	case SHADER:
		resource = make_shared<ShaderResource>(resourceName);
		break;

	case TEXTURE:
		resource = make_shared<TextureResource>(resourceName);
		break;

	case SOUND:
		resource = make_shared<SoundResource>(resourceName);
		break;

	default:
		return nullptr; // If not valid type, return nullptr.
		break;
	}

	auto& resourceMap = m_resourceMaps[resourceType]; 
	auto returnIt = resourceMap.insert({ subPath, std::move(resource) });

	bool isSuccessful = returnIt.second;
	const auto& insertionIt = returnIt.first;

	if (isSuccessful == false)
	{
		std::string blockerName = insertionIt->second.get()->resourceName;
		std::string blockerPath = insertionIt->first;
		EXC("Failed to create resource of type '%s' with path '%s'. Prevented by resource '%s' with path '%s'", 
			resourceName.c_str(), subPath.c_str(), blockerName.c_str(), blockerPath.c_str());
	}
	
	return insertionIt->second.get();
}



