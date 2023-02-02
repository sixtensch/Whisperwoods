#pragma once

#include "Window.h"

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
	RenderCore(shared_ptr<Window> window);
	~RenderCore();

	void NewFrame();

	void EndFrame();
	
private:
	HRESULT CompileShaders();
	HRESULT CreateVSConstantBuffer();

private:
	shared_ptr<Window> m_window;

	// Core
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;

	// Shaders
	ShaderData m_shaders;
	ComPtr<ID3D11Buffer> m_vsCBuffer;


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