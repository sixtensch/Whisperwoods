#pragma once

#include "Renderer.h"

class Game sealed
{
public:
	Game();
	~Game();

	void Update();

private:
	void ChangeTimeline();
	void SetFutureTime() const;
	void SetCurrentTime() const;

private:
	bool m_futureTime;
};

