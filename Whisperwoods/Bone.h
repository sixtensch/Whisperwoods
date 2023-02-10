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
	DirectX::XMFLOAT4X4 inverseBindMatrix;
	DirectX::XMFLOAT4X4 posedMatrix;
	Bone() = default;
	~Bone() = default;
};

struct BoneSerialized
{
	char name[128];
	char parentName[128];
	int index;
	int parentIndex;
	Vec3 localPos;
	Quaternion localRot;
	Vec3 localScale;
	DirectX::XMFLOAT4X4 inverseBindMatrix;	
	DirectX::XMFLOAT4X4 posedMatrix;
	BoneSerialized() : name{ '\0' }, parentName{ '\0' } {}
	BoneSerialized(const Bone& source)
	{
		for (int i = 0; i < 128; i++)
		{
			name[i] = '\0';
			parentName[i] = '\0';
		}
		int nameLen = (source.name.length() > 128) ? 128 : source.name.length();
		int parentNameLen = (source.parentName.length() > 128) ? 128 : source.parentName.length();
		for (int i = 0; i < nameLen; i++)
		{
			name[i] = source.name[i];
		}
		for (int i = 0; i < parentNameLen; i++)
		{
			parentName[i] = source.parentName[i];
		}
		index = source.index;
		parentIndex = source.parentIndex;
		localPos = source.localPos;
		localRot = source.localRot;
		localScale = source.localScale;
		inverseBindMatrix = source.inverseBindMatrix;
		posedMatrix = source.posedMatrix;
	}
	Bone Deserialize()
	{
		Bone bone;
		bone.name = std::string(name);
		bone.parentName = std::string(parentName);
		bone.index = index;
		bone.parentIndex = parentIndex;
		bone.localPos = localPos;
		bone.localScale = localScale;
		bone.inverseBindMatrix = inverseBindMatrix;
		bone.posedMatrix = posedMatrix;
		return bone;
	}
	~BoneSerialized() = default;
	//DirectX::XMFLOAT4X4 bindMatrix; // Inverse Draw matrix
	//aiMatrix4x4 localMatrix;	// local Transform
	//aiMatrix4x4 modelMatrix;	// model Transform
};


