#include "core.h"
#include "MaterialResource.h"

#include "RenderCore.h"

int MaterialBundle::AddMaterial(const MaterialResource* material)
{
	materials.Add(material);
	return materials.Size() - 1;
}

bool MaterialBundle::BindIndex(const RenderCore* core, int index) const
{
	if (index >= materials.Size())
	{
		core->UpdateMaterialInfo(nullptr);
		return false;
	}

	core->UpdateMaterialInfo(materials[index]);
	return false;
}
