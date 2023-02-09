#include "Core.h"

#include "RenderCore.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <d3dcompiler.h>

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

    InitPipelines();
    InitConstantBuffers();

    m_pipelineCurrent = -1;
}

RenderCore::~RenderCore()
{
}

void RenderCore::NewFrame()
{
    EXC_COMINFO(m_context->ClearDepthStencilView(m_dsDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));

    EXC_COMINFO(m_context->ClearRenderTargetView(m_bbRTV.Get(), (float*)&m_bbClearColor));
    EXC_COMINFO(m_context->OMSetRenderTargets(1u, m_bbRTV.GetAddressOf(), m_dsDSV.Get()));
}

void RenderCore::TargetShadowMap(Light* light)
{

}

void RenderCore::TargetBackBuffer()
{
    EXC_COMINFO(m_context->ClearRenderTargetView(m_bbRTV.Get(), (float*)&m_bbClearColor));
    EXC_COMINFO(m_context->OMSetRenderTargets(1u, m_bbRTV.GetAddressOf(), m_dsDSV.Get()));
}

void RenderCore::EndFrame()
{
    EXC_COMCHECK(m_swapChain->Present(0u, 0u));
}

HRESULT RenderCore::CreateVertexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP)
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

    hr = m_device->CreateBuffer(&bufferDesc, &subData, out_bufferPP);
    if ( FAILED(hr) )
    {
        LOG_ERROR("failed to create static model vertexBuffer");
    }
    
    return hr;
}

HRESULT RenderCore::CreateIndexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP)
{
    HRESULT hr = {};
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = byteWidth;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    //bufferDesc.StructureByteStride 

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = data;
    subData.SysMemPitch = 0;
    subData.SysMemSlicePitch = 0;

    hr = m_device->CreateBuffer(&bufferDesc, &subData, out_bufferPP);
    if ( FAILED(hr) )
    {
        LOG_ERROR("failed to create model indexBuffer");
    }
    return hr;
}

HRESULT RenderCore::CreateImageTexture(char* image, UINT resHeight, UINT resWidth, UINT sysMemPitch, DXGI_FORMAT format, ID3D11Texture2D** out_texturePP)
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

    hr = m_device->CreateTexture2D(
        &textureDesc,
        &subData,
        out_texturePP
    );
    if ( FAILED(hr) )
        LOG_ERROR("Failed to create texture2d");
    return hr;
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

void RenderCore::SetIndexBuffer(ComPtr<ID3D11Buffer> buffer, uint offset, DXGI_FORMAT format)
{
    EXC_COMINFO(m_context->IASetIndexBuffer(buffer.Get(), format, offset));
}

void RenderCore::DrawIndexed(uint indexCount, uint start, uint base)
{
    EXC_COMINFO(m_context->DrawIndexed(indexCount, start, base));
}

void RenderCore::InitImGui() const
{
    ImGui_ImplWin32_Init(m_window->Data());
    ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
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
        EXC_COMINFO(m_context->PSSetShader(n.pixelShader.Get(), nullptr, 0));

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
        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    EXC_COMCHECK(m_device->CreateInputLayout(
        inputLayoutStandardRigged,
        (uint)(sizeof(inputLayoutStandardRigged) / sizeof(*inputLayoutStandardRigged)),
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        m_pipelines[PipelineTypeStandardRigged].inputLayout.GetAddressOf()
    ));

    blob->Release();
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



    // Shading info

    desc.ByteWidth = sizeof(CB::ShadingInfo);

    EXC_COMCHECK(m_device->CreateBuffer(
        &desc,
        nullptr,
        m_constantBuffers.shadingInfo.GetAddressOf()
    ));

    EXC_COMINFO(m_context->PSSetConstantBuffers(RegCBVShadingInfo, 1, m_constantBuffers.shadingInfo.GetAddressOf()));
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

void RenderCore::WriteLights(cs::Color3f ambientColor, float ambientIntensity, const Camera& mainCamera,
    const DirectionalLight& lightDirectional,
    const cs::List<PointLight>& lightsPoint,
    const cs::List<SpotLight>& lightsSpot)
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

    si.directional = lightDirectional.bufferData;

    for (int i = 0; i < si.pointCount; i++)
    {
        si.points[i] = lightsPoint[i].bufferData;
    }

    for (int i = 0; i < si.spotCount; i++)
    {
        si.spots[i] = lightsSpot[i].bufferData;
    }



    // Mapping

    D3D11_MAPPED_SUBRESOURCE msr = {};
    EXC_COMCHECK(m_context->Map(m_constantBuffers.shadingInfo.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
    memcpy(msr.pData, &si, sizeof(CB::ShadingInfo));
    EXC_COMINFO(m_context->Unmap(m_constantBuffers.shadingInfo.Get(), 0u));
}
