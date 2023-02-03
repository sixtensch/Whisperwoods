#pragma once


struct Transform
{
public:
	// Data
	Vec3 position;
	Quaternion rotation;
	Vec3 scale;

	// Matricies
	Mat4 localMatrix;
	Mat4 worldMatrix;

	// World Parameters
	Vec3 worldPosition;
	Quaternion worldRotation;
	Vec3 worldScale;

	// Parent / Child stuff.
	Transform* parent;
	cs::List<Transform*> transforms; // children, TODO: Maybe remove this as it might be redundant.

private:

	void DecomposeWorldMatrixIntoWorldParameters();

public:
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
	Mat4 CalculateMatrix(Vec3 p_pos, Quaternion p_rotation, Vec3 p_scale);
	void CalculateLocalMatrix();
	void CalculateLocalMatrix(Vec3 p_pos, Quaternion p_rotation, Vec3 p_scale);
	void CalculateWorldMatrix();
	void CalculateWorldMatrix(Mat4 parentWorldMatrix);

	// Nice to haves
	void SetRotationEuler(Vec3 p_rotation);
	//Vec3 GetRotationEuler();
	//void Rotate(Quaternion p_rotation);
	Vec3 GetWorldPosition();
	Quaternion GetWorldRotation();
};