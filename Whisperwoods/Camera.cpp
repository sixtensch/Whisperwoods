#include "Core.h"
#include "Camera.h"

Camera::Camera()
{
	this->m_transform.position = Vec3(0,0,0);
	this->m_transform.rotation = Quaternion(0,0,0,0);
	this->m_far = 0;
	this->m_near = 0;
	this->m_fov = 0;
	this->m_ratio = 0;
}

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
	viewMat = m_transform.rotation.Conjugate().Matrix() * viewMat; //TODO: is this correct? (Probably now)

	m_viewMatrix = viewMat;
	m_transform.CalculateWorldMatrix();
	m_worldMatrix = m_transform.worldMatrix;
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

void Camera::CopyTransform(const Transform& transform)
{
	m_transform = transform;
}

const Vec3 Camera::GetDirection() const
{
	// TODO: This should be a "globally" defined vector that transform makes use of.
	const Vec4 forward = Vec4(0.0f, 0.0f, 1.0f, 0.0f);

	return (Mat4)m_transform.rotation.Matrix() * forward;
}

const Vec3 Camera::GetUp()
{
	// TODO: This should be a "globally" defined vector that transform makes use of.
	const Vec4 up = Vec4(0.0f, 1.0f, 0.0f, 0.0f);

	return (Mat4)m_transform.rotation.Matrix() * up;
}

const Vec3 Camera::GetRight()
{
	// TODO: This should be a "globally" defined vector that transform makes use of.
	const Vec4 right = Vec4(1.0f, 0.0f, 0.0f, 0.0f);

	return (Mat4)m_transform.rotation.Matrix() * right;
}


const Quaternion Camera::GetRotation() const
{
	return m_transform.rotation;
}

const Vec3 Camera::GetPosition() const
{
	return m_transform.position;
}

const Mat4 Camera::GetWorldMatrix() const
{
	return m_worldMatrix;
}

const Mat4 Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

const Mat4 Camera::GetProjectionMatrix() const
{
	return m_projection;
}

float Camera::GetFov() const
{
	return m_fov;
}

void Camera::SetFov(float fov)
{
	m_fov = fov;
}
