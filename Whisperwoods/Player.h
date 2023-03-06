#pragma once
#include "GameObject.h"
#include "ModelResource.h"
#include "Animator.h"
#include "MeshRenderable.h"
#include "AnimationResource.h"
#include "Room.h"

constexpr float RUNNING_STAMINA_DECAY = 2.0f;


class Player : public GameObject
{
	std::string m_modelResource;
	Mat4 m_modelOffset;
	Vec3 m_targetVelocity;
	float m_maxStamina;
	float m_stamina;
	float m_walkSpeed;
	float m_runSpeed;
	float m_animationSpeed;
	bool m_isCrouch;
	bool m_ranOutOfSprint;

	Vec3 Lerp( Vec3 a, Vec3 b, float t )
	{
		return a * (1.0f - t) + b * t;
	}

public:
	
	Vec3 m_velocity;
	bool cameraIsLocked;
	float cameraFollowDistance;
	float cameraFollowTilt;
	Vec3 cameraLookTargetOffset;
	Room* currentRoom;
	float collectiveForwardValue;
	Vec3 sampleVector;
	bool playerInFuture;
	bool hasPickedUpEssenceBloom;

	
	shared_ptr<MeshRenderableRigged> characterModel;
	AnimationResource* animationSet;
	shared_ptr<Animator> characterAnimator;
	
	Vec3 cameraFollowTarget;
	Vec3 cameraLookAtTarget;
	Quaternion cameraLookRotationTarget;


	shared_ptr<AudioSource> m_vegetationSound;
	shared_ptr<AudioSource> m_stepsSound;



	Player() = default;
	Player(std::string modelResource, std::string animationsPath, Mat4 modelOffset);
	void ReloadPlayer();

	void UpdateStamina(float maxStamina);
	void ResetStaminaToMax(float staminaMax);
	float GetCurrentStamina();
	bool IsCrouching();
	bool IsRunning();

	void PlayerMovement(float delta_time, float movementMultiplier);
	void Update(float delta_time) override;

	void CinematicUpdate( float delta_time );


};