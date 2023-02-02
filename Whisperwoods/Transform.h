#pragma once


struct Transform
{
	// Data
	Transform* parent;
	Vec3 position;
	Quaternion rotation;
	Vec3 scale;
	Mat4 worldMatrix;

	// Default constructor
	Transform();
	Transform(Vec3 p_position, Quaternion p_rotation, Vec3 p_scale);
	Transform(Vec3 p_position, Vec3 p_rotation_euler, Vec3 p_scale);

	// Copy and assignment constructors
	Transform(const Transform& other) = default; 
	Transform& operator=(const Transform& other) = default;
	Transform(Transform&& other) = default;
	Transform& operator=(Transform&& other) = default;

	// Core functionality
	void CalculateWorldMatrix();

	// Nice to haves
	void SetRotationEuler(Vec3 p_rotation);
	//Vec3 GetRotationEuler();
	//void Rotate(Quaternion p_rotation);
	Vec3 GetWorldPosition();
	Quaternion GetWorldRotation();
};