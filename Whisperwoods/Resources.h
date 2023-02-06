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

// Define functions that COULD be defined if its too wordy to use casting etc etc.
#define GET_TEXTURE(subPath) GetResource(TEXTURE, subPath);
#define GET_SOUND(subPath) GetResource(SOUND, subPath);

class Resources sealed
{
public:
	Resources();
	~Resources();

	static Resources& Get();

	// Gets a const pointer thats only used for reading purposes. Writable pointers are only used when allocating.
	const BasicResource* GetResource(const RESOURCE_TYPES resourceType, std::string subPath) const;

	BasicResource* GetWritableResource(const RESOURCE_TYPES resourceType, std::string subPath) const;

private:

	void InitMapList();

	// Allocates a specific resource type in its specific map and returns a pointer to the allocated memory for modification.
	BasicResource* AllocateResource(const RESOURCE_TYPES resourceType, const std::string subPath, const std::string resourceName);

private:
	static Resources* s_singleton;

	cs::List<ResourceMap> m_resourceMaps;
};

