#include "Core.h"
#include "Transform.h"

void Transform::DecomposeWorldMatrixIntoWorldParameters()
{
	CalculateWorldMatrix(); // TODO: Possibly remove this as it might be redundant after a proper implementation of the frame sequence. 

	// TODO: Change this dirty, naughty, filthyness to use the CHSL better.
	// Decompose matrix
	DirectX::XMVECTOR outScale;
	DirectX::XMVECTOR outRotation;
	DirectX::XMVECTOR outTranslation;
	DirectX::XMMATRIX inMatrix = ((Mat4)worldMatrix.Transpose()).XMMatrix();
	DirectX::XMMatrixDecompose(&outScale, &outRotation, &outTranslation, inMatrix);

	// Convert into readable data
	DirectX::XMFLOAT3 f3Scale;
	DirectX::XMFLOAT4 f4Rotation;
	DirectX::XMFLOAT3 f3Translation; 
	DirectX::XMStoreFloat3(&f3Scale, outScale);
	DirectX::XMStoreFloat4(&f4Rotation, outRotation);
	DirectX::XMStoreFloat3(&f3Translation, outTranslation);

	// Store into parameters.
	worldPosition = Vec3(f3Translation.x, f3Translation.y, f3Translation.z);
	worldRotation = Quaternion(f4Rotation.x, f4Rotation.y, f4Rotation.z, f4Rotation.w);
	worldScale = Vec3(f3Scale.x, f3Scale.y, f3Scale.z);
}

Transform::Transform()
{
	rotation = Quaternion::GetIdentity();
	scale = Vec3(1, 1, 1); // scale 0 as basis isn't great.
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
	rotation = Quaternion::GetEuler(p_rotation.x, p_rotation.y, p_rotation.z);
}

Vec3 Transform::GetWorldPosition()
{
	DecomposeWorldMatrixIntoWorldParameters(); // TODO: Possibly do this in a more systematic manner instead of on each Get.
	return worldPosition;
}

Quaternion Transform::GetWorldRotation()
{
	DecomposeWorldMatrixIntoWorldParameters(); // TODO: Possibly do this in a more systematic manner instead of on each Get.
	return worldRotation;
}
