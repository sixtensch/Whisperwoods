#include "Core.h"
#include "Player.h"
#include "FBXImporter.h"
#include "Renderer.h"
#include "Input.h"

Player::Player(std::string modelResource, std::string animationsPath, Mat4 modelOffset)
{
	// Initialize the model
	characterModel = Renderer::CreateMeshRigged(modelResource);
	FBXImporter importer;

	m_modelOffset = modelOffset;
	// Import the animations
	animationSet = std::make_shared<AnimationResource>();
	importer.ImportFBXAnimations(animationsPath, animationSet.get());

}


void Player::PlayerMovement(float delta_time, float movementMultiplier)
{
	Vec3 inputVector;
	Quaternion wRot = transform.GetWorldRotation();
	Vec3 forward = wRot * Vec3(0, 0, 1);
	Vec3 right = wRot * Vec3(1, 0, 0);
	forward.y = right.y = 0;
	forward.Normalize();
	right.Normalize();
	if (Input::Get().IsKeybindDown(KeybindForward))		inputVector += forward;
	if (Input::Get().IsKeybindDown(KeybindBackward))	inputVector -= forward;
	if (Input::Get().IsKeybindDown(KeybindRight))		inputVector += right;
	if (Input::Get().IsKeybindDown(KeybindLeft))		inputVector -= right;

	MouseState mouseState = Input::Get().GetMouseState();

	static Vec3 rotationVec = {};
	if (mouseState.positionMode == dx::Mouse::MODE_RELATIVE)
	{
		cs::Vec3 delta = Vec3(0.0f, mouseState.x, 0.0f);
		//LOG_TRACE("Mouse state X: %d", mouseState.x);
		transform.rotation = transform.rotation * Quaternion::GetEuler(delta * delta_time * 4);
	}

	transform.position += inputVector * delta_time * movementMultiplier;

	Vec3 followPoint = -(forward * 3);
	followPoint += Vec3(0, 2, 0);
	cameraFollowTarget = followPoint;
	Vec3 currentPos = transform.GetWorldPosition();
	cameraFollowTarget = currentPos + cameraFollowTarget;

	Vec3 lookDir = currentPos - cameraFollowTarget;
	lookDir.Normalize();

	cameraLookRotationTarget = Quaternion::GetDirection(lookDir);
}

void Player::Update(float delta_time)
{
	// Handle the input and movement (beta) TODO: collision shit.
	PlayerMovement(delta_time, 5);
	transform.CalculateWorldMatrix();
	characterModel->worldMatrix = transform.worldMatrix * m_modelOffset;
}