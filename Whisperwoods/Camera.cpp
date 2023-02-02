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
	Mat4 viewMat = cs::Mat::translation3(-m_transform.position.x, -m_transform.position.y, -m_transform.position.z);
	viewMat = viewMat * m_transform.rotation.Matrix(); //TODO: is this correct?

	// write view to vertexBuffer
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


