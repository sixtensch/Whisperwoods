#include "Core.h"
#include "Armature.h"


Armature::Armature()
{
	numBones = -1;
	//initialized = false;
}

Armature::~Armature()
{
	// nothing
}

//void Armature::InitializeMatrix(int numB)
//{
//	for (int i = 0; i < numB; i++)
//	{
//		boneMatricies.Add(Mat4());
//	}
//	numBones = numB;
//	//initialized = true;
//}

void Armature::InitializeGPUResources(ComPtr<ID3D11Device>& device)
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
}

void Armature::UpdateBoneMatrixBuffer(ComPtr<ID3D11DeviceContext>& immediateContext)
{
	//DirectX::XMFLOAT4X4* boneMatricies = new DirectX::XMFLOAT4X4[bones.size()];
	for (size_t i = 0; i < bones.Size(); i++)
	{
		boneMatricies[i] = bones[i].posedMatrix;
		//DirectX::XMStoreFloat4x4(&boneMatricies[i], bones[i].posedMatrix.XMMatrix());
	}
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	immediateContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, boneMatricies.Data(), bones.Size() * sizeof(Mat4));
	immediateContext->Unmap(matrixBuffer.Get(), 0);
	//delete[] boneMatricies;
}

/* IS DONE IN THE IMPORTER NOW.
void Armature::CalculateVertexBoneInfo(cs::List<VertexRigged>* verticies, cs::List<VertexGroup>* vertexGroups)
{
	std::cout << "Calculating Vertex Bone Info...";
	for (size_t i = 0; i < verticies->Size(); i++)
	{
		std::vector<BoneWeightPair> boneWeights;
		for (size_t j = 0; j < vertexGroups->Size(); j++)
		{
			VertexGroup group = vertexGroups->At(j);
			for (size_t k = 0; k < group.verticies.Size(); k++)
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

		if (boneWeights.size() > 4)
		{
			while (boneWeights.size() > 4) // TODO: Change to a constant for amount of bones possible.
			{
				boneWeights.pop_back();
			}
		}
		else if (boneWeights.size() < 4)
		{
			while (boneWeights.size() < 4)
			{
				BoneWeightPair empty;
				empty.bone = 0;
				empty.weight = 0;
				boneWeights.push_back(empty);
			}
		}

		for (size_t j = 0; j < 4; j++)
		{
			verticies->At(i).bones[j] = boneWeights[j].bone;
			verticies->At(i).weights[j] = boneWeights[j].weight;
		}
		//for (size_t j = 4; j < 8; j++) // for 8 bone influences
		//{
		//	verticies->At(i).bones1[j - 4] = boneWeights[j].bone;
		//	verticies->At(i).weights1[j - 4] = boneWeights[j].weight;
		//}
	}
	std::cout << "done!" << std::endl;
}
*/

void Armature::ResolveBoneParentIndicies()
{
	for (size_t i = 0; i < bones.Size(); i++)
	{
		bones[i].parentIndex = -1;
		for (size_t j = 0; j < bones.Size(); j++)
		{
			if (bones[i].parentName == bones[j].name)
			{
				bones[i].parentIndex = j;
				break;
			}
		}
	}
}

