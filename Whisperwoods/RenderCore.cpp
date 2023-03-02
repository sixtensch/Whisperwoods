#include "Core.h"

#include "RenderCore.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <d3dcompiler.h>
#include <WICTextureLoader.h>


#define RTV_COUNT 2u // TODO: Maybe make this a const member variable for more official use? 

RenderCore::RenderCore(shared_ptr<Window> window)
{
	m_window = window;

	DXGI_SWAP_CHAIN_DESC desc = {};

	desc.BufferDesc.Width = window->GetWidth();  // width/height of window
	desc.BufferDesc.Height = window->GetHeight();
	desc.BufferDesc.RefreshRate.Numerator = 0; // RefreshRate 60 hertz (0 from the beginning) 
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; // 32 bits with 8 per channel
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Scanline order unspecified
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //  scaling is unspecified
	desc.SampleDesc.Count = 1; // one desc
	desc.SampleDesc.Quality = 0; //default
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS; //use resource or surface as result of rendering
	desc.BufferCount = 3; 
	desc.OutputWindow = window->Data();
	desc.Windowed = true;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //deletes contents of backup buffer when called on
	desc.Flags = 0;

	UINT flags = 0;

#ifdef WW_DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1 };

	EXC_COMCHECK(D3D11CreateDeviceAndSwapChain(
		nullptr, //IDXGI Adapter
		D3D_DRIVER_TYPE_HARDWARE, // Program selects, since first parameter is nullptr
		nullptr, // driver type of software
		flags, // which runtime layers we enable, which are "flags"
		featureLevels, // pointer to array which determines order of feature levels created. in this case D3D_FEATURE_LEVEL_11_0
		1, //element is called D3D_FEATURE_LEVEL_11_0
		D3D11_SDK_VERSION, // Standard sdk_version used
		&desc, // swapchain desc
		&m_swapChain, // adress of swapchain
		&m_device, // adress of device
		nullptr, // no definition needed
		&m_context// adress of immidiatecontext
	));



	// Setup viewport

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;
	m_viewport.Width = static_cast<float>(window->GetWidth());
	m_viewport.Height = static_cast<float>(window->GetHeight());

	UINT shadowMapHeight = 2048;
	UINT shadowMapWidth = 2048;
	m_shadowViewport.TopLeftX = 0;
	m_shadowViewport.TopLeftX = 0;
	m_shadowViewport.MinDepth = 0;
	m_shadowViewport.MaxDepth = 1;
	m_shadowViewport.Width = (float)shadowMapWidth;
	m_shadowViewport.Height = (float)shadowMapHeight;

	// Setup back buffer

	EXC_COMCHECK(m_swapChain->GetBuffer(0u, __uuidof(ID3D11Texture2D), (void**)&m_bbTexture));

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D = { 0 };

	EXC_COMCHECK(m_device->CreateRenderTargetView(m_bbTexture.Get(), &rtvd, &m_bbRTV));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	uavd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavd.Texture2D = { 0 };

	EXC_COMCHECK(m_device->CreateUnorderedAccessView(m_bbTexture.Get(), &uavd, &m_bbUAV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };
	
	EXC_COMCHECK(m_device->CreateShaderResourceView(m_bbTexture.Get(), &srvd, &m_bbSRV));

	m_bbClearColor = cs::Color4f(0.0f, 0.0f, 0.0f, 1.0f);

	// Main render texture

	D3D11_TEXTURE2D_DESC rttd;
	rttd.Width = window->GetWidth();
	rttd.Height = window->GetHeight();
	rttd.MipLevels = 1u;
	rttd.ArraySize = 1u;
	rttd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rttd.SampleDesc = { 1u, 0u };
	rttd.Usage = D3D11_USAGE_DEFAULT;
	rttd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rttd.CPUAccessFlags = 0u;
	rttd.MiscFlags = 0u;

	EXC_COMCHECK(m_device->CreateTexture2D(&rttd, nullptr, m_renderTexture.GetAddressOf()));
	EXC_COMCHECK(m_device->CreateTexture2D(&rttd, nullptr, m_renderTextureCopy.GetAddressOf()));

	rtvd = {};
	rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D = { 0u };

	EXC_COMCHECK(m_device->CreateRenderTargetView(m_renderTexture.Get(), &rtvd, m_renderTextureRTV.GetAddressOf()));
	
	uavd = {};
	uavd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavd.Texture2D = { 0 };

	EXC_COMCHECK(m_device->CreateUnorderedAccessView(m_renderTexture.Get(), &uavd, m_renderTextureUAV.GetAddressOf()));
	
	srvd = {};
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };

	EXC_COMCHECK(m_device->CreateShaderResourceView(m_renderTexture.Get(), &srvd, m_renderTextureSRV.GetAddressOf()));
	EXC_COMCHECK(m_device->CreateShaderResourceView(m_renderTextureCopy.Get(), &srvd, m_renderTextureCopySRV.GetAddressOf()));

	// PPFX Textures and views

	D3D11_TEXTURE2D_DESC bloomtd;
	m_renderTexture->GetDesc(&bloomtd);
	bloomtd.MipLevels = BLOOM_MIP_LEVELS;
	bloomtd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS; // Has to bind as RTV for GenerateMips() to work, even though its not used.
	bloomtd.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	EXC_COMCHECK(m_device->CreateTexture2D(&bloomtd, nullptr, m_ppfxLumTexture.GetAddressOf()));

	
	srvd = {};
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, (UINT)-1 }; // Keep all mips as this will be used in compute.

	// Create views for luminosity texture.
	EXC_COMCHECK(m_device->CreateShaderResourceView(m_ppfxLumTexture.Get(), &srvd, m_ppfxLumSRV.GetAddressOf()));
	EXC_COMCHECK(m_device->CreateUnorderedAccessView(m_ppfxLumTexture.Get(), nullptr, m_ppfxLumUAV.GetAddressOf()));
	EXC_COMCHECK(m_device->CreateRenderTargetView(m_ppfxLumTexture.Get(), nullptr, m_ppfxLumRTV.GetAddressOf()));


	bloomtd = {};
	m_renderTexture->GetDesc(&bloomtd);
	bloomtd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	bloomtd.MipLevels = 1u;
	bloomtd.MiscFlags = 0u;

	EXC_COMCHECK(m_device->CreateTexture2D(&bloomtd, nullptr, m_ppfxLumSumTexture.GetAddressOf()));

	srvd = {};
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1u }; // Keep all mips as this will be used in compute.
	EXC_COMCHECK(m_device->CreateShaderResourceView(m_ppfxLumSumTexture.Get(), &srvd, m_ppfxLumSumSRV.GetAddressOf()));
	EXC_COMCHECK(m_device->CreateUnorderedAccessView(m_ppfxLumSumTexture.Get(), nullptr, m_ppfxLumSumUAV.GetAddressOf()));

	// Position texture

	D3D11_TEXTURE2D_DESC postd;
	postd.Width = window->GetWidth();
	postd.Height = window->GetHeight();
	postd.MipLevels = 1u;
	postd.ArraySize = 1u;
	postd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	postd.Usage = D3D11_USAGE_DEFAULT;
	postd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	postd.SampleDesc = { 1u, 0u };
	postd.CPUAccessFlags = 0;
	postd.MiscFlags = 0u;
	
	EXC_COMCHECK(m_device->CreateTexture2D(&postd, nullptr, m_positionTexture.GetAddressOf()));
	
	// Create views for position texture.
 	EXC_COMCHECK(m_device->CreateShaderResourceView(m_positionTexture.Get(), nullptr, m_positionTextureSRV.GetAddressOf()));
	EXC_COMCHECK(m_device->CreateRenderTargetView(m_positionTexture.Get(), nullptr, m_positionTextureRTV.GetAddressOf()));

	// Depth stencil

	D3D11_DEPTH_STENCIL_DESC dssDesc = {};
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	EXC_COMCHECK(m_device->CreateDepthStencilState(&dssDesc, &m_dsDSS));

	D3D11_TEXTURE2D_DESC dstDesc;
	dstDesc.Width = window->GetWidth();
	dstDesc.Height = window->GetHeight();
	dstDesc.MipLevels = 1;
	dstDesc.ArraySize = 1;
	dstDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	dstDesc.SampleDesc = { 1, 0 };
	dstDesc.Usage = D3D11_USAGE_DEFAULT;
	dstDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dstDesc.CPUAccessFlags = 0;
	dstDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA dstSRD;
	dstSRD.pSysMem = nullptr;
	dstSRD.SysMemPitch = window->GetWidth() * 4;

	EXC_COMCHECK(m_device->CreateTexture2D(&dstDesc, nullptr, &m_dsTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC dstSRVD;
	dstSRVD.Format = DXGI_FORMAT_R32_FLOAT;
	dstSRVD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dstSRVD.Texture2D = { 0, 1 };

	EXC_COMCHECK(m_device->CreateShaderResourceView(m_dsTexture.Get(), &dstSRVD, &m_dsSRV));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Texture2D.MipSlice = 0u;

	EXC_COMCHECK(m_device->CreateDepthStencilView(m_dsTexture.Get(), &dsvDesc, &m_dsDSV));


	D3D11_TEXTURE2D_DESC shadowMapDesc = {};
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = shadowMapHeight;
	shadowMapDesc.Width = shadowMapWidth;

	// Dynamic texture
	EXC_COMCHECK(m_device->CreateTexture2D(
		&shadowMapDesc,
		nullptr,
		m_shadowTexture.GetAddressOf()
	));

	// Static texture
	EXC_COMCHECK(m_device->CreateTexture2D(
		&shadowMapDesc,
		nullptr,
		m_shadowStaticTexture.GetAddressOf()
	));

	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSVDesc = {};
	shadowDSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	shadowDSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSVDesc.Texture2D.MipSlice = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC shadowSRVDesc = {};
	shadowSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shadowSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shadowSRVDesc.Texture2D.MipLevels = 1;

	// Dynamic texture
	EXC_COMCHECK(m_device->CreateDepthStencilView(
		m_shadowTexture.Get(),
		&shadowDSVDesc,
		m_shadowDSV.GetAddressOf()
	));
	EXC_COMCHECK(m_device->CreateShaderResourceView(
		m_shadowTexture.Get(),
		&shadowSRVDesc,
		m_shadowSRV.GetAddressOf()
	));

	// Static texture
	EXC_COMCHECK(m_device->CreateDepthStencilView(
		m_shadowStaticTexture.Get(),
		&shadowDSVDesc,
		m_shadowStaticDSV.GetAddressOf()
	));
	EXC_COMCHECK(m_device->CreateShaderResourceView(
		m_shadowStaticTexture.Get(),
		&shadowSRVDesc,
		m_shadowStaticSRV.GetAddressOf()
	));

	// Depth stencil state



	// Rasterizer

	D3D11_RASTERIZER_DESC rsd = {};
	rsd.AntialiasedLineEnable = false;
	rsd.CullMode = D3D11_CULL_BACK;
	rsd.DepthBias = 0;
	rsd.DepthBiasClamp = 0.0f;
	rsd.DepthClipEnable = true;
	rsd.FillMode = D3D11_FILL_SOLID;
	rsd.FrontCounterClockwise = false;
	rsd.MultisampleEnable = false; // 
	rsd.ScissorEnable = false;
	rsd.SlopeScaledDepthBias = 0.0f;

	EXC_COMCHECK(m_device->CreateRasterizerState(&rsd, &m_rasterizerState));

	D3D11_RASTERIZER_DESC shadowRSDesc = {};
	shadowRSDesc.CullMode = D3D11_CULL_BACK;
	shadowRSDesc.FillMode = D3D11_FILL_SOLID;
	shadowRSDesc.DepthClipEnable = true;

	EXC_COMCHECK(m_device->CreateRasterizerState(
		&shadowRSDesc,
		m_shadowRenderState.GetAddressOf()
	));
	// Blend state

	D3D11_RENDER_TARGET_BLEND_DESC rtbd = {};
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	uint sampleMask = 0xffffffff;
	
	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0] = rtbd;

	EXC_COMCHECK(m_device->CreateBlendState(&bd, &m_blendState));
	EXC_COMINFO(m_context->OMSetBlendState(m_blendState.Get(), nullptr, sampleMask));

	InitPipelines();
	InitComputeShaders();
	InitConstantBuffers();
	InitFont(m_fonts, &m_spriteBatch);
	InitDefaultMaterials();

	m_pipelineCurrent = -1;



	// Samplers

	D3D11_SAMPLER_DESC sd = {};

	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MipLODBias = 0.0f;
	sd.MaxAnisotropy = 1u;
	sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sd.BorderColor[0] = 0.0f;
	sd.BorderColor[1] = 0.0f;
	sd.BorderColor[2] = 0.0f;
	sd.BorderColor[3] = 0.0f;
	sd.MinLOD = 0.0f;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	EXC_COMCHECK(m_device->CreateSamplerState(&sd, &m_sampler));
	EXC_COMINFO(m_context->PSSetSamplers(RegSamplerStandard, 1, m_sampler.GetAddressOf()));

	D3D11_SAMPLER_DESC shadowSDesc = {};
	shadowSDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSDesc.BorderColor[0] = 1.0f;
	shadowSDesc.BorderColor[1] = 1.0f;
	shadowSDesc.BorderColor[2] = 1.0f;
	shadowSDesc.BorderColor[3] = 1.0f;
	shadowSDesc.MinLOD = 0.0f;
	shadowSDesc.MaxLOD = D3D11_FLOAT32_MAX;
	shadowSDesc.MipLODBias = 0.0f;
	shadowSDesc.MaxAnisotropy = 0;
	shadowSDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowSDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

	EXC_COMCHECK(m_device->CreateSamplerState(
		&shadowSDesc,
		m_shadowSampler.GetAddressOf()
	));
	EXC_COMINFO(m_context->PSSetSamplers(RegSamplerShadow, 1, m_shadowSampler.GetAddressOf()));

	sd = {};

	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.MipLODBias = 0.0f;
	sd.MaxAnisotropy = 1u;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 0.0f;
	sd.BorderColor[1] = 0.0f;
	sd.BorderColor[2] = 0.0f;
	sd.BorderColor[3] = 0.0f;
	sd.MinLOD = 0.0f;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	EXC_COMCHECK(m_device->CreateSamplerState(&sd, m_pointSampler.GetAddressOf()));
	EXC_COMINFO(m_context->PSSetSamplers(RegSamplerPoint, 1, m_pointSampler.GetAddressOf()));

	sd = {};

	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.MipLODBias = 0.0f;
	sd.MaxAnisotropy = 1u;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 0.0f;
	sd.BorderColor[1] = 0.0f;
	sd.BorderColor[2] = 0.0f;
	sd.BorderColor[3] = 0.0f;
	sd.MinLOD = 0.0f;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	EXC_COMCHECK(m_device->CreateSamplerState(&sd, m_bloomUpscaleSampler.GetAddressOf()));
}

RenderCore::~RenderCore()
{
}

void RenderCore::NewFrame()
{
	EXC_COMINFO(m_context->ClearDepthStencilView(m_dsDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));

	EXC_COMINFO(m_context->OMSetRenderTargets(1u, m_renderTextureRTV.GetAddressOf(), m_dsDSV.Get()));

	EXC_COMINFO(m_context->RSSetState(m_rasterizerState.Get()));
	EXC_COMINFO(m_context->OMSetBlendState(m_blendState.Get(), nullptr, 0xffffffff));
	EXC_COMINFO(m_context->OMSetDepthStencilState(m_dsDSS.Get(), 1));
}

void RenderCore::TargetPrepass()
{
	EXC_COMINFO(m_context->OMSetRenderTargets(0u, nullptr, m_dsDSV.Get()));
	EXC_COMINFO(m_context->RSSetState(m_rasterizerState.Get())); // Backface culling
	EXC_COMINFO(m_context->RSSetViewports(1u, &m_viewport));
}

void RenderCore::TargetShadowMap()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVShadowDepth, 1, &nullSRV)); // Unbind SRV to use as RTV

	EXC_COMINFO(m_context->CopyResource(m_shadowTexture.Get(), m_shadowStaticTexture.Get()));

	EXC_COMINFO(m_context->OMSetRenderTargets(0u, nullptr, m_shadowDSV.Get()));
	EXC_COMINFO(m_context->RSSetState(m_shadowRenderState.Get())); // Frontface culling
	EXC_COMINFO(m_context->RSSetViewports(1, &m_shadowViewport));
}

void RenderCore::TargetStaticShadowMap()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVShadowDepth, 1, &nullSRV)); // Unbind SRV to use as RTV

	EXC_COMINFO(m_context->ClearDepthStencilView(m_shadowStaticDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0));

	EXC_COMINFO(m_context->OMSetRenderTargets(0u, nullptr, m_shadowStaticDSV.Get()));
	EXC_COMINFO(m_context->RSSetState(m_shadowRenderState.Get())); // Frontface culling
	EXC_COMINFO(m_context->RSSetViewports(1, &m_shadowViewport));
}

void RenderCore::TargetRenderTexture()
{
	ID3D11RenderTargetView* rtvs[RTV_COUNT] = {
		m_renderTextureRTV.Get(), 
		m_positionTextureRTV.Get() 
	};

	EXC_COMINFO(m_context->OMSetRenderTargets(1u, rtvs, m_dsDSV.Get()));
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVShadowDepth, 1, m_shadowSRV.GetAddressOf()));
}

void RenderCore::UnbindRenderTexture()
{
	static ID3D11RenderTargetView* nullRTVs[RTV_COUNT] = {
		nullptr,
		nullptr
	};

	EXC_COMINFO(m_context->OMSetRenderTargets(RTV_COUNT, nullRTVs, nullptr));
}

void RenderCore::EndFrame()
{
	EXC_COMCHECK(m_swapChain->Present(0u, 0u));
}

void RenderCore::CreateVertexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const
{
	HRESULT hr = {};
	
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0; // not relevant

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	EXC_COMCHECK(m_device->CreateBuffer(&bufferDesc, &subData, out_bufferPP));
}

void RenderCore::CreateInstanceBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const
{
	HRESULT hr = {};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0; // not relevant

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	EXC_COMCHECK(m_device->CreateBuffer(&bufferDesc, data ? &subData : nullptr, out_bufferPP));
}

void RenderCore::CreateIndexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const
{
	HRESULT hr = {};
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//bufferDesc.StructureByteStride 

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	EXC_COMCHECK(m_device->CreateBuffer(&bufferDesc, &subData, out_bufferPP));
}

void RenderCore::CreateImageTexture(char* image, UINT resHeight, UINT resWidth, UINT sysMemPitch, DXGI_FORMAT format, ID3D11Texture2D** out_texturePP)
{
	HRESULT hr = {};
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = resHeight;
	textureDesc.Width = resWidth;
	textureDesc.Format = format;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = image;
	subData.SysMemPitch = sysMemPitch;
	subData.SysMemSlicePitch = 0;

	EXC_COMCHECK(m_device->CreateTexture2D(
		&textureDesc,
		&subData,
		out_texturePP
	));
}

void RenderCore::LoadImageTexture(const std::wstring& filePath, ComPtr<ID3D11Texture2D>& textureResource, ComPtr<ID3D11ShaderResourceView>& srv) const
{
	// Load texture using DXTK from filepath.

	ComPtr<ID3D11Resource> resource;
	
	EXC_COMCHECK(
		dx::CreateWICTextureFromFileEx(
			m_device.Get(),
			m_context.Get(),
			filePath.c_str(),
			0,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
			0,
			D3D11_RESOURCE_MISC_GENERATE_MIPS,
			dx::WIC_LOADER_IGNORE_SRGB | dx::WIC_LOADER_FORCE_RGBA32 | dx::WIC_LOADER_DEFAULT,
			&resource,
			&srv)
	);

	EXC_COMCHECK(resource->QueryInterface(IID_ID3D11Texture2D, (void**)textureResource.GetAddressOf()));

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	//srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvd.Texture2D.MostDetailedMip = 0;
	//srvd.Texture2D.MipLevels = 2;

	//EXC_COMCHECK(m_device->CreateShaderResourceView(textureResource.Get(), &srvd, &srv));
}

void RenderCore::DumpTexture(ID3D11Texture2D* texture, uint* outWidth, uint* outHeight, cs::Color4** newOutData) const
{
	D3D11_TEXTURE2D_DESC desc = {};
	texture->GetDesc(&desc);

	if (desc.Format != DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		EXC("DumpTexture only supports 4-byte RGBA texture formats.");
	}

	*outWidth = desc.Width;
	*outWidth = desc.Height;

	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	ComPtr<ID3D11Texture2D> stageTexture;

	EXC_COMCHECK(m_device->CreateTexture2D(
		&desc,
		nullptr,
		&stageTexture
	));

	D3D11_BOX box = {};
	box.right = desc.Width;
	box.bottom = desc.Height;
	box.back = 1;

	EXC_COMINFO(m_context->CopySubresourceRegion(stageTexture.Get(), 0, 0, 0, 0, texture, 0, &box));

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(stageTexture.Get(), 0u, D3D11_MAP_READ, 0u, &msr));

	*outWidth = desc.Width;
	*outHeight = desc.Height;
	*newOutData = new cs::Color4[desc.Width * desc.Height];

	for (uint i = 0; i < desc.Height; i++)
	{
		memcpy(*newOutData + i * desc.Width, (byte*)msr.pData + i * msr.RowPitch, desc.Width * sizeof(cs::Color4));
	}

	EXC_COMINFO(m_context->Unmap(stageTexture.Get(), 0u));
}

void RenderCore::CreateArmatureStructuredBuffer(ComPtr<ID3D11Buffer>& matrixBuffer, int numBones) const
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * numBones;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(DirectX::XMFLOAT4X4);
	//	D3D11_SUBRESOURCE_DATA data;
	//	data.pSysMem = boneMatricies;
	//	data.SysMemPitch = 0;
	//	data.SysMemSlicePitch = 0;
	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &matrixBuffer);
	if (FAILED(hr))
	{
		LOG_WARN("Failed to create bone matrix buffer.");
	}
	EXC_COMCHECK(hr);
}

void RenderCore::CreateArmatureSRV(ComPtr<ID3D11ShaderResourceView>& matrixSRV, ComPtr<ID3D11Buffer>& matrixBuffer, int numBones) const
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	//shaderResourceViewDesc.Buffer.ElementOffset = 0;
	shaderResourceViewDesc.Buffer.FirstElement = 0;
	shaderResourceViewDesc.Buffer.NumElements = numBones;
	//shaderResourceViewDesc.Buffer.ElementWidth = 64;
	//ID3D11Buffer* testBuffer = matrixBuffer.Get();
	EXC_COMCHECK(m_device->CreateShaderResourceView(matrixBuffer.Get(), &shaderResourceViewDesc, &matrixSRV));
}

void RenderCore::UpdateViewInfo(const Camera& camera)
{
	CB::ViewInfo vi =
	{
		camera.GetViewMatrix(),
		camera.GetProjectionMatrix().Transpose()
	};

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(m_constantBuffers.viewInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, &vi, sizeof(CB::ViewInfo));
	EXC_COMINFO(m_context->Unmap(m_constantBuffers.viewInfo.Get(), 0u));
}

void RenderCore::UpdateObjectInfo(const WorldRenderable* worldRenderable)
{
	CB::ObjectInfo oi =
	{
		worldRenderable->worldMatrix
	};

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(m_constantBuffers.objectInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, &oi, sizeof(CB::ObjectInfo));
	EXC_COMINFO(m_context->Unmap(m_constantBuffers.objectInfo.Get(), 0u));
}

void RenderCore::UpdatePlayerInfo( Mat4 matrix )
{
	CB::ObjectInfo oi =
	{
		matrix
	};

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK( m_context->Map( m_constantBuffers.playerInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr ) );
	memcpy( msr.pData, &oi, sizeof( CB::ObjectInfo ) );
	EXC_COMINFO( m_context->Unmap( m_constantBuffers.playerInfo.Get(), 0u ) );
}

void RenderCore::UpdateMaterialInfo(const MaterialResource* material) const
{
	if (material == nullptr)
	{
		UpdateMaterialInfo(&m_defaultMaterial);
		return;
	}

	// KEEP IN MIND that this relies on the fact that MaterialResource and MaterialInfo are aligned identically!
	const CB::MaterialInfo* mi = (const CB::MaterialInfo*)&(material->diffuse);

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(m_constantBuffers.materialInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, mi, sizeof(CB::MaterialInfo));
	EXC_COMINFO(m_context->Unmap(m_constantBuffers.materialInfo.Get(), 0u));

	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVTexDiffuse,	1,	(material->textureDiffuse	? material->textureDiffuse->shaderResourceView	: m_defaultDiffuseSRV)	.GetAddressOf()));
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVTexSpecular,	1,	(material->textureSpecular	? material->textureSpecular->shaderResourceView	: m_defaultSpecularSRV)	.GetAddressOf()));
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVTexEmissive,	1,	(material->textureEmissive	? material->textureEmissive->shaderResourceView	: m_defaultEmissiveSRV)	.GetAddressOf()));
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVTexNormal,	1,	(material->textureNormal	? material->textureNormal->shaderResourceView	: m_defaultNormalSRV)	.GetAddressOf()));
}

void RenderCore::UpdateInstanceBuffer(ComPtr<ID3D11Buffer> iBuffer, const Mat4* data, uint count)
{
	/*D3D11_BOX box =
	{
		0, 0, 0,
		count * sizeof(Mat4), 1, 1
	};

	EXC_COMINFO(m_context->UpdateSubresource(iBuffer.Get(), 0, &box, data, count * sizeof(Mat4), 1));*/

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(iBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, data, sizeof(Mat4) * count);
	EXC_COMINFO(m_context->Unmap(iBuffer.Get(), 0u));
}

void RenderCore::DrawObject(const Renderable* renderable, bool shadowing)
{
	if (renderable->pipelineType != m_pipelineCurrent || shadowing != m_shadowPSBound)
	{
		BindPipeline(renderable->pipelineType, shadowing);
	}

	DrawInfo drawInfo =
	{
		this
	};

	renderable->Draw(drawInfo);
}

void RenderCore::SetVertexBuffer(ComPtr<ID3D11Buffer> buffer, uint stride, uint offset)
{
	EXC_COMINFO(m_context->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &stride, &offset));
}

void RenderCore::SetInstanceBuffers(ComPtr<ID3D11Buffer> vBuffer, ComPtr<ID3D11Buffer> iBuffer, uint vStride, uint iStride, uint vOffset, uint iOffset)
{
	ID3D11Buffer* bArr[] = { vBuffer.Get(), iBuffer.Get() };
	uint sArr[] = { vStride, iStride };
	uint oArr[] = { vOffset, iOffset };

	m_context->IASetVertexBuffers(0, 2, bArr, sArr, oArr);
}

void RenderCore::SetIndexBuffer(ComPtr<ID3D11Buffer> buffer, uint offset, DXGI_FORMAT format)
{
	EXC_COMINFO(m_context->IASetIndexBuffer(buffer.Get(), format, offset));
}

void RenderCore::BindInstancedPipeline(bool shadowed)
{
	BindPipeline(PipelineTypeEnvironment, shadowed);
}

void RenderCore::DrawIndexed(uint indexCount, uint indexStart, uint vertexBase)
{
	EXC_COMINFO(m_context->DrawIndexed(indexCount, indexStart, vertexBase));
}

void RenderCore::DrawInstanced(uint indexCount, uint instanceCount, uint startIndex, uint baseVertex, uint startInstance)
{
	EXC_COMINFO(m_context->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance));
}

void RenderCore::DrawText(dx::SimpleMath::Vector2 fontPos, const wchar_t* m_text, Font font, cs::Color4f color, Vec2 originScalar)
{
   
	dx::XMVECTOR col = ((Vec4)color).GetXM3(); //converts from cs to xmvector, which Drawstring() needs

	m_spriteBatch->Begin();
	dx::SimpleMath::Vector2 origin = m_fonts[font]->MeasureString(m_text);
	origin = dx::SimpleMath::Vector2(origin.x * originScalar.x, origin.y * originScalar.y);

	m_fonts[font]->DrawString(m_spriteBatch.get(), m_text,
		fontPos + dx::SimpleMath::Vector2(1.f, 1.f), dx::Colors::Black, 0.f, origin);
	m_fonts[font]->DrawString(m_spriteBatch.get(), m_text,
		fontPos + dx::SimpleMath::Vector2(-1.f, 1.f), dx::Colors::Black, 0.f, origin);
	
	m_fonts[font]->DrawString(m_spriteBatch.get(), m_text,
		fontPos, col, 0.f, origin);
	m_spriteBatch->End();
}

void RenderCore::DrawPPFX()
{

	static float luminanceThreshold = 7.0f;
	static float strength = 1.0f;
	static float minLuminance = 0.1f;

	if (ImGui::Begin("Bloom Settings"))
	{
		float speed = 0.01f;
		ImGui::DragFloat("Luminance Threshold", (float*)&luminanceThreshold, speed, minLuminance, FLT_MAX);
		ImGui::DragFloat("Bloom Strength", &strength, speed, 0.0f, FLT_MAX);
		ImGui::DragFloat("Minimum Luminance", &minLuminance, speed / 10.0f, 0.0f, luminanceThreshold);
	}
	ImGui::End();

	WritePPFXThresholdInfo(luminanceThreshold, strength, minLuminance);

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11RenderTargetView* nullRTV = nullptr;

	const RegSRV renderTexSRVReg		= RegSRVCopySource;
	const RegUAV lumTexUAVReg			= RegUAVSystem0;
	const RegSRV lumTexSRVReg			= RegSRVUser0;
	
	// Luminance threshold pass
	{
		EXC_COMINFO(m_context->CSSetShader(m_thresholdCompute.Get(), nullptr, 0u));
		
		EXC_COMINFO(m_context->CSSetShaderResources(renderTexSRVReg, 1u, m_renderTextureSRV.GetAddressOf()));
		EXC_COMINFO(m_context->CSSetUnorderedAccessViews(lumTexUAVReg, 1u, m_ppfxLumUAV.GetAddressOf(), nullptr)); // Last argument is ignored.
		EXC_COMINFO(m_context->Dispatch(COMPUTE_GROUP_COUNT_X, COMPUTE_GROUP_COUNT_Y, 1u));
		
		// Generate all mips for lumen texture for artificial blur used in bloom pass.
		EXC_COMINFO(m_context->GenerateMips(m_ppfxLumSRV.Get()));
		EXC_COMINFO(m_context->CSSetSamplers(RegSamplerSystem0, 1u, m_bloomUpscaleSampler.GetAddressOf()));
		EXC_COMINFO(m_context->CSSetShaderResources(renderTexSRVReg, 1u, &nullSRV));
		EXC_COMINFO(m_context->CSSetUnorderedAccessViews(lumTexUAVReg, 1u, &nullUAV, nullptr));
	}
	
	// Bloom pass
	{
		EXC_COMINFO(m_context->CSSetShader(m_bloomCompute.Get(), nullptr, 0u));
		
		EXC_COMINFO(m_context->CSSetShaderResources(lumTexSRVReg, 1u, m_ppfxLumSRV.GetAddressOf()));
		EXC_COMINFO(m_context->CSSetUnorderedAccessViews(lumTexUAVReg, 1u, m_ppfxLumSumUAV.GetAddressOf(), nullptr));
		EXC_COMINFO(m_context->Dispatch(COMPUTE_GROUP_COUNT_X, COMPUTE_GROUP_COUNT_Y, 1u));
	
		EXC_COMINFO(m_context->CSSetShaderResources(lumTexSRVReg, 1u, &nullSRV)); // Unbind lum SRV.
		EXC_COMINFO(m_context->CSSetUnorderedAccessViews(lumTexUAVReg, 1u, &nullUAV, nullptr)); // Unbind render tex UAV from compute.
	}
	
	
}

void RenderCore::DrawPositionalEffects()
{
	static ID3D11UnorderedAccessView* nullUAV = nullptr;
	static ID3D11ShaderResourceView* nullSRV = nullptr;

	const RegUAV renderTexUAVReg = RegUAVRenderTarget;
	const RegSRV renderTexCopySRVReg = RegSRVCopySource;
	const RegSRV positionalTexSRVReg = RegSRVDefault;

	// Copies picture of render texture into other resource to use as SRV.
	m_context->CopyResource(m_renderTextureCopy.Get(), m_renderTexture.Get());

	// Draws all positional effects into render texture.
	{
		EXC_COMINFO(m_context->CSSetShader(m_positionalEffectCompute.Get(), nullptr, 0u));

		EXC_COMINFO(m_context->CSSetUnorderedAccessViews(renderTexUAVReg, 1u, m_renderTextureUAV.GetAddressOf(), nullptr));
		EXC_COMINFO(m_context->CSSetShaderResources(renderTexCopySRVReg, 1u, m_renderTextureCopySRV.GetAddressOf()));
		EXC_COMINFO(m_context->CSSetShaderResources(positionalTexSRVReg, 1u, m_positionTextureSRV.GetAddressOf()));
		EXC_COMINFO(m_context->Dispatch(COMPUTE_GROUP_COUNT_X, COMPUTE_GROUP_COUNT_Y, 1u));

		// Unbind views for other dependencies.
		EXC_COMINFO(m_context->CSSetShaderResources(renderTexCopySRVReg, 1u, &nullSRV));
		EXC_COMINFO(m_context->CSSetShaderResources(positionalTexSRVReg, 1u, &nullSRV));
		EXC_COMINFO(m_context->CSSetUnorderedAccessViews(renderTexUAVReg, 1u, &nullUAV, nullptr));
	}
}

void RenderCore::DrawToBackBuffer()
{

	static Vec2 vignette = Vec2(0.5f, 1.0f);
	static Vec2 contrast = Vec2(1.0f, 0.4f);
	static float brightness = 0.0f;
	static float saturation = 1.1f;

	if (ImGui::Begin("Color Settings"))
	{
		float speed = 0.01f;
		ImGui::DragFloat2("Vignette Radius & Strength", (float*)&vignette, speed, 0.0f, FLT_MAX);
		ImGui::DragFloat2("Contrast Amount & Midpoint", (float*)&contrast, speed, 0.0f);
		ImGui::DragFloat("Brightness", &brightness, speed, 0.0f, FLT_MAX);
		ImGui::DragFloat("Saturation", &saturation, speed, 0.0f, FLT_MAX);
	}
	ImGui::End();

	WritePPFXColorgradeInfo(vignette, contrast, brightness, saturation);

	// Color grade and final back buffer write.
	{
		EXC_COMINFO(m_context->CSSetShader(m_colorGradeCompute.Get(), nullptr, 0u));
		EXC_COMINFO(m_context->CSSetUnorderedAccessViews(RegUAVRenderTarget, 1u, m_bbUAV.GetAddressOf(), nullptr)); // Bind bb.
		EXC_COMINFO(m_context->CSSetShaderResources(RegSRVUser4, 1u, m_ppfxLumSumSRV.GetAddressOf()));
		EXC_COMINFO(m_context->CSSetShaderResources(RegSRVCopySource, 1u, m_renderTextureSRV.GetAddressOf())); // Bind render tex.
		EXC_COMINFO(m_context->Dispatch(COMPUTE_GROUP_COUNT_X, COMPUTE_GROUP_COUNT_Y, 1u));
	}

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	EXC_COMINFO(m_context->CSSetUnorderedAccessViews(RegUAVRenderTarget, 1u, &nullUAV, nullptr)); // Unbind bb UAV from compute.
	EXC_COMINFO(m_context->CSSetShaderResources(RegSRVCopySource, 1u, &nullSRV)); // Unbind render texture SRV from compute.
	EXC_COMINFO(m_context->CSSetShaderResources(RegSRVUser4, 1u, &nullSRV)); // Unbind lum sum texture SRV from compute.

	// TODO: Move this to start of text rendering instead.
	EXC_COMINFO(m_context->OMSetRenderTargets(1u, m_bbRTV.GetAddressOf(), m_dsDSV.Get()));

	// Make sure that lumsum texture is cleared.
	// Its either this or creating a new dispatch in bloom pass to write lum sum to render texture before inserting it to final color grading step.
	EXC_COMINFO(m_context->ClearUnorderedAccessViewFloat(m_ppfxLumSumUAV.Get(), (float*)&m_bbClearColor));
}

void RenderCore::DrawFullScreenQuad()
{
	static Vec2 vignette = Vec2(0.5f, 1.0f);
	static Vec2 contrast = Vec2(1.0f, 0.4f);
	static float brightness = 0.0f;
	static float saturation = 1.1f;

	if (ImGui::Begin("Color Settings"))
	{
		float speed = 0.01f;
		ImGui::DragFloat2("Vignette Radius & Strength", (float*)&vignette, speed, 0.0f, FLT_MAX);
		ImGui::DragFloat2("Contrast Amount & Midpoint", (float*)&contrast, speed, 0.0f);
		ImGui::DragFloat("Brightness", &brightness, speed, 0.0f, FLT_MAX);
		ImGui::DragFloat("Saturation", &saturation, speed, 0.0f, FLT_MAX);
	}
	ImGui::End();

	WritePPFXColorgradeInfo(vignette, contrast, brightness, saturation);

	// Copies picture of render texture into other resource to use as SRV.
	//m_context->CopyResource(m_renderTextureCopy.Get(), m_renderTexture.Get());

	EXC_COMINFO(m_context->OMSetRenderTargets(1u, m_bbRTV.GetAddressOf(), m_dsDSV.Get()));
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVCopySource, 1u, m_renderTextureSRV.GetAddressOf()));
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVUser4, 1u, m_ppfxLumSumSRV.GetAddressOf()));
	BindPipeline(PipelineTypeFullScreenQuad, false);

	m_context->Draw(4u, 0u);

	ID3D11RenderTargetView* nullrtv = nullptr;
	ID3D11ShaderResourceView* nullsrv = nullptr;
	//EXC_COMINFO(m_context->OMSetRenderTargets(1u, &nullrtv, nullptr));
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVCopySource, 1u, &nullsrv));
	EXC_COMINFO(m_context->PSSetShaderResources(RegSRVUser4, 1u, &nullsrv));
}

//void RenderCore::SetArmatureStructuredBuffer(ComPtr<ID3D11Buffer> matrixBuffer)
//{
//    EXC_COMINFO(m_context->DrawIndexed(indexCount, start, base));
//}

void RenderCore::SetArmatureArmatureSRV(ComPtr<ID3D11ShaderResourceView> matrixSRV)
{
	EXC_COMINFO(m_context->VSSetShaderResources(6, 1, matrixSRV.GetAddressOf()));
}

void RenderCore::UpdateBoneMatrixBuffer(ComPtr<ID3D11Buffer> matrixBuffer, cs::List<DirectX::XMFLOAT4X4> bones)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_context->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, bones.Data(), bones.Size() * sizeof(DirectX::XMFLOAT4X4));
	m_context->Unmap(matrixBuffer.Get(), 0);
}

void RenderCore::InitImGui() const
{
	ImGui_ImplWin32_Init(m_window->Data());
	ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
}

void RenderCore::InitFont(std::unique_ptr<dx::SpriteFont> font[FontCount], std::unique_ptr<dx::SpriteBatch>* batch) const
{
	//Push back each font
	font[FontDefault] = std::make_unique<dx::SpriteFont>(m_device.Get(), L"myfileb.spritefont");
  
	// Create spriteBatch;
	*batch = std::make_unique<dx::SpriteBatch>(m_context.Get());
}

void RenderCore::BindPipeline(PipelineType pipeline, bool shadowing)
{
	const Pipeline& n = m_pipelines[pipeline];  // New pipeline

	if (m_pipelineCurrent < 0)
	{
		EXC_COMINFO(m_context->IASetInputLayout(n.inputLayout.Get()));
		EXC_COMINFO(m_context->IASetPrimitiveTopology(n.primitiveTopology));

		EXC_COMINFO(m_context->VSSetShader(n.vertexShader.Get(), nullptr, 0));
		EXC_COMINFO(m_context->GSSetShader(n.geometryShader.Get(), nullptr, 0));
		EXC_COMINFO(m_context->DSSetShader(n.domainShader.Get(), nullptr, 0));
		EXC_COMINFO(m_context->HSSetShader(n.hullShader.Get(), nullptr, 0));

		if(!shadowing)
		{
			EXC_COMINFO(m_context->PSSetShader(n.pixelShader.Get(), nullptr, 0));
		}
		else
		{
			EXC_COMINFO(m_context->PSSetShader(nullptr, nullptr, 0));
		}

		return;
	}

	const Pipeline& o = m_pipelines[m_pipelineCurrent];     // Old pipeline
	
	if (n.inputLayout != o.inputLayout)
	{
		m_context->IASetInputLayout(n.inputLayout.Get());
	}

	if (n.primitiveTopology != o.primitiveTopology)
	{
		m_context->IASetPrimitiveTopology(n.primitiveTopology);
	}

	if (n.vertexShader != o.vertexShader)
	{
		m_context->VSSetShader(n.vertexShader.Get(), nullptr, 0);
	}

	if (n.geometryShader != o.geometryShader)
	{
		m_context->GSSetShader(n.geometryShader.Get(), nullptr, 0);
	}

	if (n.domainShader != o.domainShader)
	{
		m_context->DSSetShader(n.domainShader.Get(), nullptr, 0);
	}

	if (n.hullShader != o.hullShader)
	{
		m_context->HSSetShader(n.hullShader.Get(), nullptr, 0);
	}

	if (shadowing && !m_shadowPSBound)
	{
		m_context->PSSetShader(nullptr, nullptr, 0);
		m_shadowPSBound = true;
	}
	else if (!shadowing && (n.pixelShader != o.pixelShader || m_shadowPSBound))
	{
		m_context->PSSetShader(n.pixelShader.Get(), nullptr, 0);
		m_shadowPSBound = false;
	}

	m_pipelineCurrent = pipeline;
}

void RenderCore::InitPipelines()
{
	ComPtr<ID3DBlob> blob;

	// Standard pipeline (blinn-phong)

	m_pipelines[PipelineTypeStandard].primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"PSBlinnPhong.cso", &blob));
	EXC_COMCHECK(m_device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_pipelines[PipelineTypeStandard].pixelShader
	));

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"VSMesh.cso", &blob));
	EXC_COMCHECK(m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_pipelines[PipelineTypeStandard].vertexShader
	));

	D3D11_INPUT_ELEMENT_DESC inputLayoutStandard[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }  
	}; 

	EXC_COMCHECK(m_device->CreateInputLayout(
		inputLayoutStandard, 
		(uint)(sizeof(inputLayoutStandard) / sizeof(*inputLayoutStandard)),
		blob->GetBufferPointer(), 
		blob->GetBufferSize(),
		m_pipelines[PipelineTypeStandard].inputLayout.GetAddressOf()
	));



	// Standard pipeline rigged (blinn-phong)

	m_pipelines[PipelineTypeStandardRigged].primitiveTopology = m_pipelines[PipelineTypeStandard].primitiveTopology;
	m_pipelines[PipelineTypeStandardRigged].pixelShader = m_pipelines[PipelineTypeStandard].pixelShader;

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"VSMeshRigged.cso", &blob));
	EXC_COMCHECK(m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_pipelines[PipelineTypeStandardRigged].vertexShader
	));

	D3D11_INPUT_ELEMENT_DESC inputLayoutStandardRigged[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }/*,
		{ "BONESTWO", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTSTWO", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }*/
	};

	EXC_COMCHECK(m_device->CreateInputLayout(
		inputLayoutStandardRigged,
		(uint)(sizeof(inputLayoutStandardRigged) / sizeof(*inputLayoutStandardRigged)),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		m_pipelines[PipelineTypeStandardRigged].inputLayout.GetAddressOf()
	));


	// Shadow mapping pipeline

	m_pipelines[PipelineTypeShadow].pixelShader = nullptr;

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"VSShadow.cso", &blob));
	EXC_COMCHECK(m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_pipelines[PipelineTypeShadow].vertexShader
	));

	D3D11_INPUT_ELEMENT_DESC inputLayoutShadow[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	EXC_COMCHECK(m_device->CreateInputLayout(
		inputLayoutShadow,
		(uint)(sizeof(inputLayoutShadow) / sizeof(*inputLayoutShadow)),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		m_pipelines[PipelineTypeShadow].inputLayout.GetAddressOf()
	));

	blob->Release();



	// Standard pipeline instanced environment

	m_pipelines[PipelineTypeEnvironment].primitiveTopology = m_pipelines[PipelineTypeStandard].primitiveTopology;
	m_pipelines[PipelineTypeEnvironment].pixelShader = m_pipelines[PipelineTypeStandard].pixelShader;

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"VSMeshInstanced.cso", &blob));
	EXC_COMCHECK(m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_pipelines[PipelineTypeEnvironment].vertexShader
	));

	D3D11_INPUT_ELEMENT_DESC inputLayoutInstanced[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	EXC_COMCHECK(m_device->CreateInputLayout(
		inputLayoutInstanced,
		(uint)(sizeof(inputLayoutInstanced) / sizeof(*inputLayoutInstanced)),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		m_pipelines[PipelineTypeEnvironment].inputLayout.GetAddressOf()
	));

	// Full screen quad pipeline

	// Has to be triangle strip.
	m_pipelines[PipelineTypeFullScreenQuad].primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"PSFullScreen.cso", &blob));
	EXC_COMCHECK(m_device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_pipelines[PipelineTypeFullScreenQuad].pixelShader
	));

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"VSFullScreen.cso", &blob));
	EXC_COMCHECK(m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_pipelines[PipelineTypeFullScreenQuad].vertexShader
	));

	blob->Release();


}

void RenderCore::InitComputeShaders()
{
	ComPtr<ID3DBlob> blob;

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"CSThresholdPass.cso", &blob));
	EXC_COMCHECK(m_device->CreateComputeShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		m_thresholdCompute.GetAddressOf()
	));

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"CSBloomPass.cso", &blob));
	EXC_COMCHECK(m_device->CreateComputeShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		m_bloomCompute.GetAddressOf()
	));

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"CSColorGrade.cso", &blob));
	EXC_COMCHECK(m_device->CreateComputeShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		m_colorGradeCompute.GetAddressOf()
	));

	EXC_COMCHECK(D3DReadFileToBlob(DIR_SHADERS L"CSPositionalEffectPass.cso", &blob));
	EXC_COMCHECK(m_device->CreateComputeShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		m_positionalEffectCompute.GetAddressOf()
	));
}

void RenderCore::InitConstantBuffers()
{
	// View info

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = sizeof(CB::ViewInfo);
	desc.StructureByteStride = 0;
	desc.MiscFlags = 0;

	EXC_COMCHECK(m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.viewInfo.GetAddressOf()
	));

	EXC_COMINFO(m_context->VSSetConstantBuffers(RegCBVViewInfo, 1, m_constantBuffers.viewInfo.GetAddressOf()));
	EXC_COMINFO(m_context->PSSetConstantBuffers(RegCBVViewInfo, 1, m_constantBuffers.viewInfo.GetAddressOf()));



	// Object info

	desc.ByteWidth = sizeof(CB::ObjectInfo);

	EXC_COMCHECK(m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.objectInfo.GetAddressOf()
	));

	EXC_COMINFO(m_context->VSSetConstantBuffers(RegCBVObjectInfo, 1, m_constantBuffers.objectInfo.GetAddressOf()));


	// Player info
	desc.ByteWidth = sizeof( CB::ObjectInfo );

	EXC_COMCHECK( m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.playerInfo.GetAddressOf()
	) );

	EXC_COMINFO( m_context->VSSetConstantBuffers( RegCBVTesselationInfo, 1, m_constantBuffers.playerInfo.GetAddressOf()));


	// Shading info

	desc.ByteWidth = sizeof(CB::ShadingInfo);

	EXC_COMCHECK(m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.shadingInfo.GetAddressOf()
	));

	EXC_COMINFO(m_context->PSSetConstantBuffers(RegCBVShadingInfo, 1, m_constantBuffers.shadingInfo.GetAddressOf()));
	// TODO: Temporary bind to CS as positional compute shader needs camera info. 
	// This data should be bound with other more relevant data in its own buffer for use in the compute shader.
	EXC_COMINFO(m_context->CSSetConstantBuffers(RegCBVShadingInfo, 1, m_constantBuffers.shadingInfo.GetAddressOf()));



	// Material info

	desc.ByteWidth = sizeof(CB::MaterialInfo);

	EXC_COMCHECK(m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.materialInfo.GetAddressOf()
	));

	EXC_COMINFO(m_context->PSSetConstantBuffers(RegCBVMaterialInfo, 1, m_constantBuffers.materialInfo.GetAddressOf()));


	// Threshold info for bloom

	desc.ByteWidth = sizeof(CB::PPFXThresholdInfo);

	EXC_COMCHECK(m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.ppfxThresholdInfo.GetAddressOf()
	));

	EXC_COMINFO(m_context->CSSetConstantBuffers(RegCBVThresholdInfo, 1, m_constantBuffers.ppfxThresholdInfo.GetAddressOf()));


	// Color grade info

	desc.ByteWidth = sizeof(CB::PPFXColorGradeInfo);

	EXC_COMCHECK(m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.ppfxColorGradeInfo.GetAddressOf()
	));

	EXC_COMINFO(m_context->CSSetConstantBuffers(RegCBVColorgradeInfo, 1, m_constantBuffers.ppfxColorGradeInfo.GetAddressOf()));
	EXC_COMINFO(m_context->PSSetConstantBuffers(RegCBVColorgradeInfo, 1, m_constantBuffers.ppfxColorGradeInfo.GetAddressOf()));

	// Time switch info

	desc.ByteWidth = sizeof(CB::GameLogicInfo);

	EXC_COMCHECK(m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.gameLogicInfo.GetAddressOf()
	));

	EXC_COMINFO(m_context->CSSetConstantBuffers(RegCBVTimeSwitchInfo, 1, m_constantBuffers.gameLogicInfo.GetAddressOf()));
	EXC_COMINFO(m_context->PSSetConstantBuffers(RegCBVTimeSwitchInfo, 1, m_constantBuffers.gameLogicInfo.GetAddressOf()));

	desc.ByteWidth = sizeof(CB::EnemyConeInfo);

	EXC_COMCHECK(m_device->CreateBuffer(
		&desc,
		nullptr,
		m_constantBuffers.enemyConeInfo.GetAddressOf()
	));

	EXC_COMINFO(m_context->CSSetConstantBuffers(RegCBVEnemyConeInfo, 1, m_constantBuffers.enemyConeInfo.GetAddressOf()));
}

void RenderCore::InitLightBuffers()
{
	/*int totalSize = 0;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.StructureByteStride = sizeof(DirectionalLight::Data);
	bufferDesc.ByteWidth = LIGHT_CAPACITY_DIR * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	EXC_COMCHECK(m_device->CreateBuffer(
		&bufferDesc,
		nullptr,
		m_lightBufferDir.GetAddressOf()
	));

	totalSize += bufferDesc.ByteWidth;

	bufferDesc.StructureByteStride = sizeof(PointLight::Data);
	bufferDesc.ByteWidth = LIGHT_CAPACITY_POINT * bufferDesc.StructureByteStride;
	EXC_COMCHECK(m_device->CreateBuffer(
		&bufferDesc,
		nullptr,
		m_lightBufferPoint.GetAddressOf()
	));

	totalSize += bufferDesc.ByteWidth;

	bufferDesc.StructureByteStride = sizeof(SpotLight::Data);
	bufferDesc.ByteWidth = LIGHT_CAPACITY_SPOT * bufferDesc.StructureByteStride;
	EXC_COMCHECK(m_device->CreateBuffer(
		&bufferDesc,
		nullptr,
		m_lightBufferSpot.GetAddressOf()
	));

	totalSize += bufferDesc.ByteWidth;*/



	/*D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.StructureByteStride = sizeof(DirectionalLight::Data);
	bufferDesc.ByteWidth = LIGHT_CAPACITY_DIR * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;*/
}

void RenderCore::InitDefaultMaterials()
{
	cs::Color4 dData(255,	255,	255,	255);	// Diffuse, alpha
	cs::Color4 sData(255,	255,	255,	255);	// Specular, glossiness
	cs::Color4 eData(0,		0,		0,		255);	// Emissive, -
	cs::Color4 nData(127,	127,	255,	0);		// Normal, height/displacement

	D3D11_TEXTURE2D_DESC td;
	td.Width = 1;
	td.Height = 1;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc = { 1, 0 };
	td.Usage = D3D11_USAGE_IMMUTABLE;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA srd;
	srd.SysMemPitch = 4;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };



	// Create textures

	srd.pSysMem = &dData;
	EXC_COMCHECK(m_device->CreateTexture2D(&td, &srd, &m_defaultDiffuse));
	EXC_COMCHECK(m_device->CreateShaderResourceView(m_defaultDiffuse.Get(), &srvd, &m_defaultDiffuseSRV));

	srd.pSysMem = &sData;
	EXC_COMCHECK(m_device->CreateTexture2D(&td, &srd, &m_defaultSpecular));
	EXC_COMCHECK(m_device->CreateShaderResourceView(m_defaultSpecular.Get(), &srvd, &m_defaultSpecularSRV));

	srd.pSysMem = &eData;
	EXC_COMCHECK(m_device->CreateTexture2D(&td, &srd, &m_defaultEmissive));
	EXC_COMCHECK(m_device->CreateShaderResourceView(m_defaultEmissive.Get(), &srvd, &m_defaultEmissiveSRV));

	srd.pSysMem = &nData;
	EXC_COMCHECK(m_device->CreateTexture2D(&td, &srd, &m_defaultNormal));
	EXC_COMCHECK(m_device->CreateShaderResourceView(m_defaultNormal.Get(), &srvd, &m_defaultNormalSRV));



	// Default material

	m_defaultMaterial = MaterialResource {};
}

void RenderCore::WriteLights(cs::Color3f ambientColor, float ambientIntensity, const Camera& mainCamera,
	const shared_ptr<DirectionalLight>& lightDirectional,
	const cs::List<shared_ptr<PointLight>>& lightsPoint,
	const cs::List<shared_ptr<SpotLight>>& lightsSpot)
{
	CB::ShadingInfo si = 
	{
		{}, {}, {},

		(Vec3)ambientColor * ambientIntensity,
		0,
		mainCamera.GetPosition(),
		0
	};

	int dirCount = /*cs::imin(LIGHT_CAPACITY_DIR, lightsDirectional.Size())*/ 1;
	si.pointCount = cs::imin(LIGHT_CAPACITY_POINT, lightsPoint.Size());
	si.spotCount = cs::imin(LIGHT_CAPACITY_SPOT, lightsSpot.Size());

	si.directional = lightDirectional->bufferData;

	for (unsigned int i = 0; i < si.pointCount; i++)
	{
		si.points[i] = lightsPoint[i]->bufferData;
	}

	for (unsigned int i = 0; i < si.spotCount; i++)
	{
		si.spots[i] = lightsSpot[i]->bufferData;
	}



	// Mapping

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(m_constantBuffers.shadingInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, &si, sizeof(CB::ShadingInfo));
	EXC_COMINFO(m_context->Unmap(m_constantBuffers.shadingInfo.Get(), 0u));
}

void RenderCore::WritePPFXThresholdInfo(const float luminanceThreshold, const float strength, const float minLuminance)
{
	CB::PPFXThresholdInfo thresholdInfo = {
		luminanceThreshold,
		strength,
		minLuminance
	};

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(m_constantBuffers.ppfxThresholdInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, &thresholdInfo, sizeof(CB::PPFXThresholdInfo));
	EXC_COMINFO(m_context->Unmap(m_constantBuffers.ppfxThresholdInfo.Get(), 0u));
}

void RenderCore::WritePPFXColorgradeInfo(const Vec2 vignetteBorderAndStrength, const Vec2 contrastAmountAndMidpoint, const float brightness, const float saturation)
{
	CB::PPFXColorGradeInfo colorGradeInfo = {
		vignetteBorderAndStrength,
		contrastAmountAndMidpoint,
		brightness,
		saturation
	};

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(m_constantBuffers.ppfxColorGradeInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, &colorGradeInfo, sizeof(CB::PPFXColorGradeInfo));
	EXC_COMINFO(m_context->Unmap(m_constantBuffers.ppfxColorGradeInfo.Get(), 0u));
}

void RenderCore::WriteTimeSwitchInfo(float timeSinceSwitch, float chargeDuration, float falloffDuration, bool isInFuture, float globalDetectionLevel)
{
	CB::GameLogicInfo timeSwitchInfo = {
		timeSinceSwitch,
		chargeDuration,
		falloffDuration,
		(float)isInFuture, // Cast to float. Check struct def for why.
		globalDetectionLevel
	}; 

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(m_constantBuffers.gameLogicInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, &timeSwitchInfo, sizeof(CB::GameLogicInfo));
	EXC_COMINFO(m_context->Unmap(m_constantBuffers.gameLogicInfo.Get(), 0u));
}

void RenderCore::WriteEnemyConeInfo(const cs::List<shared_ptr<Enemy>>& enemies)
{
	CB::EnemyConeInfo enemyConeInfo = {};

	const uint enemyCount = cs::imin(ENEMY_CONE_INFO_CAPACITY, enemies.Size());

	if (enemyCount > 0u)
	{
		const float combinedViewAngleRad = enemies[0]->GetViewAngle() * cs::c_pi / 180.0f;
		enemyConeInfo.coneAngle = combinedViewAngleRad;
		enemyConeInfo.coneLength = enemies[0]->GetViewDistance();

		for (uint i = 0; i < enemyCount; i++)
		{
			const Enemy& enemy = *enemies[i];
			Vec4 worldPosAndDir = Vec4();
			Vec2 enemyDir = enemy.GetForwardVector();
			Vec3 enemyPos = enemy.transform.worldPosition;

			// Make sure that position values are in xz plane.
			enemyConeInfo.worldPosAndDir[i] = Vec4(enemyPos.x, enemyPos.z, enemyDir.x, enemyDir.y);
		}
	}

	enemyConeInfo.coneCount = enemyCount;

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECK(m_context->Map(m_constantBuffers.enemyConeInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
	memcpy(msr.pData, &enemyConeInfo, sizeof(CB::EnemyConeInfo));
	EXC_COMINFO(m_context->Unmap(m_constantBuffers.enemyConeInfo.Get(), 0u));
}

