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

	cs::List<TextureResource*> alternativeImages;
	char indexValue;

	bool isActive;

	Vec3 colorTint;
	float alpha;
	Vec3 vectorData;
	float floatData;
	Point4 intData;
	Vec2 minimapCurrentRoomPos; // Only used when intData set accordingly to z value of 1.

	TextureResource* firstTexture;
	TextureResource* secondTexture;
	GUIElement(Vec2 position, Vec2 size, TextureResource* spriteOne, TextureResource* spriteTwo) : position(position), size(size), firstTexture(spriteOne), secondTexture(spriteTwo)
	{
		indexValue = 0;
		// Fetch the renderable
		uiRenderable = Renderer::CreateGUIRenderable("ui_rect.wwm");
		// Create matrix
		uiRenderable->worldMatrix =
			Mat::translation3({ position.x, position.y, 0 }) *
			Mat::rotation3(0, 0, 0) *
			Mat::scale3(size.x, size.y, 1);
		// Link reference
		uiRenderable->m_elementRef = this;
	};

	bool TestMouse();

};