#pragma once

class Window;
class RenderCore;

class GUI sealed
{
public:
	GUI() = delete;
	GUI(const Window* window, const RenderCore* renderCore, bool docking = true, bool viewports = false);
	~GUI();

	void BeginDraw();
	void EndDraw();
};

