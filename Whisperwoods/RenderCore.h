#pragma once



class RenderCore
{
public:
	RenderCore(UINT width, UINT height, HWND window);
	~RenderCore();


	
private:
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;

	D3D11_VIEWPORT m_viewPort;
};