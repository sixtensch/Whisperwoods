#include "Core.h"
#include "ModelResource.h"
#include "Debug.h"

void ModelResource::CreateIndexBuffer(ComPtr<ID3D11Device>& device)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(unsigned int) * indicies.Size();
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &indicies;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &indexBuffer);
	if (FAILED(hr))
	{
		LOG_ERROR("failed to create model indexBuffer");
	}
}

void ModelStaticResource::CreateVertexBuffer(ComPtr<ID3D11Device>& device)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(VertexTextured) * verticies.Size();
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0; // not relevant

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &verticies;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	if (FAILED(hr))
	{
		LOG_ERROR("failed to create static model vertexBuffer");
	}

	CreateIndexBuffer(device);
}

void ModelRiggedResource::CreateVertexBuffer(ComPtr<ID3D11Device>& device)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(VertexRigged) * verticies.Size();
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0; // not relevant

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &verticies;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	if (FAILED(hr))
	{
		LOG_ERROR("failed to create rigged model vertexBuffer");
	}
	
	CreateIndexBuffer(device);
}
