#pragma once
#include "MeshRenderable.h"
#include "GameObject.h"
#include "AnimationResource.h"
#include "Animator.h"




class Enemy : public GameObject
{
public:
	Enemy(std::string modelResource, std::string animationsPath, Mat4 modelOffset);
	~Enemy();

	void Update(float dTime) override;
	void AddCoordinateToPatrolPath(Vec2 coord, bool enclosed);
	void EmptyPatrolPath(); // In order to re-use enemies, wipe the patrol path and add a new one when player reaches a new room rather than creating a new Enemy object.
	void AddModel(std::string modelResource, std::string animationsPath, Mat4 modelOffset);
	bool m_enemyAlive; // A bool to know if we render/update the enemy or not
	
	
	shared_ptr<MeshRenderableRigged> m_carcinian;
	shared_ptr<AnimationResource> m_animationSet;
	unique_ptr<Animator> m_characterAnimator;

	Mat4 m_modelOffset;

private:
	

	std::vector<Vec2> m_patrolPath; // To save the coordinates mapped in bitmap for patrol path.
	int m_currentPatrolIndex; // Which index in vector enemy is currently walking towards.
	Vec2 m_currentPosition; // Where the enemy currently is
	float m_walkingSpeed; // How fast the enemies move
	bool m_enclosedLoop; // If the patrol path is a circle or line, kind of
	int m_indexChanger; // Wether to increase or decrease the vector index
	float m_distanceToPatrolPoint; // To handle edge cases where frames causes the enemy to walk to far
	Vec2 m_walkingDirection; // Which direction enemies walk
	bool m_firstTrigger;
	Vec2 m_lastWalkingDirection;
	bool m_rotateClockWise;
	bool m_rotation;
	float m_rotationSpeed;
	float m_rotationCounter;
	float m_offset;
};

