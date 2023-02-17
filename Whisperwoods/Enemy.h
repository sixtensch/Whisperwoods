#pragma once
#include "MeshRenderable.h"
#include "GameObject.h"
#include "AnimationResource.h"
#include "Animator.h"
#include "Room.h"


//temp include
#include "AudioSource.h"



class Enemy : public GameObject
{
public:
	Enemy(std::string modelResource, std::string animationsPath, Mat4 modelOffset);
	~Enemy();

	void Update(float dTime) override;
	void AddCoordinateToPatrolPath(Vec2 coord, bool enclosed);
	void EmptyPatrolPath(); // In order to re-use enemies, wipe the patrol path and add a new one when player reaches a new room rather than creating a new Enemy object.
	void AddModel(std::string modelResource, std::string animationsPath, Mat4 modelOffset);
	bool SeesPlayer(Vec2 playerPosition, AudioSource &quack, Room &room);
	
	bool m_enemyAlive; // A bool to know if we render/update the enemy or not

	shared_ptr<MeshRenderableRigged> m_carcinian;
	AnimationResource* m_animationSet;
	unique_ptr<Animator> m_characterAnimator;


private:

	// move/rotation variables
	//*****************************
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
	bool m_isMoving;
	bool m_PatrolEnemy;
	int m_idleCounter; // To calculate how many times the idle animation has run, see it as a sleep() function that doesn't actually sleep the system
	bool m_triggerTurn;
	bool m_lookBehind; // to handle idle enemies
	float m_idleAngle;
	Mat4 m_modelOffset;
	//*****************************

	//variables for detection
	//***************************
	//Vec3 m_currentRotation;
	bool m_seesPlayer;
	Vec2 m_forwardVector;
	int m_lastPlayedAnimation;
	float m_timeToGivePlayerAChanceToRunAway;
	const float m_amountOfTimeToRunAway = 1.0f; //how long enemies give player to run away in seconds
	const float m_enemyViewDistance = 8.0f; //how far enemies can see
	const float m_enemyViewAngle = 50.0f; // angle to each side of view vector
	//****************************
};

