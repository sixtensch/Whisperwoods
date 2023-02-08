#pragma once
#include "Light.h"

enum LightType
{
	LightTypeDirectional = 0,
	LightTypeSpotlight = 1,
	LightTypePointlight = 2
};

constexpr int START_MAX_LIGHTS = 40;

class LightHandler sealed
{
public:
	LightHandler();
	~LightHandler();

	void Init(ID3D11Device* device, UINT startMaxDirectionalLight, UINT startMaxSpotlight, UINT startMaxPointLight);

	ID3D11Buffer* GetStructuredBufferP( LightType type );
	ID3D11Buffer* const* GetStructuredBufferPP( LightType type );
	ID3D11ShaderResourceView* GetSRV(LightType type);
	
	void SetDirectionalLights( ID3D11DeviceContext* context, const cs::List<DirectionalLight::DirectionalLightBufferData>& data);
	void SetSpotLights( ID3D11DeviceContext* context, const cs::List<SpotLight::SpotLightBufferData>& data );
	void SetPointLights( ID3D11DeviceContext* context, const cs::List<PointLight::PointLightBufferData>& data );

	bool ReallocLights( ID3D11Device* device, LightType type );

private:
	bool InitPointSBuffer( ID3D11Device* device, UINT LightCount );
	bool InitDirSBuffer( ID3D11Device* device, UINT LightCount );
	bool InitSpotSBuffer( ID3D11Device* device, UINT LightCount );

private:
	ComPtr<ID3D11Buffer> m_pointSBuffer;
	ComPtr<ID3D11Buffer> m_spotSBuffer;
	ComPtr<ID3D11Buffer> m_dirSBuffer;

	ComPtr<ID3D11ShaderResourceView> m_SRVList[3];

	int m_maxSize[3];
};

