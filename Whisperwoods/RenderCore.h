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
#include "MaterialResource.h"
#include "Bone.h"
#include "Enemy.h"
#include "GPUProfiler.h"
//#include "GUIElement.h"
struct GUIElement;

class RenderCore
{
public:
	RenderCore(shared_ptr<Window> window);
	~RenderCore();

	void NewFrame();
	void TargetPrepass();
	void TargetRenderTexture();
	void UnbindRenderTexture();
	void TargetShadowMap();

	void TargetStaticShadowMap();
	void TargetStaticShadowMapFuture();

	void BindStaticShadowMap(bool future);

	//void TargetBackBuffer(); // Use target render texture if you want to render anything to the scene.
	void EndFrame();

	void CreateVertexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const;
	void CreateInstanceBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const;
	void CreateIndexBuffer(const void* data, UINT byteWidth, ID3D11Buffer** out_bufferPP) const;
	void CreateImageTextureUnorm(char* image, UINT resHeight, UINT resWidth, UINT sysMemPitch, DXGI_FORMAT format, ID3D11Texture2D** out_texturePP);
	void CreateArmatureStructuredBuffer(ComPtr<ID3D11Buffer>& matrixBuffer, int numBones) const;
	void CreateArmatureSRV(ComPtr<ID3D11ShaderResourceView>& matrixSRV, ComPtr<ID3D11Buffer>& matrixBuffer, int numBones) const;
	void LoadImageTexture(const std::wstring& filePath, ComPtr<ID3D11Texture2D>& textureResource, ComPtr<ID3D11ShaderResourceView>& srv) const;
	void CreateImageTextureUnorm(uint8_t* data, uint width, uint height, ComPtr<ID3D11Texture2D>& textureResource, ComPtr<ID3D11ShaderResourceView>& srv, bool isCPUWritable = false) const;

	// This calls new for the out data
	void DumpTexture(ID3D11Texture2D* texture, uint* outWidth, uint* outHeight, cs::Color4** newOutData) const;

	void UpdateViewInfo(const Camera& camera);
	void UpdateObjectInfo(const WorldRenderable* worldRenderable);

	void UpdatePlayerInfo( Mat4 matrix, Vec4 worldInfo1, Vec4 worldInfo2 );

	void UpdateGUIInfo(const GUIElement* guiElement) const;

	void UpdateBitmapInfo(const TextureResource* bitmap) const;
	// Not really unorm specifically but assumed to be 4 bytes.
	void UpdateTexture2DUnormData(ComPtr<ID3D11Texture2D> texture2D, uint8_t* data, uint textureWidth, uint textureHeight);

	void UpdateMaterialInfo(const MaterialResource* material) const;
	void UpdateInstanceBuffer(ComPtr<ID3D11Buffer> iBuffer, const Mat4* data, uint count);

	void SetVertexBuffer(ComPtr<ID3D11Buffer> buffer, uint stride, uint offset);
	void SetInstanceBuffers(ComPtr<ID3D11Buffer> vBuffer, ComPtr<ID3D11Buffer> iBuffer, uint vStride, uint iStride, uint vOffset, uint iOffset);
	void SetIndexBuffer(ComPtr<ID3D11Buffer> buffer, uint offset, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);

	void BindInstancedPipeline(bool shadowed, bool discardPipeline);

	void DrawObject(const Renderable* renderable, bool shadowing, bool discardPipeline);
	void DrawIndexed(uint indexCount, uint indexStart, uint vertexBase);
	void DrawInstanced(uint indexCount, uint instanceCount, uint startIndex, uint baseVertex, uint startInstance);

	//void SetArmatureStructuredBuffer(ComPtr<ID3D11Buffer> matrixBuffer);
	void SetArmatureArmatureSRV(ComPtr<ID3D11ShaderResourceView> matrixSRV);
	void UpdateBoneMatrixBuffer(ComPtr<ID3D11Buffer> matrixBuffer, cs::List<DirectX::XMFLOAT4X4> bones);


	void WriteLights(cs::Color3f ambientColor, float ambientIntensity, const Camera& mainCamera,
		const shared_ptr<DirectionalLight>& lightDirectional,
		const cs::List<shared_ptr<PointLight>>& lightsPoint,
		const cs::List<shared_ptr<SpotLight>>& lightsSpot,
		Vec3 fogFocus, float fogRadius);

	void WritePPFXThresholdInfo(
		const float luminanceThreshold, 
		const float strength, 
		const float minLuminance
	);
	
	void WritePPFXColorgradeInfo(
		const Vec2 vignetteBorderAndStrength, 
		const Vec2 contrastAmountAndMidpoint, 
		const float brightness, 
		const float saturation
	);

	void WriteTimeSwitchInfo(
		float timeSinceSwitch,
		float chargeDuration,
		float falloffDuration,
		bool isInFuture,
		float globalDetectionLevel
	);

	void WriteEnemyConeInfo(const cs::List<shared_ptr<Enemy>>& enemies);

	void DrawText(dx::SimpleMath::Vector2 fontPos, const wchar_t* m_text, Font font, cs::Color4f color, Vec2 origin);

	void DrawPPFX();
	void DrawPositionalEffects();
	void DrawToBackBuffer();

	void InitImGui() const;

	void InitFont(std::unique_ptr<dx::SpriteFont> fonts[FontCount], std::unique_ptr<dx::SpriteBatch>* batch) const;

	void ProfileBegin(const std::string& profileName);
	void ProfileEnd(const std::string& profileName);
	void UpdateGPUProfiler();

	void SetFuture(bool future);
	bool IsInFuture() const;

	bool m_bindShadowPS;

private:
	void BindPipeline(PipelineType pipeline, bool shadowing, bool discardPipeline);

	void InitPipelines();
	void InitComputeShaders();
	void InitConstantBuffers();
	void InitLightBuffers();
	void InitDefaultMaterials();

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
	ComPtr<ID3D11UnorderedAccessView> m_bbUAV;
	ComPtr<ID3D11ShaderResourceView> m_bbSRV;
	cs::Color4f m_bbClearColor;

	// Render texture
	ComPtr<ID3D11Texture2D> m_renderTexture;
	ComPtr<ID3D11RenderTargetView> m_renderTextureRTV;
	ComPtr<ID3D11ShaderResourceView> m_renderTextureSRV;
	ComPtr<ID3D11UnorderedAccessView> m_renderTextureUAV;

	// Render texture copy
	ComPtr<ID3D11Texture2D> m_renderTextureCopy;
	ComPtr<ID3D11ShaderResourceView> m_renderTextureCopySRV;

	// Position texture
	ComPtr<ID3D11Texture2D> m_positionTexture;
	ComPtr<ID3D11RenderTargetView> m_positionTextureRTV;
	ComPtr<ID3D11ShaderResourceView> m_positionTextureSRV;

	

	// PPFX
	ComPtr<ID3D11Texture2D> m_ppfxLumTexture;
	
	ComPtr<ID3D11UnorderedAccessView> m_ppfxLumUAV;
	ComPtr<ID3D11RenderTargetView> m_ppfxLumRTV; // This is only created for being able to call CreateBitmaps() function. No other function.
	ComPtr<ID3D11ShaderResourceView> m_ppfxLumSRV;
	
	ComPtr<ID3D11Texture2D> m_ppfxLumSumTexture;
	ComPtr<ID3D11ShaderResourceView> m_ppfxLumSumSRV;
	ComPtr<ID3D11UnorderedAccessView> m_ppfxLumSumUAV;

	ComPtr<ID3D11ComputeShader> m_thresholdCompute;
	ComPtr<ID3D11ComputeShader> m_bloomCompute;
	ComPtr<ID3D11ComputeShader> m_colorGradeCompute;
	ComPtr<ID3D11ComputeShader> m_positionalEffectCompute;

	ComPtr<ID3D11SamplerState> m_bloomUpscaleSampler;

	// Depth stencil
	ComPtr<ID3D11Texture2D> m_dsTexture;
	ComPtr<ID3D11DepthStencilState> m_dsDSS;
	ComPtr<ID3D11DepthStencilState> m_ppDSS;
	ComPtr<ID3D11DepthStencilView> m_dsDSV;
	ComPtr<ID3D11ShaderResourceView> m_dsSRV;

	D3D11_VIEWPORT m_viewport;

	MaterialResource m_defaultMaterial;

	ComPtr<ID3D11Texture2D> m_defaultDiffuse;
	ComPtr<ID3D11Texture2D> m_defaultSpecular;
	ComPtr<ID3D11Texture2D> m_defaultEmissive;
	ComPtr<ID3D11Texture2D> m_defaultNormal;

	ComPtr<ID3D11ShaderResourceView> m_defaultDiffuseSRV;
	ComPtr<ID3D11ShaderResourceView> m_defaultSpecularSRV;
	ComPtr<ID3D11ShaderResourceView> m_defaultEmissiveSRV;
	ComPtr<ID3D11ShaderResourceView> m_defaultNormalSRV;


	ComPtr<ID3D11SamplerState> m_sampler;
	ComPtr<ID3D11SamplerState> m_samplerNoWrap;
	ComPtr<ID3D11SamplerState> m_pointSampler;

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


	// Shadow Resources Present
	ComPtr<ID3D11Texture2D> m_shadowStaticTexture;
	ComPtr<ID3D11DepthStencilView> m_shadowStaticDSV;
	ComPtr<ID3D11ShaderResourceView> m_shadowStaticSRV;
	// Shadow Resources Future
	ComPtr<ID3D11Texture2D> m_shadowFutureTexture;
	ComPtr<ID3D11DepthStencilView> m_shadowFutureDSV;
	ComPtr<ID3D11ShaderResourceView> m_shadowFutureSRV;


	ComPtr<ID3D11Texture2D> m_shadowTexture;
	ComPtr<ID3D11DepthStencilView> m_shadowDSV;
	ComPtr<ID3D11ShaderResourceView> m_shadowSRV;
	ComPtr<ID3D11RasterizerState> m_shadowRenderState;
	ComPtr<ID3D11SamplerState> m_shadowSampler;
	D3D11_VIEWPORT m_shadowViewport;

	std::unique_ptr<dx::SpriteFont> m_fonts[FontCount];
	std::unique_ptr<dx::SpriteBatch> m_spriteBatch;

	GPUProfiler m_gpuProfiler;

	bool m_drawFuture;
};