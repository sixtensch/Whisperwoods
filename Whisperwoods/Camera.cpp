#include "Core.h"
#include "Camera.h"

Camera::Camera()
{
	//add position/rotation to transform here
}

Camera::~Camera()
{
}

void Camera::Update()
{
	float cameraPositionX = 0.0f, cameraPositionY = 0.0f, cameraPositionZ = 0.0f; //These are scrap values, replace them with the transform later
	m_viewMatrix = Mat4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	-cameraPositionX, -cameraPositionY, -cameraPositionZ, 1.0f);
}

void Camera::SetValues(float fovRadians, float viewRatio, float nearDistance, float farDistance)
{
	this->m_far = farDistance;
	this->m_near = nearDistance;
	this->m_fov = fovRadians;
	this->m_ratio = viewRatio;
}
