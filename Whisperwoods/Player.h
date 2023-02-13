#pragma once
#include "GameObject.h"
#include "ModelResource.h"
#include "Animator.h"
#include "MeshRenderable.h"
#include "AnimationResource.h"

class Player : public GameObject
{
	Mat4 m_modelOffset;
public:
	
	shared_ptr<MeshRenderableRigged> characterModel;
	shared_ptr<AnimationResource> animationSet;
	unique_ptr<Animator> characterAnimator;
	
	Vec3 cameraFollowTarget;
	Vec3 cameraLookAtTarget;
	Quaternion cameraLookRotationTarget;

	Player(std::string modelResource, std::string animationsPath, Mat4 modelOffset);

	void PlayerMovement(float delta_time, float movementMultiplier);

	void Update(float delta_time) override;
};