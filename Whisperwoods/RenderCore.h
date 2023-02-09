#pragma once

#include "Window.h"
#include "Camera.h"
#include "ConstantbufferData.h"
#include "Pipeline.h"
#include "Renderable.h"

class RenderCore
{
public:
	RenderCore(shared_ptr<Window> window);
	~RenderCore();

	void NewFrame();
	void EndFrame();

	HRESULT CreateVertexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const;
	HRESULT CreateIndexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const;
	HRESULT CreateImageTexture(char* image, UINT resHeight, UINT resWidth, UINT sysMemPitch, DXGI_FORMAT format, ID3D11Texture2D** out_texturePP);
	void LoadImageTexture(const std::wstring& filePath, ComPtr<ID3D11Texture2D>& textureResource) const;

	void UpdateViewInfo(const Camera& camera);
	void UpdateObjectInfo(const WorldRenderable* worldRenderable);
	void DrawObject(const Renderable* renderable);

	void SetVertexBuffer(ComPtr<ID3D11Buffer> buffer, uint stride, uint offset);
	void SetIndexBuffer(ComPtr<ID3D11Buffer> buffer, uint offset, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
	void DrawIndexed(uint indexCount, uint start, uint base);

	void InitImGui() const;

private:
	void BindPipeline(PipelineType pipeline);

	void InitPipelines();
	void InitConstantBuffers();

private:
	shared_ptr<Window> m_window;

	// Core
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;

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

	// Pipelines
	Pipeline m_pipelines[PipelineTypeCount];
	int m_pipelineCurrent;

	// Constant buffers
	ConstantBuffers m_constantBuffers;
};