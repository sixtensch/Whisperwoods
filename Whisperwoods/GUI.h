#pragma once
#include "TextureResource.h"
#include "Vertex.h"
#include "GUIElement.h"


class GUI
{
	cs::List<shared_ptr<GUIElement>> m_guiElements;

public:
	GUI();
	~GUI();

	shared_ptr<GUIElement> GetElement(int index);

	void AddGUIElement(Vec2 position, Vec2 size, TextureResource* spriteOne, TextureResource* spriteTwo);

	void DrawGUI();
};