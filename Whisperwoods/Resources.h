#pragma once

#include <string.h>
#include <unordered_map>

#include "BasicResource.h"
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

private:

	void InitMapList();
	void InitializeTextures();
private:
	static Resources* s_singleton;

	cs::List<ResourceMap> m_resourceMaps;
};

