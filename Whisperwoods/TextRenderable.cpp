#include "core.h"
#include "TextRenderable.h"
#include <SimpleMath.h>
#include "RenderCore.h"

TextRenderable::TextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color)
{
	m_font = font;
	m_text = text;
	m_fontPos = fontPos;
	m_color = color;
}

TextRenderable::~TextRenderable()
{
}

void TextRenderable::Draw(const DrawInfo& drawInfo) const
{
	drawInfo.core->DrawText(m_fontPos, m_text, m_font, m_color);
}

const wchar_t* TextRenderable::GetText()
{
	return m_text;
}

dx::SimpleMath::Vector2 TextRenderable::GetFontPos()
{
	return m_fontPos;
}

Font TextRenderable::GetFont()
{
	return m_font;
}

cs::Color4f TextRenderable::GetColor()
{
	return m_color;
}
