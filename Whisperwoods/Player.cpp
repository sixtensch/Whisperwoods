#include "Core.h"
#include "Player.h"
#include "FBXImporter.h"
#include "Renderer.h"
#include "Input.h"
#include "Resources.h"
#include "LevelImporter.h"

Player::Player(std::string modelResource, std::string animationsPath, Mat4 modelOffset)
{
	// Initialize the model
	m_modelResource = modelResource;
	characterModel = Renderer::CreateMeshRigged( m_modelResource );
	//FBXImporter importer;

	m_modelOffset = modelOffset;
	// Import the animations
	Resources& resources = Resources::Get(); // BIG NONO: Resources resources = Resources::Get();
	
	animationSet = (AnimationResource*)resources.GetResource( ResourceTypeAnimations, animationsPath );
	//importer.ImportFBXAnimations(animationsPath, animationSet.get());
	// Init the animator
	//ModelRiggedResource* modelResource = (ModelRiggedResource*)resources.GetResource( ResourceTypeModelRigged, m_modelResource );
	characterAnimator = make_shared<Animator>( (ModelRiggedResource*)resources.GetResource( ResourceTypeModelRigged, m_modelResource ), characterModel );

	// Hardcoded animation retrieving because loops are annoying.
	// Idle
	Animation* idleAnimation = &animationSet->animations[0];
	// Tail
	Animation* tailAnimation = &animationSet->animations[3];
	// Walk
	Animation* walkAnimation = &animationSet->animations[4];
	// Run
	Animation* runAnimation = &animationSet->animations[2];
	// Crouch
	Animation* couchAnimation = &animationSet->animations[5];

	// Set the initial animation speed.
	m_animationSpeed = 1.0f;

	// Add the animations to the animator.
	characterAnimator->AddAnimation( idleAnimation, 0, m_animationSpeed, 1.0f );
	characterAnimator->AddAnimation( tailAnimation, 0, m_animationSpeed, 0.5f );
	characterAnimator->AddAnimation( walkAnimation, 0, m_animationSpeed, 0.0f );
	characterAnimator->AddAnimation( runAnimation, 0, m_animationSpeed, 0.0f );
	characterAnimator->AddAnimation( couchAnimation, 0, m_animationSpeed, 0.0f );

	// Get materials
	characterModel->Materials().AddMaterial( (const MaterialResource*)resources.GetResource( ResourceTypeMaterial, "ShadiiCombined.wwmt" ) );
	//characterModel->Materials().AddMaterial( (const MaterialResource*)resources.GetResource( ResourceTypeMaterial, "ShadiiWhite.wwmt" ) );
	//characterModel->Materials().AddMaterial( (const MaterialResource*)resources.GetResource( ResourceTypeMaterial, "ShadiiPupil.wwmt" ) );
	//characterModel->Materials().AddMaterial( (const MaterialResource*)resources.GetResource( ResourceTypeMaterial, "ShadiiPants.wwmt" ) );
	//characterModel->Materials().AddMaterial( (const MaterialResource*)resources.GetResource( ResourceTypeMaterial, "ShadiiSpikes.wwmt" ) );

	m_walkSpeed = 1.5f;
	m_runSpeed = 3.5f;
	cameraFollowDistance = 3.0f;
	//cameraFollowHeight = 2.0f;
	cameraFollowTilt = cs::c_pi / 4;
	cameraIsLocked = true;
}

void Player::PlayerMovement(float delta_time, float movementMultiplier)
{
	if (cameraIsLocked)
	{
		Vec3 inputVector;
		Quaternion wRot = transform.GetWorldRotation();
		Vec3 forward = wRot * Vec3( 0, 0, 1 );
		Vec3 right = wRot * Vec3( 1, 0, 0 );
		forward.y = right.y = 0;
		forward.Normalize();
		right.Normalize();
		if (Input::Get().IsKeybindDown( KeybindForward ))	inputVector += forward;
		if (Input::Get().IsKeybindDown( KeybindBackward ))	inputVector -= forward;
		if (Input::Get().IsKeybindDown( KeybindRight ))		inputVector += right;
		if (Input::Get().IsKeybindDown( KeybindLeft ))		inputVector -= right;
		float walkRunMultiplier = ((Input::Get().IsKeybindDown( KeybindSprint )) ? m_runSpeed : m_walkSpeed);
		m_targetVelocity = (inputVector)*walkRunMultiplier;

		if (m_targetVelocity.Length() > m_runSpeed)
		{
			m_targetVelocity.Normalize();
			m_targetVelocity *= m_runSpeed;
		}

		Point2 mapPoint = currentRoom->worldToBitmapPoint(transform.GetWorldPosition());
		if (m_velocity.Length() > m_runSpeed)
		{
			m_velocity = m_velocity.Normalize() * m_runSpeed;
		}
		
		sampleVector = currentRoom->sampleBitMapCollision(transform.GetWorldPosition());
		Vec3 converted(( - (sampleVector.x * sampleVector.x * sampleVector.x))*0.05f, 0,
			(sampleVector.y * sampleVector.y * sampleVector.y )*0.05f);
		if (converted.Length() > m_runSpeed)
		{
			converted.Normalize();
			converted *= m_runSpeed;
		}

		m_velocity = Lerp( m_velocity, m_targetVelocity-converted, delta_time * movementMultiplier );

		if (transform.parent != nullptr)
		{
			transform.position += transform.parent->GetWorldRotation().Conjugate() *  m_velocity * delta_time;
		}
		else
		{
			transform.position += m_velocity * delta_time;
		}

		m_isCrouch = Input::Get().IsKeybindDown( KeybindCrouch );


		// Player rotation
		MouseState mouseState = Input::Get().GetMouseState();
		static Vec3 rotationVec = {};
		if (mouseState.positionMode == dx::Mouse::MODE_RELATIVE)
		{
			cs::Vec3 delta = Vec3( 0.0f, (float)mouseState.x * delta_time, 0.0f );
			//LOG_TRACE("Mouse state X: %d", mouseState.x);
			transform.rotation = transform.rotation * (Quaternion::GetEuler( delta ));
		}

		// Camera follow point calculation.
		Vec3 followPoint = -(wRot * (Quaternion::GetAxis( Vec3( 1, 0, 0 ), cameraFollowTilt ) * Vec3( 0, 0, 1 )) * cameraFollowDistance);
		cameraFollowTarget = followPoint;
		Vec3 currentPos = transform.GetWorldPosition();
		cameraFollowTarget = currentPos + cameraFollowTarget;
		Vec3 lookDir = currentPos - (cameraFollowTarget + cameraLookTargetOffset);
		lookDir.Normalize();
		cameraLookRotationTarget = Quaternion::GetDirection( lookDir );
	}
}

void Player::Update(float delta_time)
{
	// Handle the input and movement (beta) TODO: collision shit.
	PlayerMovement(delta_time, 10);
	characterAnimator->loadedAnimations[2].influence = (m_velocity.Length() / m_walkSpeed);
	characterAnimator->loadedAnimations[3].influence = (m_velocity.Length() / m_runSpeed);
	characterAnimator->loadedAnimations[4].influence = (m_isCrouch);

	Vec3 forward = transform.rotation * Vec3( 0, 0, 1 );
	forward.y = 0;

	float forwardRelative = m_velocity.Dot( forward );
	if (forwardRelative >= 0)
		m_animationSpeed = 1.0f + ((m_velocity.Length() / m_runSpeed)*0.5f);
	else
		m_animationSpeed = -1.0f - ((m_velocity.Length() / m_runSpeed) * 0.5f);
	
	if (m_isCrouch)
	{
		if (forwardRelative > 0.2f)
			m_animationSpeed = 1.0f + ((m_velocity.Length() / m_runSpeed) * 0.5f);
		else if (forwardRelative < -0.2f)
			m_animationSpeed = -1.0f - ((m_velocity.Length() / m_runSpeed) * 0.5f);
		else
			m_animationSpeed = 0;
	}

	characterAnimator->playbackSpeed = m_animationSpeed;

	characterAnimator->Update( delta_time );
	transform.CalculateWorldMatrix();
	characterModel->worldMatrix = transform.worldMatrix * m_modelOffset;
}