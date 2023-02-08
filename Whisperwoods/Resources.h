#pragma once

#include <string.h>
#include <unordered_map>

#include "BasicResource.h"

/*
	Ett till lager av säkerhet kan läggas på datan genom att returnera weak_ptrs. Dessa kan endast bli
	castade till en shared_ptr OM den underliggande shared_ptr faktiskt existerar. Den äger på så sätt aldrig datan.
*/

enum ResourceType 
{
	ResourceTypeShader,
	ResourceTypeTexture,
	ResourceTypeSound,
	ResourceTypeMaterial,
	ResourceTypeModelStatic,
	ResourceTypeModelRigged,
	ResourceTypeMap,

	ResourceTypeCount // <-- Keep last
};

typedef std::unordered_map<std::string, shared_ptr<BasicResource>> ResourceMap;

class Resources sealed
{
public:
	Resources();
	~Resources();

	static Resources& Get();

	// This should be how resources are acquired. Writable pointers are used when allocating or when having to write for a special reason.
	const BasicResource* GetResource(const ResourceType resourceType, std::string subPath) const;

	// Available if acquisition of a resource needs to be writable.
	BasicResource* GetWritableResource(const ResourceType resourceType, std::string subPath) const;

private:
	void InitMapList();

	// Allocates a specific resource type in its specific map and returns a pointer to the allocated memory.
	BasicResource* AllocateResource(ResourceType resourceType, const std::string subPath, const std::string resourceName);

private:
	static Resources* s_singleton;

	cs::List<ResourceMap> m_resourceMaps;
};

