#pragma once
#include "Cutscene.h"

class CutsceneController
{

public:

	float m_time;

	cs::List<shared_ptr<Cutscene>> m_cutscenes;

	void AddCutscene( shared_ptr<Cutscene> cutscene );

	bool CutsceneActive();

	void ActivateCutscene( int index );

	void Update();


};