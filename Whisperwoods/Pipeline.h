#pragma once

enum PipelineType
{
	PipelineTypeStandard = 0,
	PipelineTypeStandardRigged,
	PipelineTypeEnvironment,
	PipelineTypeShadow,
	PipelineTypeParticle,
	PipelineTypePPFX,
	PipelineTypeText,
	PipelineTypeGUI,
	PipelineTypePrepass,
	PipelineTypeTerrain,

	PipelineTypeCount // <-- Keep last
};

struct Pipeline
{
	ComPtr<ID3D11InputLayout>		inputLayout			= nullptr;
	D3D_PRIMITIVE_TOPOLOGY			primitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ComPtr<ID3D11VertexShader>		vertexShader		= nullptr;
	ComPtr<ID3D11GeometryShader>	geometryShader		= nullptr;
	ComPtr<ID3D11DomainShader>		domainShader		= nullptr;
	ComPtr<ID3D11HullShader>		hullShader			= nullptr;
	ComPtr<ID3D11PixelShader>		pixelShader			= nullptr;
};

