#include "core.h"
#include "TextRenderable.h"
#include <SimpleMath.h>
#include "RenderCore.h"

TextRenderable::TextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin)
{
	m_font = font;
	m_text = text;
	m_fontPos = fontPos;
	m_color = color;
	m_origin = origin;
}

TextRenderable::~TextRenderable()
{
}

void TextRenderable::Draw(const DrawInfo& drawInfo) const
{
	drawInfo.core->DrawText(m_fontPos, m_text, m_font, m_color, m_origin);
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

Vec2 TextRenderable::GetOrigin()
{
	return m_origin;
}
