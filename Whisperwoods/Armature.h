#pragma once
#include "Bone.h"
#include "Vertex.h"
#include "VertexGroup.h"

#define DXMAT DirectX::XMMATRIX
#define DX4X4 DirectX::XMFLOAT4X4
#define DX DirextX
#define MAT XMMATRIX
#define XMIDENT DirectX::XMMatrixIdentity()

// Stuff required later for skeletal anim.
struct BoneWeightPair
{
	int bone;
	float weight;
	bool operator < (const BoneWeightPair& rhs)
	{
		return (weight < rhs.weight);
	}
	bool operator > ( const BoneWeightPair& rhs )
	{
		return (weight > rhs.weight);
	}
};

struct Armature
{
	DirectX::XMFLOAT4X4 globalInverseTransform;
	cs::List<Bone> bones;
	cs::List<DirectX::XMFLOAT4X4> boneMatricies; // For updating the buffer
	int numBones;

	ComPtr<ID3D11Buffer> matrixBuffer;
	ComPtr<ID3D11ShaderResourceView> matrixSRV;

	Armature();
	~Armature();

	void ResolveBoneParentIndicies();
	
	//void PrintMatrix(DirectX::XMFLOAT4X4 m, std::string name)
	//{
	//	std::cout << "Matrix for: " << name << std::endl;
	//	printf("%.2f, %.2f, %.2f, %.2f\n", m._11, m._12, m._13, m._14);
	//	printf("%.2f, %.2f, %.2f, %.2f\n", m._21, m._22, m._23, m._24);
	//	printf("%.2f, %.2f, %.2f, %.2f\n", m._31, m._32, m._33, m._34);
	//	printf("%.2f, %.2f, %.2f, %.2f\n", m._41, m._42, m._43, m._44);
	//}

	//void PrintAllFinalMatricies()
	//{
	//	std::cout << "Final Matricies" << std::endl;
	//	for (size_t i = 0; i < numBones; i++)
	//	{
	//		PrintMatrix(boneMatricies[i], "Bone: ");
	//	}
	//}

	//void PrintAllBones();
	//{
	//	if (bones.size() == 0) return;
	//	std::cout << "Armature:" << std::endl;
	//	for (size_t i = 0; i < bones.size(); i++)
	//	{
	//		std::cout
	//			<< bones[i].index << " - Bone: " << bones[i].name
	//			//<< " Pos: " << bones[i].transform.position.ToString() 
	//			//<< " Rot: " << bones[i].transform.rotation.ToString() 
	//			<< " Parent: " << bones[i].parentName
	//			<< " ParentI: " << bones[i].parentIndex << std::endl;
	//	}
	//}

};