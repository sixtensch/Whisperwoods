#pragma once
#include "Bone.h"

// Stuff required later for skeletal anim.
struct BoneWeightPair
{
	int bone;
	float weight;
	bool operator < (const BoneWeightPair& rhs)
	{
		return (weight < rhs.weight);
	}
};


struct Armature
{
	Mat4 globalInverseTransform;
	cs::List<Bone> bones;
	cs::List<Mat4> boneMatricies;
	int numBones;

	//ComPtr<ID3D11Buffer> matrixBuffer;
	//ComPtr<ID3D11ShaderResourceView> matrixSRV;

	Armature();

	~Armature();

	//void InitializeMatrix(int numB);
	//{
	//	numBones = numB;
	//	initialized = true;
	//}

	/*
	void InitializeGPUResources(ComPtr<ID3D11Device>& device)
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * numBones;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(DirectX::XMFLOAT4X4);

		//	D3D11_SUBRESOURCE_DATA data;
		//	data.pSysMem = boneMatricies;
		//	data.SysMemPitch = 0;
		//	data.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &matrixBuffer);
		if (FAILED(hr))
		{
			std::cout << "failed to create bone matrix buffer" << std::endl;
		}


		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		//shaderResourceViewDesc.Buffer.ElementOffset = 0;
		shaderResourceViewDesc.Buffer.FirstElement = 0;
		shaderResourceViewDesc.Buffer.NumElements = numBones;
		//shaderResourceViewDesc.Buffer.ElementWidth = 64;

		//ID3D11Buffer* testBuffer = matrixBuffer.Get();
		hr = device->CreateShaderResourceView(matrixBuffer.Get(), &shaderResourceViewDesc, &matrixSRV);
		if (FAILED(hr))
		{
			std::cout << "Failed to Create resource view" << std::endl;
		}

		//delete[] boneMatricies;
	}
	*/

	/*
	void ResolveBoneParents()
	{
		for (size_t i = 1; i < bones.size(); i++)
		{
			bones[i].transform.parent = &bones[bones[i].parentIndex].transform;
		}
	}

	DirectX::XMMATRIX ToXM(DX4X4 input4x4)
	{
		return DirectX::XMLoadFloat4x4(&input4x4);
	}
	*/
	
	/*
	DirectX::XMFLOAT4X4 ConvertToDirectX(aiMatrix4x4* mat)
	{
		DirectX::XMFLOAT4X4 returnMat;
		returnMat._11 = mat->a1;
		returnMat._12 = mat->a2;
		returnMat._13 = mat->a3;
		returnMat._14 = mat->a4;

		returnMat._21 = mat->b1;
		returnMat._22 = mat->b2;
		returnMat._23 = mat->b3;
		returnMat._24 = mat->b4;

		returnMat._31 = mat->c1;
		returnMat._32 = mat->c2;
		returnMat._33 = mat->c3;
		returnMat._34 = mat->c4;

		returnMat._41 = mat->d1;
		returnMat._42 = mat->d2;
		returnMat._43 = mat->d3;
		returnMat._44 = mat->d4;

		return returnMat;
	}
	*/

	/*
	void UpdateBoneMatrixBuffer(ComPtr<ID3D11DeviceContext>& immediateContext);
	{
		//DirectX::XMFLOAT4X4* boneMatricies = new DirectX::XMFLOAT4X4[bones.size()];
		for (size_t i = 0; i < bones.size(); i++)
		{
			DirectX::XMStoreFloat4x4(&boneMatricies[i], ToXM(bones[i].posedMatrix));
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		immediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, boneMatricies, bones.size() * sizeof(DirectX::XMFLOAT4X4));
		immediateContext->Unmap(matrixBuffer.Get(), 0);

		//delete[] boneMatricies;
	}
	*/

	/*
	static void CalculateVertexBoneInfo(std::vector<Vertex>* verticies, std::vector<VertexGroup>* vertexGroups);
	{
		std::cout << "Calculating Vertex Bone Info...";
		for (size_t i = 0; i < verticies->size(); i++)
		{
			std::vector<BoneWeightPair> boneWeights;
			for (size_t j = 0; j < vertexGroups->size(); j++)
			{
				VertexGroup group = vertexGroups->at(j);
				for (size_t k = 0; k < group.verticies.size(); k++)
				{
					if (group.verticies[k].vertex == i)
					{
						BoneWeightPair newPair;
						newPair.bone = group.index;
						newPair.weight = group.verticies[k].weight;
						boneWeights.push_back(newPair);
						break;
					}
				}
			}
			std::sort(boneWeights.begin(), boneWeights.end());

			if (boneWeights.size() > 8)
			{
				while (boneWeights.size() > 8)
				{
					boneWeights.pop_back();
				}
			}
			else if (boneWeights.size() < 8)
			{
				while (boneWeights.size() < 8)
				{
					BoneWeightPair empty;
					empty.bone = 0;
					empty.weight = 0;
					boneWeights.push_back(empty);
				}
			}

			for (size_t j = 0; j < 4; j++)
			{
				verticies->at(i).bones0[j] = boneWeights[j].bone;
				verticies->at(i).weights0[j] = boneWeights[j].weight;
			}
			for (size_t j = 4; j < 8; j++)
			{
				verticies->at(i).bones1[j - 4] = boneWeights[j].bone;
				verticies->at(i).weights1[j - 4] = boneWeights[j].weight;
			}
		}
		std::cout << "done!" << std::endl;
	}
	*/

	/*
	void ResolveBoneParentIndicies();
	{
		for (size_t i = 0; i < bones.size(); i++)
		{
			bones[i].parentIndex = -1;
			for (size_t j = 0; j < bones.size(); j++)
			{
				if (bones[i].parentName == bones[j].name)
				{
					bones[i].parentIndex = j;
					break;
				}
			}
		}
	}
	*/

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