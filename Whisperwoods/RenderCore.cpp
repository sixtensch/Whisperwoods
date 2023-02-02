#include "Core.h"
#include "RenderCore.h"

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

    //setup viewport
    m_viewPort.TopLeftX = 0;
    m_viewPort.TopLeftY = 0;
    m_viewPort.MinDepth = 0;
    m_viewPort.MaxDepth = 1;
    m_viewPort.Width = static_cast<float>(window->GetWidth());
    m_viewPort.Height = static_cast<float>(window->GetHeight());
}

RenderCore::~RenderCore()
{
}
