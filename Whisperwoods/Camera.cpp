#include "Core.h"
#include "Camera.h"

Camera::Camera(Vec3 position, Quaternion rotation)
{
	this->m_transform.position = position;
	this->m_transform.rotation = rotation;
	this->m_far = 0;
	this->m_near = 0;
	this->m_fov = 0;
	this->m_ratio = 0;
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

void Camera::SetPosition(Vec3 position)
{
	this->m_transform.position = position;
}

void Camera::SetRotation(Quaternion rotation)
{
	this->m_transform.rotation = rotation;
}

const Mat4 Camera::GetProjectionMatrix() const
{
	return m_projection;
}
