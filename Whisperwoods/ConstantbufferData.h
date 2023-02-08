#pragma once
#include "BindInfo.h"

namespace CB
{
	struct VSViewInfo
	{
		Mat4 viewMatrix;
		Mat4 projectionMatrix;
	};

	struct VSObjectInfo
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
	ComPtr<ID3D11Buffer> vsViewInfo;
	ComPtr<ID3D11Buffer> vsObjectInfo;
};