#pragma once

class Window;
class RenderCore;

class DebugGUI sealed
{
public:
	DebugGUI() = delete;
	DebugGUI(const RenderCore* renderCore, bool docking = true, bool viewports = false);
	~DebugGUI();

	void BeginDraw();
	void EndDraw();
};

