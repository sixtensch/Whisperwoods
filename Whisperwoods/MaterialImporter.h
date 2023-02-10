#pragma once

#include "MaterialResource.h"

class MaterialImporter
{
public:
	static bool ImportWWMT(string filepath, MaterialResource* const outMaterial);
};

