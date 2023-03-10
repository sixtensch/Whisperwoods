#include "Core.h"
#include "GUI.h"

GUI::GUI()
{
}

GUI::~GUI()
{
}

shared_ptr<GUIElement> GUI::GetElement(int index)
{
	return m_guiElements[index];
}

shared_ptr<GUIElement> GUI::AddGUIElement(Vec2 position, Vec2 size, TextureResource* spriteOne, TextureResource* spriteTwo)
{
	m_guiElements.Add(shared_ptr<GUIElement>(new GUIElement(position, size, spriteOne, spriteTwo)));
	return m_guiElements.Back();
}

void GUI::DrawGUI()
{
	


}