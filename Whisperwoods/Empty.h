#pragma once
#include "GameObject.h"


class Empty : public GameObject
{

public:
	void Update(float delta_time) override {
		transform.CalculateWorldMatrix();
	}
};