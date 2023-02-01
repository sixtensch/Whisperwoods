#pragma once

#include "Window.h"

class RenderCore
{
public:
	RenderCore(shared_ptr<Window> window);
	~RenderCore();


	
private:
	shared_ptr<Window> m_window;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;

	D3D11_VIEWPORT m_viewPort;
};