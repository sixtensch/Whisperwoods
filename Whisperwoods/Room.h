#pragma once
#include "GameObject.h"

class Room : public GameObject
{
	// Map resource here
public:
	Room() = default;

	void Update( float deltaTime ) override {}
};