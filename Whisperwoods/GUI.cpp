#include "Core.h"
#include "GUI.h"

GUI::GUI()
{
}

GUI::~GUI()
{
}

void GUI::AddGUIElement(Vec2 position, Vec2 size, TextureResource* sprite)
{
	m_guiElements.Add(shared_ptr<GUIElement>(new GUIElement(position, size, sprite)));
}

void GUI::DrawGUI()
{
	


}
