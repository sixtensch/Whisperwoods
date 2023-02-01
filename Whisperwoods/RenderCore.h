#pragma once



class RenderCore
{
public:
	RenderCore(UINT width, UINT height, HWND window);
	~RenderCore();

	
private:
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	//ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	//ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ComPtr<IDXGISwapChain> m_swapChain;
	//ComPtr<ID3D11InputLayout> m_inputLayout;
	//ComPtr<ID3D11DepthStencilView> m_depthStencilView;


	D3D11_VIEWPORT m_viewPort;

	
};