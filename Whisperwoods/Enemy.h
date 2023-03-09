#pragma once

#include "MeshRenderable.h"
#include "GameObject.h"
#include "AnimationResource.h"
#include "Animator.h"
#include "Room.h"
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
	bool SeesPlayer(Vec2 playerPosition, Room &room, bool inFuture);
	void ChangeTimelineState(bool isInFuture);
	void EnemySoundUpdate(float dTime, Vec2 playerPosition, float detectLevel);

	bool enemyAlive; // A bool to know if we render/update the enemy or not in the current room

	shared_ptr<MeshRenderableRigged> m_carcinian;
	AnimationResource* m_animationSet;
	unique_ptr<Animator> m_characterAnimator;

	float GetViewAngle() const;
	float GetViewDistance() const;
	Vec2 GetForwardVector() const;
	float GetDistance() const;
	float GetMaxDistance() const;

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
	float m_rotationSpeed = 0.8f;
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
	const float m_enemyViewDistance = 6.0f; //how far enemies can see
	float m_currentViewDistance = 0.0f; 
	const float m_enemyViewAngle = 50.0f; // angle to each side of view vector
	const float m_proximityDetectionLength = 1.25f;
	bool m_startingDetectionAnimation = false;
	//****************************

	//Audio
	//****************************
	bool m_playAmbientSounds = true;
	float m_ambientWaitTime = 0.0f;
	int currentAction = -1;
	bool m_stepAlternation = true;

	float m_walkingVol = 0.7f;
	unique_ptr<AudioSource> m_walkingSource; //use for walking sounds
	float m_closeAmbientVol = 0.2f;
	unique_ptr<AudioSource> m_ambientCloseSource; //use for ambient and idle noises
	float m_farAmbientVol = 0.7f;
	unique_ptr<AudioSource> m_ambientFarSource; //use for ambient and idle noises
	float m_actionVol = 0.7f;
	unique_ptr<AudioSource> m_actionSource; //use for more specific reactions and actions. turn, detect, loss.
	float m_futureVol = 0.3f;
	unique_ptr<AudioSource> m_futureSource; //Use for future sound
	float m_screamVol = 0.5f;
	unique_ptr<AudioSource> m_screamSource; //Use for scream sound
	float m_detectedVol = 1.5f;
	unique_ptr<AudioSource> m_detectedSource; //Use for detection sound

	FMOD::Sound* m_crabClick;
	FMOD::Sound* m_softerIdle;
	FMOD::Sound* m_varyingClicks;
	FMOD::Sound* m_chirpsLow;
	FMOD::Sound* m_chirps;
	FMOD::Sound* m_chirpsLouder;
	FMOD::Sound* m_smallScreetch;
	FMOD::Sound* m_megatron;
	FMOD::Sound* m_theHorror;
	FMOD::Sound* m_bigStep;
	//****************************

	shared_ptr<cs::Random> m_randGen;
};

