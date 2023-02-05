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




	// ######################### DUD ########################################
	struct Vertex {
		float position[3];
		float normal[3];
		float uv[2];
	};
	ComPtr<ID3D11Buffer> VertexBuffer = {};
	ComPtr<ID3D11Buffer> IndexBuffer = {};

	std::vector<Vertex> vData = {
		{{-0.5f, 0.5f, 0.5f},{0,0,-1.0f},{0,0}},
		{{0.5f, -0.5f, 0.5f},{0,0,-1.0f},{0,0}},
		{{-0.5f, -0.5f, 0.5f},{0,0,-1.0f},{0,0}}
	};
	std::vector<int> iData = {
		0,1,2
	};

	/// Vertex Buffer
	D3D11_BUFFER_DESC vertexDesc = {};
	vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexDesc.ByteWidth = sizeof(Vertex) * vData.size();
	vertexDesc.StructureByteStride = 0;
	vertexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vsubData = {};
	vsubData.pSysMem = vData.data();

	m_renderCore.get()->GetDeviceP()->CreateBuffer(
		&vertexDesc,
		&vsubData,
		VertexBuffer.GetAddressOf()
	);

	/// Index Buffer
	D3D11_BUFFER_DESC indexDesc = {};
	indexDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexDesc.ByteWidth = sizeof(int) * iData.size();
	indexDesc.StructureByteStride = 0;
	indexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA isubData = {};
	isubData.pSysMem = iData.data();

	m_renderCore.get()->GetDeviceP()->CreateBuffer(
		&indexDesc,
		&isubData,
		IndexBuffer.GetAddressOf()
	);


	m_renderCore->BindGPipeline(VertexBuffer.GetAddressOf(), IndexBuffer.Get(), sizeof(Vertex), 0, PIPELINE_TYPE::BLINN_PHONG);
	
	m_renderCore->DrawIndexed(iData.size(), 0, 0);
	// ######################### DUD ########################################



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
