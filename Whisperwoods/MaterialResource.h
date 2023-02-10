#pragma once

#include "BasicResource.h"
#include "TextureResource.h"

class RenderCore;

struct MaterialResource : public BasicResource
{
	// Do not reorder
	Vec3	diffuse		= Vec3(1.0f, 1.0f, 1.0f);
	float	alpha		= 1.0f;
	Vec3	specular	= Vec3(1.0f, 1.0f, 1.0f);
	float	glossiness	= 10.0f;
	Vec3	emissive	= Vec3(1.0f, 1.0f, 1.0f);
	float	height		= 0.0f;

	const TextureResource* textureDiffuse	= nullptr;
	const TextureResource* textureSpecular	= nullptr;
	const TextureResource* textureEmissive	= nullptr;
	const TextureResource* textureNormal	= nullptr;
};

struct MaterialBundle
{
public:
	MaterialBundle() = default;

	int AddMaterial(const MaterialResource* material);
	bool BindIndex(const RenderCore* core, int index) const;	// Binds material, or the default if null

private:
	cs::List<const MaterialResource*> materials;
};
