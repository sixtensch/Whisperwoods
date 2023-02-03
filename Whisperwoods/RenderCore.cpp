#include "Core.h"
#include "RenderCore.h"
#include "ConstantbufferData.h"
#include <d3dcompiler.h>

RenderCore::RenderCore(shared_ptr<Window> window, const Camera& camera)
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
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS; //use resource or surface as result of rendering
    desc.BufferCount = 2; 
    desc.OutputWindow = window->Data();
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //deletes contents of backup buffer when called on
    desc.Flags = 0;

    UINT flags = 0;

#ifdef WW_DEBUG
    flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

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

    EXC_COMINFO(m_context->RSSetViewports(1u, &m_viewport));



    // Setup back buffer

    EXC_COMCHECK(m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&m_bbTexture));

    D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvd.Texture2D = { 0 };

    EXC_COMCHECK(m_device->CreateRenderTargetView(m_bbTexture.Get(), &rtvd, &m_bbRTV));

    //D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    //srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //srvd.Texture2D = { 0, 1 };
    //
    //EXC_COMCHECK(m_device->CreateShaderResourceView(m_bbTexture.Get(), &srvd, &m_bbSRV));

    m_bbClearColor = cs::Color4f(0.1f, 0.1f, 0.2f, 1.0f);



    // Depth stencil

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

    ComPtr<ID3D11RasterizerState> rss;

    EXC_COMCHECK(m_device->CreateRasterizerState(&rsd, &rss));
    EXC_COMINFO(m_context->RSSetState(rss.Get()));



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

    ComPtr<ID3D11BlendState> bss;

    EXC_COMCHECK(m_device->CreateBlendState(&bd, &bss));
    EXC_COMINFO(m_context->OMSetBlendState(bss.Get(), nullptr, sampleMask));

    CompileShaders();
    CreateVSConstantBuffers(camera);
}
RenderCore::~RenderCore()
{
}

void RenderCore::NewFrame()
{
    m_context->ClearDepthStencilView(m_dsDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

    EXC_COMINFO(m_context->ClearRenderTargetView(m_bbRTV.Get(), (float*)&m_bbClearColor));
    m_context->OMSetRenderTargets(1u, m_bbRTV.GetAddressOf(), m_dsDSV.Get());

    //m_context->DrawIndexed();
}

void RenderCore::EndFrame()
{
    EXC_COMCHECK(m_swapChain->Present(0u, 0u));
}

void RenderCore::BindGPipeline(ID3D11Buffer* const* vertexBufferPP, ID3D11Buffer* indexBufferP, const UINT& stride, const UINT& offset, PIPELINE_TYPE flag)
{
    // General binds all pipelines will use
    
    // Input Assembly
    m_context->IASetVertexBuffers(0, 1, vertexBufferPP, &stride, &offset);
    m_context->IASetIndexBuffer(indexBufferP, DXGI_FORMAT_R32_UINT, 0);

    // Vertex Shader
    m_context->VSSetConstantBuffers(0, 1, m_vertexShaderCBuffer.GetAddressOf());


    // Specialised binds
    switch (flag)
    {
        case BLINN_PHONG:
            BindBlinnPhong();
            break;


        default:
            LOG_CRITICAL("Incorrect flag, could not bind any new pipeline");
            break;
    }
}
void RenderCore::BindBlinnPhong()
{
    // Input Assembly
    m_context->IASetInputLayout(m_shaders.inputLayout.Get());
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Vertex Shader
    m_context->VSSetShader(m_shaders.vertexShader.Get(), nullptr, 0);

    // Pixel Shader
    m_context->PSSetShader(m_shaders.pixelShader.Get(), nullptr, 0);
}



void RenderCore::CompileShaders()
{
    ID3DBlob* shaderBlob = {};

#if WW_DEBUG
    wchar_t vsPath[] = L"../Bin/Whisperwoods64-Debug/MeshVS.cso";
    wchar_t pxPath[] = L"../Bin/Whisperwoods64-Debug/BlinnPhong.cso";
#endif
    
    EXC_COMCHECK(D3DReadFileToBlob(pxPath, &shaderBlob));
    EXC_COMCHECK(m_device->CreatePixelShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        m_shaders.pixelShader.GetAddressOf()
    ));

    EXC_COMCHECK(D3DReadFileToBlob(vsPath, &shaderBlob));
    EXC_COMCHECK(m_device->CreateVertexShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        m_shaders.vertexShader.GetAddressOf()
    ));

    // Input Layout
    D3D11_INPUT_ELEMENT_DESC vShaderInput[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    EXC_COMCHECK(m_device->CreateInputLayout(
        vShaderInput, static_cast<UINT>(sizeof(vShaderInput) / sizeof(*vShaderInput)),
        shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
        m_shaders.inputLayout.GetAddressOf()
    ));
}
HRESULT RenderCore::CreateVSConstantBuffers(const Camera& camera)
{
    HRESULT hr = {};
    D3D11_BUFFER_DESC Desc   = {};
    Desc.Usage               = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    Desc.ByteWidth           = sizeof( CB::VSData );
    Desc.StructureByteStride = 0;
    Desc.MiscFlags           = 0;


    // Create the initial data for the cbuffer
    CB::VSData data = {
        // Identity
        // View
        camera.GetProjectionMatrix()
    };


    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem          = &data;
    subData.SysMemPitch      = 0;
    subData.SysMemSlicePitch = 0;

    hr = m_device->CreateBuffer(
        &Desc,
        &subData,
        m_vertexShaderCBuffer.GetAddressOf()
    );
    return hr;
}
