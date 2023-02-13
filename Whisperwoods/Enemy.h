#pragma once
#include "MeshRenderable.h"
#include "GameObject.h"


struct Coordinates
{
	float x;
	float y;
};

class Enemy : public GameObject
{
public:
	Enemy(MeshRenderableRigged carcinian);
	~Enemy();

	void Update(/*float dTime*/) override;
	void AddCoordinateToPatrolPath(float x, float y);
	void EmptyPatrolPath(); // In order to re-use enemies, wipe the patrol path and add a new one when player reaches a new room rather than creating a new Enemy object.

private:
	shared_ptr<MeshRenderableRigged> carcinian;
	std::vector<Coordinates> patrolPath; // To save the coordinates mapped in bitmap for patrol path.
	int currentPatrolIndex; // To keep track of where in the patrol path the enemy currently is.
};

