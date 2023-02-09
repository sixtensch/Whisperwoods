#include "Core.h"
#include "Armature.h"

Armature::Armature()
{
	numBones = -1;
	//initialized = false;
}

Armature::~Armature()
{
	// nothing
}

void Armature::ResolveBoneParentIndicies()
{
	for (size_t i = 0; i < bones.Size(); i++)
	{
		bones[i].parentIndex = -1;
		for (size_t j = 0; j < bones.Size(); j++)
		{
			if (bones[i].parentName == bones[j].name)
			{
				bones[i].parentIndex = j;
				break;
			}
		}
	}
}