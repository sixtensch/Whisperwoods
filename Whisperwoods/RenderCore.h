#pragma once

#include "Window.h"
#include "Camera.h"

enum PIPELINE_TYPE
{
	BLINN_PHONG = 0,
	SHADOW,
	PPFX,
	ANIMATION,
	PARTICLE,
	TEXT
	// Stamina bar pipeline
};

class RenderCore
{
private:
	struct ShaderData
	{
		ComPtr<ID3D11InputLayout>    inputLayout;

		ComPtr<ID3D11VertexShader>   vertexShader;
		ComPtr<ID3D11DomainShader>   domainShader;
		ComPtr<ID3D11HullShader>     hullShader;
		ComPtr<ID3D11GeometryShader> geometryShader;
		ComPtr<ID3D11PixelShader>    pixelShader;
	};
public:
	RenderCore(shared_ptr<Window> window, const Camera& camera);
	~RenderCore();

	void NewFrame();

	void EndFrame();

	void BindGPipeline(ID3D11Buffer* const* vertexBufferPP, ID3D11Buffer* indexBufferP, const UINT& stride, const UINT& offset, PIPELINE_TYPE flag);
	// TODO: add functions for the handler to modify the data in the constant buffers

private:
	void CompileShaders();
	void CreateVSConstantBuffers(const Camera& camera);

	// Helper functions for the binder
	void BindBlinnPhong();

private:
	shared_ptr<Window> m_window;

	// Core
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;

	// Shaders
	ShaderData m_shaders;
	ComPtr<ID3D11Buffer> m_vertexShaderCBuffer; // Should this be moved to the handler?


	// Back buffer
	ComPtr<ID3D11Texture2D> m_bbTexture;
	ComPtr<ID3D11RenderTargetView> m_bbRTV;
	//ComPtr<ID3D11ShaderResourceView> m_bbSRV;
	cs::Color4f m_bbClearColor;

	// Depth stencil
	ComPtr<ID3D11Texture2D> m_dsTexture;
	ComPtr<ID3D11DepthStencilView> m_dsDSV;
	ComPtr<ID3D11ShaderResourceView> m_dsSRV;

	D3D11_VIEWPORT m_viewport;

};