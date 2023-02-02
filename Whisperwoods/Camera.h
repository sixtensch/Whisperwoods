#pragma once
#include "Transform.h"



//still need things with transform rather than a vec3 for position/rotation

class Camera
{
public:
	Camera(Vec3 position, Quaternion rotation);
	~Camera();

	void Update();
	void SetValues(float fovRadians, float viewRatio, float nearDistance, float farDistance);
	void CalculatePerspectiveProjection();
	void CalculateOrthoProjection(float width, float height);
	void SetPosition(Vec3 position);
	void SetRotation(Quaternion rotation);


	const Mat4 GetProjectionMatrix() const;

private:
	float m_fov, m_ratio, m_near, m_far;
	float m_cameraSpeed = 10; //how fast the camera is moving, change value after need! 
	Mat4 m_viewMatrix; //holds the view matrix for current frame
	Mat4 m_projection; // matrix

	Transform m_transform;

};