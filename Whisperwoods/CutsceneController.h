#pragma once
#include "Cutscene.h"
#include <imgui.h>

class CutsceneController
{

public:
	CutsceneController();

	// Test vars for the timeline
	int selected = 0;
	int keyTime = 0;
	int32_t currentFrame = 0;
	int32_t startFrame = 0;
	int32_t endFrame = 100;
	bool transformOpen = false;
	std::vector<int> keys;
	bool doDelete = false;


	float m_time;
	cs::List<shared_ptr<Cutscene>> m_cutscenes;
	Cutscene activeCutscene;

	void AddCutscene( shared_ptr<Cutscene> cutscene );

	bool CutsceneActive();

	void ActivateCutscene( int index );

	void Update();


};