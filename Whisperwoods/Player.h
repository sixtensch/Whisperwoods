#pragma once
#include "GameObject.h"
#include "ModelResource.h"
#include "Animator.h"
#include "MeshRenderable.h"
#include "AnimationResource.h"

class Player : public GameObject
{
	std::string m_modelResource;
	Mat4 m_modelOffset;
	Vec3 m_velocity;
	Vec3 m_targetVelocity;
	float m_walkSpeed;
	float m_runSpeed;
	float m_animationSpeed;
	bool m_isCrouch;

	Vec3 Lerp( Vec3 a, Vec3 b, float t )
	{
		return a * (1.0f - t) + b * t;
	}
public:

	bool cameraIsLocked;

	float cameraFollowDistance;
	float cameraFollowTilt;
	Vec3 cameraLookTargetOffset;

	//float walkSpeed = 2;
	//float runSpeed = 5;
	
	shared_ptr<MeshRenderableRigged> characterModel;
	shared_ptr<AnimationResource> animationSet;
	shared_ptr<Animator> characterAnimator;
	
	Vec3 cameraFollowTarget;
	Vec3 cameraLookAtTarget;
	Quaternion cameraLookRotationTarget;

	Player(std::string modelResource, std::string animationsPath, Mat4 modelOffset);

	void PlayerMovement(float delta_time, float movementMultiplier);

	void Update(float delta_time) override;
};