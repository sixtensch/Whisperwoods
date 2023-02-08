#pragma once

#include "Renderable.h"
#include "Font.h"
#include "SimpleMath.h"

class TextRenderable : public Renderable
{
public:
	TextRenderable(const wchar_t* m_text, dx::SimpleMath::Vector2 m_fontPos, Font m_font);
	~TextRenderable();
	void Draw(const DrawInfo& drawInfo) const override;

	const wchar_t* GetText();
	dx::SimpleMath::Vector2 GetFontPos();
	Font GetFont();
private:
	const wchar_t* m_text;
	dx::SimpleMath::Vector2 m_fontPos;
	Font m_font;

	
};

