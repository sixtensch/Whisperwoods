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

void Camera::CalculatePerspectiveProjection()
{
	m_projection = Mat4(dx::XMMatrixPerspectiveFovLH(this->m_fov, this->m_ratio, this->m_near, this->m_far));
}

void Camera::CalculateOrthoProjection(float width, float height)
{
	m_projection =  Mat4(dx::XMMatrixOrthographicLH(width, height, this->m_near, this->m_far));
}

const Mat4 Camera::GetProjectionMatrix() const
{
	return m_projection;
}
