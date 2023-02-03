#include "core.h"
#include "RenderHandler.h"

RenderHandler::RenderHandler()
{}

RenderHandler::~RenderHandler()
{}

void RenderHandler::InitCore(shared_ptr<Window> window)
{
	mainCamera.SetValues( 90 * dx::XM_PI/180, window->GetAspectRatio(),0.01f, 100.0f );
	mainCamera.CalculatePerspectiveProjection();
	mainCamera.Update();

	m_renderCore = make_unique<RenderCore>(window, mainCamera);

    InitVSConstantBuffers();
}

void RenderHandler::Draw()
{
	m_renderCore->NewFrame(cbufferData);
	m_renderCore->EndFrame();
}


void RenderHandler::InitVSConstantBuffers()
{
    D3D11_BUFFER_DESC Desc = {};
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.ByteWidth = sizeof(CB::VSData);
    Desc.StructureByteStride = 0;
    Desc.MiscFlags = 0;


    // Create the initial data for the cbuffer
    CB::VSData data = {
        mainCamera.GetWorldMatrix(),
        mainCamera.GetViewMatrix(),
        mainCamera.GetProjectionMatrix()
    };


    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = &data;
    subData.SysMemPitch = 0;
    subData.SysMemSlicePitch = 0;

    EXC_COMCHECK(m_renderCore.get()->GetDeviceP()->CreateBuffer(
        &Desc,
        &subData,
        cbufferData.vertexShaderCBuffer.GetAddressOf()
    ));
}
