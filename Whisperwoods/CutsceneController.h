#pragma once
#include "Cutscene.h"
#include <imgui.h>

class CutsceneController
{
public:

	CutsceneController();

	bool m_isPlaying;
	// Test vars for the timeline
	int selected = 0;
	int keyTime = 0;
	int32_t currentFrame = 0;
	int32_t startFrame = 0;
	int32_t endFrame = 200;
	bool transformOpen = false;
	std::vector<int> keys;
	bool doDelete = false;

	bool m_cameraKeyOpen;
	bool m_animatorKeyOpen;
	bool m_transformKeyOpen;
	bool m_uiKeyOpen;

	bool m_savingOpen;

	cs::List<bool> channelTabs;

	bool m_cutSceneActive;
	float m_time;
	float m_playbackRate;
	cs::List<shared_ptr<Cutscene>> m_cutscenes;
	Cutscene* activeCutscene;

	void AddCutscene( shared_ptr<Cutscene> cutscene );

	bool CutsceneActive();

	void ActivateCutscene( int index );

	void Update(float deltaTime);

};