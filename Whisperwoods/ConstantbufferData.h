#pragma once
#include "BindInfo.h"

namespace CB
{
	struct ViewInfo
	{
		Mat4 viewMatrix;
		Mat4 projectionMatrix;
	};

	struct ObjectInfo
	{
		Mat4 worldMatrix;
	};

	//...
	struct PSData
	{

	};
	// etc
}

struct ConstantBuffers
{
	ComPtr<ID3D11Buffer> viewInfo;
	ComPtr<ID3D11Buffer> objectInfo;
};