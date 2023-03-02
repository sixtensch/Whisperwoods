#pragma once
#include "Renderable.h"
#include "ModelResource.h"
#include "TextureResource.h"

class GUIRenderable : public WorldRenderable
{
public:
	GUIRenderable(uint id, const ModelStaticResource* modelStatic, Mat4 worldMatrix);
	void Draw(const DrawInfo& drawInfo) const override;
	TextureResource* spriteRef;
private:
	const ModelStaticResource* m_model;
};
