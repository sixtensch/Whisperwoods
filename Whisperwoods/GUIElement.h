#pragma once
#include "TextureResource.h"
#include "Renderer.h"
#include "GUIRenderable.h"

struct GUIElement
{
	// Data for the drawcall
	cs::List<VertexTextured> verticies;
	cs::List<int> indicies;

	shared_ptr<GUIRenderable> uiRenderable;

	Vec2 position;
	Vec2 size;
	TextureResource* spriteReference;
	GUIElement(Vec2 position, Vec2 size, TextureResource* sprite) : position(position), size(size), spriteReference(sprite)
	{
		// Fetch the renderable
		uiRenderable = Renderer::CreateGUIRenderable("ui_rect.wwm");
		// Create matrix
		/*uiRenderable->worldMatrix =
			Mat::translation3({ position.x, position.y, 0 }) *
			Mat::rotation3(0, 0, 0) *
			Mat::scale3(size.x, size.y, 1);*/
		uiRenderable->worldMatrix = Mat4();
	};
};