#include "core.h"
#include "LightHandler.h"

LightHandler::LightHandler( ID3D11Device* device )
{
	m_maxSize[LightTypeDirectional] = START_MAX_LIGHTS;
	m_maxSize[LightTypeSpotlight]   = START_MAX_LIGHTS;
	m_maxSize[LightTypePointlight]  = START_MAX_LIGHTS;

	if ( !InitDirSBuffer( device, m_maxSize[LightTypeDirectional] ) )
	{
		EXC( "Error creating light resource at LightHandler.cpp, line " + __LINE__ );
	};
	if( !InitPointSBuffer( device, m_maxSize[LightTypePointlight]) )
	{
		EXC( "Error creating light resource at LightHandler.cpp, line " + __LINE__ );
	};
	if ( !InitSpotSBuffer( device, m_maxSize[LightTypeSpotlight]) )
	{
		EXC( "Error creating light resource at LightHandler.cpp, line " + __LINE__ );
	};
}
LightHandler::~LightHandler()
{}


ID3D11Buffer* LightHandler::GetStructuredBufferP( LightType type )
{
	switch ( type )
	{
		case LightTypeDirectional: return m_dirSBuffer.Get();
		case LightTypePointlight: return m_pointSBuffer.Get();
		case LightTypeSpotlight: return m_spotSBuffer.Get();
	}
	return nullptr;
}
ID3D11Buffer* const* LightHandler::GetStructuredBufferPP( LightType type )
{
	switch ( type )
	{
		case LightTypeDirectional: return m_dirSBuffer.GetAddressOf();
		case LightTypePointlight: return m_pointSBuffer.GetAddressOf();
		case LightTypeSpotlight: return m_spotSBuffer.GetAddressOf();
	}
	return nullptr;
}

void LightHandler::SetDirectionalLights(ID3D11DeviceContext* context, const cs::List<DirectionalLight::DirectionalLightBufferData>& data)
{
	HRESULT hr = {};

	// Update light buffer
	D3D11_MAPPED_SUBRESOURCE Resource = {};
	hr = context->Map( m_dirSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Resource);
	if ( FAILED( hr ) )
	{
		LOG_CRITICAL( "Failed to get map for directional structured buffer in SetDirectionalLights() function call, line: ");
		return;
	}
	memcpy(
		Resource.pData,
		data.Data(),
		sizeof(DirectionalLight) * data.Size()
	);
	context->Unmap( m_dirSBuffer.Get(), 0 );
}
void LightHandler::SetSpotLights( ID3D11DeviceContext* context, const cs::List<SpotLight::SpotLightBufferData>& data )
{
	HRESULT hr = {};

	// Update light buffer
	D3D11_MAPPED_SUBRESOURCE Resource = {};
	hr = context->Map( m_spotSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Resource );
	if ( FAILED( hr ) )
	{
		LOG_CRITICAL( "Failed to get map for directional structured buffer in SetSpotLights() function call, line: " + __LINE__ );
		return;
	}
	memcpy(
		Resource.pData,
		data.Data(),
		sizeof( SpotLight ) * data.Size()
	);
	context->Unmap( m_spotSBuffer.Get(), 0 );
}
void LightHandler::SetPointLights( ID3D11DeviceContext* context, const cs::List<PointLight::PointLightBufferData>& data )
{
	HRESULT hr = {};

	// Update light buffer
	D3D11_MAPPED_SUBRESOURCE Resource = {};
	hr = context->Map( m_pointSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Resource );
	if ( FAILED( hr ) )
	{
		LOG_CRITICAL( "Failed to get map for directional structured buffer in SetPointLights() function call, line: ");
		return;
	}
	memcpy(
		Resource.pData,
		data.Data(),
		sizeof( PointLight ) * data.Size()
	);
	context->Unmap( m_pointSBuffer.Get(), 0 );
}


bool LightHandler::InitDirSBuffer( ID3D11Device* device, UINT LightCount )
{
	HRESULT hr = {};
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth           = LightCount * sizeof( DirectionalLight::DirectionalLightBufferData);
	bufferDesc.StructureByteStride = sizeof( DirectionalLight::DirectionalLightBufferData);
	bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(
		&bufferDesc,
		NULL,
		m_dirSBuffer.GetAddressOf() );

	if ( FAILED( hr ) )
		LOG_CRITICAL("Failure constructing the directional light structuredbuffer line: ");
	return SUCCEEDED( hr );
}
bool LightHandler::InitPointSBuffer( ID3D11Device* device, UINT LightCount )
{
	HRESULT hr = {};
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth           = LightCount * sizeof( PointLight::PointLightBufferData);
	bufferDesc.StructureByteStride = sizeof( PointLight::PointLightBufferData);
	bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(
		&bufferDesc,
		NULL,
		m_pointSBuffer.GetAddressOf() );

	if ( FAILED( hr ) )
		LOG_CRITICAL( "Failure constructing the pointlight structuredbuffer line: " + __LINE__ );
	return SUCCEEDED( hr );
}
bool LightHandler::InitSpotSBuffer( ID3D11Device* device, UINT LightCount )
{
	HRESULT hr = {};
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth           = LightCount * sizeof( SpotLight::SpotLightBufferData);
	bufferDesc.StructureByteStride = sizeof( SpotLight::SpotLightBufferData);
	bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(
		&bufferDesc,
		NULL,
		m_spotSBuffer.GetAddressOf() );

	if ( FAILED( hr ) )
		LOG_CRITICAL( "Failure constructing the spotlight structuredbuffer line: " + __LINE__ );
	return SUCCEEDED( hr );
}


bool LightHandler::ReallocLights( ID3D11Device* device, LightType type )
{
	bool toReturn = false;
	switch ( type )
	{
		case LightTypeDirectional:
			m_dirSBuffer.Get()->Release();
			m_maxSize[LightTypeDirectional] *= 2;
			toReturn = InitDirSBuffer( device, m_maxSize[LightTypeDirectional] );
			break;

		case LightTypeSpotlight:
			m_dirSBuffer.Get()->Release();
			m_maxSize[LightTypeSpotlight] *= 2;
			toReturn = InitSpotSBuffer( device, m_maxSize[LightTypeSpotlight] );
			break;

		case LightTypePointlight:
			m_dirSBuffer.Get()->Release();
			m_maxSize[LightTypePointlight] *= 2;
			toReturn = InitPointSBuffer( device, m_maxSize[LightTypePointlight] );
			break;

		default:
			break;
	}
	return toReturn;
}
