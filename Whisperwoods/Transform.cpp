#include "Core.h"
#include "Transform.h"

Transform::Transform()
{
	parent = nullptr;
}

Transform::Transform(Vec3 p_position, Quaternion p_rotation, Vec3 p_scale) : position(p_position), rotation(p_rotation), scale(p_scale) 
{
	parent = nullptr;
}

Transform::Transform(Vec3 p_position, Vec3 p_rotation_euler, Vec3 p_scale) : position(p_position), scale(p_scale)
{
	parent = nullptr;
	SetRotationEuler(p_rotation_euler);
}

Mat4 Transform::CalculateMatrix(Vec3 p_pos, Quaternion p_rotation, Vec3 p_scale)
{
	Mat4 rotationMatrix = p_rotation.Matrix();
	Mat4 scalingMatrix = Mat::scale3(p_scale);
	Mat4 translationMatrix = Mat::translation3(p_pos);
	Mat4 identityMatrix = Mat4();
	return identityMatrix * scalingMatrix * rotationMatrix * translationMatrix;
}


void Transform::CalculateLocalMatrix()
{
	localMatrix = CalculateMatrix(position, rotation, scale);
}

void Transform::CalculateLocalMatrix(Vec3 p_pos, Quaternion p_rotation, Vec3 p_scale)
{
	position = p_pos;
	rotation = p_rotation;
	scale = p_scale;
	localMatrix = CalculateMatrix(position, rotation, scale);
}

void Transform::CalculateWorldMatrix()
{
	CalculateLocalMatrix();
	worldMatrix = (parent == nullptr) ? localMatrix : localMatrix * parent->worldMatrix;
}


// Overload for external interaction
void Transform::CalculateWorldMatrix(Mat4 parentWorldMatrix)
{
	CalculateLocalMatrix();
	worldMatrix = localMatrix * parentWorldMatrix;
}

void Transform::SetRotationEuler(Vec3 p_rotation)
{
	DirectX::XMFLOAT4 dxquat;
	DirectX::XMStoreFloat4(&dxquat, DirectX::XMQuaternionRotationRollPitchYaw(p_rotation.z, p_rotation.x, p_rotation.y));
	//rotation = Quaternion(dxquat.x, dxquat.y, dxquat.z, dxquat.w); // TODO: Waiting for fix
}


Vec3 Transform::GetWorldPosition()
{
	CalculateWorldMatrix();
	return Vec3(worldMatrix(0, 3), worldMatrix(1, 3), worldMatrix(2, 3));
}

Quaternion Transform::GetWorldRotation()
{
	// TODO: FIX THIS
	//DirectX::XMVECTOR dxvec = DirectX::XMQuaternionRotationMatrix()
	//Quaternion rot = Quaternion()
	return Quaternion();
}
