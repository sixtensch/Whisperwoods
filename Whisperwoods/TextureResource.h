#pragma once
#include "Core.h"

#include "BasicResource.h"

struct TextureResource : public BasicResource {

	TextureResource() 
		: TextureResource("Default texture resource") {}

	TextureResource(const std::string name)
		: BasicResource(name), texture2D(nullptr) {}

	ComPtr<ID3D11Texture2D> texture2D;
};