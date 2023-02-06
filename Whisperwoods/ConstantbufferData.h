#pragma once
#include "Core.h"

namespace CB
{
	struct VSData
	{
		Mat4 worldMatrix;
		Mat4 viewMatrix;
		Mat4 ProjectionMatrix;
	};
	//...
	struct PSData
	{

	};
	// etc
}

struct ConstantBuffers
{
	ComPtr<ID3D11Buffer> vertexShaderCBuffer;
};