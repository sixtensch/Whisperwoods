#pragma once

struct Bone
{
	std::string name;
	std::string parentName;
	int index;
	int parentIndex;
	Vec3 localPos;
	Quaternion localRot;
	Vec3 localScale;
	//DirectX::XMFLOAT4X4 bindMatrix; // Inverse Bind matrix
	Mat4 inverseBindMatrix;	// local Transform
	//aiMatrix4x4 localMatrix;	// local Transform
	//aiMatrix4x4 modelMatrix;	// model Transform
	Mat4 posedMatrix;
	Bone()
	{
		index = -1;
		parentIndex = -1;
	}
	// TODO: create
};