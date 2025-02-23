#pragma once
#include "Transform.h"



//still need things with transform rather than a vec3 for position/rotation

class Camera
{
public:
	Camera();
	Camera(Vec3 position, Quaternion rotation);
	~Camera();

	void Update();
	void SetValues(float fovRadians, float viewRatio, float nearDistance, float farDistance);
	void CalculatePerspectiveProjection();
	void CalculateOrthoProjection(float width, float height);
	void SetPosition(Vec3 position);
	void SetRotation(Quaternion rotation);

	void CopyTransform(const Transform& transform);

	// TODO: These functions should be inside transform.
	const Vec3 GetDirection() const;
	const Vec3 GetUp();
	const Vec3 GetRight();
	const Quaternion GetRotation() const;

	const Vec3 GetWPosition();
	const Vec3 GetPosition() const;
	const Mat4 GetWorldMatrix() const;
	const Mat4 GetViewMatrix() const;
	const Mat4 GetProjectionMatrix() const;

	float GetFov() const;
	void SetFov(float fov);

	Transform m_transform;
private:
	float m_fov, m_ratio, m_near, m_far;
	float m_cameraSpeed = 10; //how fast the camera is moving, change value after need! 
	Mat4 m_viewMatrix; //holds the view matrix for current frame
	Mat4 m_projection; // matrix
	Mat4 m_worldMatrix; 


};