#include "Core.h"
#include "Player.h"
#include "FBXImporter.h"
#include "Renderer.h"
#include "Input.h"
#include "Resources.h"
#include "LevelImporter.h"
#include "SoundResource.h"


void Player::CalculateCompassMatrix()
{
	compassMatrix = transform.CalculateMatrix( transform.position, cameraCompassRotation, transform.scale );
}

Player::Player(std::string modelResource, std::string animationsPath, Mat4 modelOffset)
{
	// Initialize the model
	m_modelResource = modelResource;
	characterModel = Renderer::CreateMeshRigged( m_modelResource );
	//FBXImporter importer;

	m_modelOffset = modelOffset;
	// Import the animations
	Resources& resources = Resources::Get(); // BIG NONO: Resources resources = Resources::Get();
	
	animationSet = resources.GetAnimation(animationsPath);
	//importer.ImportFBXAnimations(animationsPath, animationSet.get());
	// Init the animator
	//ModelRiggedResource* modelResource = (ModelRiggedResource*)resources.GetResource( ResourceTypeModelRigged, m_modelResource );
	characterAnimator = make_shared<Animator>(resources.GetModelRigged(m_modelResource), characterModel );

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
	characterModel->Materials().AddMaterial(resources.GetMaterial("ShadiiCombined.wwmt"));
	//characterModel->Materials().AddMaterial(resources.GetMaterial("ShadiiWhite.wwmt"));
	//characterModel->Materials().AddMaterial(resources.GetMaterial("ShadiiPupil.wwmt"));
	//characterModel->Materials().AddMaterial(resources.GetMaterial("ShadiiPants.wwmt"));
	//characterModel->Materials().AddMaterial(resources.GetMaterial("ShadiiSpikes.wwmt"));

	m_stamina = 10.0f;
	m_maxStamina = 10.0f;
	m_walkSpeed = 2.0f;
	m_runSpeed = 3.5f;
	cameraFollowDistance = 3.2f;
	//cameraFollowHeight = 2.0f;
	cameraLookTargetOffset = Vec3( 0, -0.5, 0 );
	cameraFollowTilt = 0.95f;
	cameraIsLocked = true;
	playerInFuture = false;
	hasPickedUpEssenceBloom = false;
	m_ranOutOfSprint = false;
	/*FMOD::Sound* soundPtr = (Resources::Get().GetSound("Duck.mp3"))->currentSound;
	m_audioSource = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 0.2f, 1.1f, 0.0f, 10.0f, soundPtr);
	m_audioSource->Play();*/
	FMOD::Sound* undergrowthSoundPtr = (Resources::Get().GetSound("foliage.mp3"))->currentSound;
	m_vegetationSound = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 0.5f, 1.1f, 0.0f, 10.0f, undergrowthSoundPtr);
	this->AddChild((GameObject *) m_vegetationSound.get());
	
	FMOD::Sound* stepsSoundPtr = ((SoundResource*)Resources::Get().GetResource(ResourceTypeSound, "footstep.mp3"))->currentSound;
	m_stepsSound = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 2.0f, 1.1f, 0.0f, 10.0f, stepsSoundPtr);
	this->AddChild((GameObject*)m_stepsSound.get());
}

void Player::ReloadPlayer()
{
	Resources& resources = Resources::Get();
	characterModel = Renderer::CreateMeshRigged( m_modelResource );
	characterAnimator->instanceReference = characterModel;
	characterModel->Materials().AddMaterial(resources.GetMaterial("ShadiiCombined.wwmt"));
}

void Player::UpdateStamina(float maxStamina)
{
	m_maxStamina = maxStamina;
}

void Player::ResetStaminaToMax(float staminaMax)
{
	m_maxStamina = staminaMax;
	m_stamina = staminaMax;
}

float Player::GetCurrentStamina()
{
	return m_stamina;
}

bool Player::IsCrouching()
{
	return m_isCrouch;
}

bool Player::IsRunning()
{
	return (m_velocity.Length() > m_walkSpeed);
}

Quaternion QuaternionLookRotation2( Vec3 forward, Vec3 up )
{
	forward.Normalize();

	Vec3 vector = forward.Normalize();
	Vec3 vector2 = up.Cross( vector ).Normalize();
	Vec3 vector3 = vector.Cross( vector2 );
	float m00 = vector2.x;
	float m01 = vector2.y;
	float m02 = vector2.z;
	float m10 = vector3.x;
	float m11 = vector3.y;
	float m12 = vector3.z;
	float m20 = vector.x;
	float m21 = vector.y;
	float m22 = vector.z;


	float num8 = (m00 + m11) + m22;
	Quaternion quaternion;
	if (num8 > 0.0f)
	{
		float num = (float)std::sqrt( num8 + 1.0f );
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m12 - m21) * num;
		quaternion.y = (m20 - m02) * num;
		quaternion.z = (m01 - m10) * num;
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)std::sqrt( ((1.0f + m00) - m11) - m22 );
		float num4 = 0.5f / num7;
		quaternion.x = 0.5f * num7;
		quaternion.y = (m01 + m10) * num4;
		quaternion.z = (m02 + m20) * num4;
		quaternion.w = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		float num6 = (float)std::sqrt( ((1.0f + m11) - m00) - m22 );
		float num3 = 0.5f / num6;
		quaternion.x = (m10 + m01) * num3;
		quaternion.y = 0.5f * num6;
		quaternion.z = (m21 + m12) * num3;
		quaternion.w = (m20 - m02) * num3;
		return quaternion;
	}
	float num5 = (float)std::sqrt( ((1.0f + m22) - m00) - m11 );
	float num2 = 0.5f / num5;
	quaternion.x = (m20 + m02) * num2;
	quaternion.y = (m21 + m12) * num2;
	quaternion.z = 0.5f * num5;
	quaternion.w = (m01 - m10) * num2;
	return quaternion;
}

Quaternion SlerpDX( Quaternion q0, Quaternion q1, float t )
{
	DirectX::XMVECTOR Q0 = DirectX::XMVectorSet( (float)q0.x, (float)q0.y, (float)q0.z, (float)q0.w );
	DirectX::XMVECTOR Q1 = DirectX::XMVectorSet( (float)q1.x, (float)q1.y, (float)q1.z, (float)q1.w );
	DirectX::XMVECTOR OUTPUT = DirectX::XMQuaternionSlerp( Q0, Q1, t );
	DirectX::XMFLOAT4 FL4;
	DirectX::XMStoreFloat4( &FL4, OUTPUT );
	return Quaternion( FL4.x, FL4.y, FL4.z, FL4.w );
}

void Player::PlayerMovement(float delta_time, float movementMultiplier)
{
	if (cameraIsLocked)
	{
		Vec3 inputVector;
		Quaternion wRot = cameraCompassRotation;
		Vec3 forward = wRot * Vec3( 0, 0, 1 );
		Vec3 right = wRot * Vec3( 1, 0, 0 );
		forward.y = right.y = 0;
		forward.Normalize();
		right.Normalize();
		if (Input::Get().IsKeybindDown( KeybindForward ))	inputVector += forward;
		if (Input::Get().IsKeybindDown( KeybindBackward ))	inputVector -= forward;
		if (Input::Get().IsKeybindDown( KeybindRight ))		inputVector += right;
		if (Input::Get().IsKeybindDown( KeybindLeft ))		inputVector -= right;
		float walkRunMultiplier = ((Input::Get().IsKeybindDown(KeybindSprint) && !Input::Get().IsKeybindDown(KeybindCrouch) && !playerInFuture) ? m_runSpeed : m_walkSpeed);
		m_targetVelocity = Vec3( inputVector.x, inputVector.y, inputVector.z );
		if (m_targetVelocity.Length() > 0)
			m_targetVelocity.Normalize();
		m_targetVelocity = m_targetVelocity * walkRunMultiplier;

		//float walkRunMultiplier = ((Input::Get().IsKeybindDown(KeybindSprint) && !Input::Get().IsKeybindDown(KeybindCrouch) && m_stamina > 0.0f && !m_ranOutOfSprint && !playerInFuture) ? m_runSpeed : m_walkSpeed);
		//
		//m_targetVelocity = Vec3( inputVector.x * walkRunMultiplier, inputVector.y * walkRunMultiplier, inputVector.z * walkRunMultiplier );

		if (m_targetVelocity.Length() > m_runSpeed)
		{
			m_targetVelocity = m_targetVelocity.Normalize();
			m_targetVelocity *= m_runSpeed;
		}

		if (m_velocity.Length() > m_walkSpeed)
		{
			m_stamina = m_stamina - ((cs::fclamp(m_targetVelocity.Length() - m_walkSpeed, 0.0f, 2.0f) * delta_time) * RUNNING_STAMINA_DECAY);
		}

		if (!Input::Get().IsKeybindDown(KeybindSprint)) // not sprinting
		{
			if (m_targetVelocity.Length() > m_walkSpeed)
			{
				m_targetVelocity = m_targetVelocity.Normalize();
				m_targetVelocity *= m_walkSpeed;
			}
		}

		if (m_ranOutOfSprint == true)
		{
			if (Input::Get().IsKeybindDown(KeybindSprint) == false || (
				Input::Get().IsKeybindDown(KeybindForward) == false &&
				Input::Get().IsKeybindDown(KeybindBackward) == false &&
				Input::Get().IsKeybindDown(KeybindLeft) == false &&
				Input::Get().IsKeybindDown(KeybindRight) == false 
				))
			{
				m_ranOutOfSprint = false;
			}
		}
		
		

		if (m_stamina <= 0.0f && m_targetVelocity.Length() > 0.0f)
		{
			m_targetVelocity = m_targetVelocity.Normalize();
			m_targetVelocity *= m_walkSpeed;
		}

		// Set the rotation based on the movement
		if (m_targetVelocity.Length() > 0.01f)
		{
			Quaternion dirQuaternion = QuaternionLookRotation2( m_targetVelocity.Normalized(), Vec3(0,1,0) );
			transform.rotation = SlerpDX( transform.rotation, dirQuaternion, cs::fclamp(delta_time*10.0f, 0.0001f, 1.0f));
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

		Vec3 targetWithCollision = m_targetVelocity - converted;

		if (!std::isnan( targetWithCollision.x ) && !std::isnan( targetWithCollision.y ) && !std::isnan( targetWithCollision.z ))
		{
			m_velocity = Lerp( m_velocity, m_targetVelocity-converted, delta_time * movementMultiplier );
		}
		else
		{
			LOG_WARN( "Velocity target was NAN: m_targetVelocity: %f %f %f - converted: %f %f %f", 
				m_targetVelocity.x, m_targetVelocity.y, m_targetVelocity.z,
				converted.x, converted.y, converted.z	
			);
		}

		if (transform.parent != nullptr)
		{
			transform.position += transform.parent->GetWorldRotation() *  m_velocity * delta_time;
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
			//cs::Vec3 delta = Vec3( 0.0f, (float)mouseState.x * delta_time, 0.0f );
			//LOG_TRACE("Mouse state X: %d", mouseState.x);
			cameraCompassRotation = cameraCompassRotation * (Quaternion::GetAxis( {0,1,0}, (float)mouseState.x * delta_time ));
			//transform.rotation = transform.rotation * (Quaternion::GetAxis( { 0,1,0 }, (float)mouseState.x * delta_time ));
			//transform.rotation = transform.rotation * (Quaternion::GetEuler( delta ));
		}

		// Camera follow point calculation.
		Vec3 followPoint = -(cameraCompassRotation * (Quaternion::GetAxis( Vec3( 1, 0, 0 ), cameraFollowTilt ) * Vec3( 0, 0, 1 )) * cameraFollowDistance);
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


	//regain stamina
	if ((!Input::Get().IsKeybindDown(KeybindSprint) || m_ranOutOfSprint == true || m_velocity.Length() <= 0.2f) || playerInFuture || IsCrouching())
	{
		m_stamina = cs::fclamp(m_stamina + (2.5f * delta_time), 0.0f, m_maxStamina); //or 0.0f rather than 0
	}


	characterAnimator->playbackSpeed = m_animationSpeed;
	characterAnimator->Update( delta_time );
	transform.CalculateWorldMatrix();
	CalculateCompassMatrix();
	characterModel->worldMatrix = transform.worldMatrix * m_modelOffset;

	UpdateSound(delta_time);
}

// Only the essentials.
void Player::CinematicUpdate( float delta_time )
{
	characterAnimator->playbackSpeed = m_animationSpeed;
	characterAnimator->Update( delta_time );
	transform.CalculateWorldMatrix();
	characterModel->worldMatrix = transform.worldMatrix * m_modelOffset;

}

void Player::UpdateSound(float delta_time)
{
	m_vegetationSound->Update(delta_time);
	m_stepsSound->Update(delta_time);

	// Sound management!

	Point2 mapPoint = currentRoom->worldToBitmapPoint(transform.GetWorldPosition());
	LevelPixel bitMapPixel = currentRoom->m_levelResource->bitmap[mapPoint.x + mapPoint.y * currentRoom->m_levelResource->pixelWidth];
	float realNotWhackDensityWhichActuallyIsAccurate = 1.0f - bitMapPixel.density;
	if (realNotWhackDensityWhichActuallyIsAccurate > 0.2f && m_velocity.Length() > 0.05f && realNotWhackDensityWhichActuallyIsAccurate < 1.0f) // density
	{
		//change volume
		float volPercent = m_velocity.Length() / m_runSpeed;
		volPercent = pow(volPercent, 2.0f);
		if (volPercent < 0.01f)
		{
			volPercent = 0.1f;
		}
		if (volPercent > 0.8f)
		{
			volPercent = 0.8f;
		}
		realNotWhackDensityWhichActuallyIsAccurate = pow(realNotWhackDensityWhichActuallyIsAccurate, 0.6);
		m_vegetationSound->volume = volPercent * 0.22f * realNotWhackDensityWhichActuallyIsAccurate;

		if (!m_vegetationSound->IsPlaying()) //repeat sound? (looping kind of)
		{
			m_vegetationSound->Play();
		}
	}
	else if (m_vegetationSound->IsPlaying()) // execute order 66
	{
		m_vegetationSound->Stop();
	}

	if (m_velocity.Length() > 0.05f)
	{
		//change volume
		float volPercent = m_velocity.Length() / m_runSpeed;
		volPercent = pow(volPercent, 0.7f);
		//volPercent *= volPercent;
		m_stepsSound->volume = 0.25f * volPercent;
		//trigger when character sets foot on ground
		if (((characterAnimator->globalTime > 0.2f && characterAnimator->globalTime < 0.3f) || (characterAnimator->globalTime > 0.66f && characterAnimator->globalTime < 0.76f)) /*&& !m_stepsSound->IsPlaying()*/)
		{
			m_stepsSound->Stop();
			m_stepsSound->Play();
		}
	}
	//else if(m_stepsSound->IsPlaying())  // execute order 66
	//{
	//	m_stepsSound->Stop();
	//}
}
