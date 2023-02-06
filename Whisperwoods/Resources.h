#pragma once

#include <string.h>
#include <unordered_map>

#include "BasicResource.h"

/*
	Ett till lager av säkerhet kan läggas på datan genom att returnera weak_ptrs. Dessa kan endast bli
	castade till en shared_ptr OM den underliggande shared_ptr faktiskt existerar. Den äger på så sätt aldrig datan.
*/

enum RESOURCE_TYPES {
	SHADER = 0,
	TEXTURE,
	SOUND,
	RESOURCE_COUNT // This has to always be at the end of the enum.
};

typedef std::unordered_map<std::string, shared_ptr<BasicResource>> ResourceMap;

class Resources sealed
{
public:
	Resources();
	~Resources();

	static Resources& Get();

	// This should be how resources are acquired. Writable pointers are used when allocating or when having to write for a special reason.
	const BasicResource* GetResource(const RESOURCE_TYPES resourceType, std::string subPath) const;

	// Available if acquisition of a resource needs to be writable.
	BasicResource* GetWritableResource(const RESOURCE_TYPES resourceType, std::string subPath) const;

private:

	void InitMapList();

	// Allocates specific resource type in its map. Returns a pointer to the allocated memory for loading with data.
	BasicResource* AllocateResource(const RESOURCE_TYPES resourceType, const std::string subPath, const std::string resourceName);

private:
	static Resources* s_singleton;

	cs::List<ResourceMap> m_resourceMaps;
};

