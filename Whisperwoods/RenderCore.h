#pragma once

#include "Window.h"
#include "Camera.h"
#include "ConstantbufferData.h"
#include "Pipeline.h"
#include "Renderable.h"
#include "Font.h"
#include "SimpleMath.h"
#include <SpriteFont.h>
#include "Light.h"


class RenderCore
{
public:
	RenderCore(shared_ptr<Window> window);
	~RenderCore();

	void NewFrame();
	void TargetShadowMap(Light* light);
	void TargetBackBuffer();
	void EndFrame();

	HRESULT CreateVertexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const;
	HRESULT CreateIndexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const;
	HRESULT CreateImageTexture(char* image, UINT resHeight, UINT resWidth, UINT sysMemPitch, DXGI_FORMAT format, ID3D11Texture2D** out_texturePP);
	HRESULT CreateArmatureStructuredBuffer(ComPtr<ID3D11Buffer>& matrixBuffer, int numBones) const;
	HRESULT CreateArmatureSRV(ComPtr<ID3D11ShaderResourceView>& matrixSRV, ComPtr<ID3D11Buffer>& matrixBuffer, int numBones) const;
	void LoadImageTexture(const std::wstring& filePath, ComPtr<ID3D11Texture2D>& textureResource) const;

	void UpdateViewInfo(const Camera& camera);
	void UpdateObjectInfo(const WorldRenderable* worldRenderable);
	void DrawObject(const Renderable* renderable, bool shadowing);

	void SetVertexBuffer(ComPtr<ID3D11Buffer> buffer, uint stride, uint offset);
	void SetIndexBuffer(ComPtr<ID3D11Buffer> buffer, uint offset, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
	void DrawIndexed(uint indexCount, uint start, uint base);

	//void SetArmatureStructuredBuffer(ComPtr<ID3D11Buffer> matrixBuffer);
	void SetArmatureArmatureSRV(ComPtr<ID3D11ShaderResourceView> matrixSRV);

	void WriteLights(cs::Color3f ambientColor, float ambientIntensity, const Camera& mainCamera,
		const shared_ptr<DirectionalLight>& lightDirectional,
		const cs::List<shared_ptr<PointLight>>& lightsPoint,
		const cs::List<shared_ptr<SpotLight>>& lightsSpot);

	void DrawText(dx::SimpleMath::Vector2 fontPos, const wchar_t* m_text, Font font, cs::Color4f color, Vec2 origin);

	void InitImGui() const;

	void InitFont(std::unique_ptr<dx::SpriteFont> fonts[FontCount], std::unique_ptr<dx::SpriteBatch> * batch) const;

private:
	void BindPipeline(PipelineType pipeline, bool shadowing);

	void InitPipelines();
	void InitConstantBuffers();
	void InitLightBuffers();

private:
	shared_ptr<Window> m_window;

	// Core
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;

	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ComPtr<ID3D11BlendState> m_blendState;

	// Back buffer
	ComPtr<ID3D11Texture2D> m_bbTexture;
	ComPtr<ID3D11RenderTargetView> m_bbRTV;
	//ComPtr<ID3D11ShaderResourceView> m_bbSRV;
	cs::Color4f m_bbClearColor;

	// Depth stencil
	ComPtr<ID3D11Texture2D> m_dsTexture;
	ComPtr<ID3D11DepthStencilState> m_dsDSS;
	ComPtr<ID3D11DepthStencilView> m_dsDSV;
	ComPtr<ID3D11ShaderResourceView> m_dsSRV;

	D3D11_VIEWPORT m_viewport;

	// Pipelines
	Pipeline m_pipelines[PipelineTypeCount];
	int m_pipelineCurrent;
	bool m_shadowPSBound;

	// Constant buffers
	ConstantBuffers m_constantBuffers;

	// Light structured buffers
	ComPtr<ID3D11Buffer> m_lightBufferPoint;
	ComPtr<ID3D11Buffer> m_lightBufferSpot;
	ComPtr<ID3D11Buffer> m_lightBufferDir;
	ComPtr<ID3D11Buffer> m_lightBufferStaging;

	std::unique_ptr<dx::SpriteFont> m_fonts[FontCount];
	std::unique_ptr<dx::SpriteBatch> m_spriteBatch;
};