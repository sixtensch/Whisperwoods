#pragma once

#include "RenderHandler.h"
#include "RenderCore.h"
#include "Window.h"
#include "MeshRenderable.h"
#include "TextRenderable.h"
#include "GUIRenderable.h"
#include "LevelResource.h"
#include "Level.h"
class DebugGUI;

class Renderer sealed
{
public:
	Renderer(HINSTANCE instance);
	~Renderer();

	void Init(uint width, uint height);

	bool UpdateWindow();
	void Draw();
	void BeginGui();
	void EndGui();
	void UpdateGPUProfiler();
	void Present();

	void SetupEnvironmentAssets();

	static void SetPlayerMatrix(const Mat4& matrix);

	static void LoadLevel(LevelResource* level, string image);

	static void LoadEnvironment(const Level* level);

	static void UnLoadEnvironment();

	static void UpdateBitMapBind( const TextureResource* bitmap );

	static void ClearShadowRenderables();
	static void RegisterShadowRenderable();
	static void ExecuteShadowRender();

	static shared_ptr<MeshRenderableStatic> CreateMeshStatic(const string& subpath);
	static shared_ptr<MeshRenderableTerrain> CreateMeshTerrain( const string& subpath );
	static shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);
	static void DestroyMeshStatic(shared_ptr<MeshRenderableStatic> renderable);
	static shared_ptr<GUIRenderable> CreateGUIRenderable(const string& subpath);

	static shared_ptr<TextRenderable> CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin);

	static void UpdatePPFXInfo(Vec2 vignette, Vec2 contrast, float brightness, float saturation);
	static void SetAmbientLight(cs::Color3f color, float intensity);
	static shared_ptr<DirectionalLight> GetDirectionalLight();
	static bool RegisterLight(shared_ptr<PointLight> pointLight);
	static bool RegisterLight(shared_ptr<SpotLight> spotLight);

	static void SetFogParameters(Vec3 focus, float radius);

	static void UpdateTexture2DData(ComPtr<ID3D11Texture2D> texture2D, uint8_t* data, uint textureWidth, uint textureHeight);

	static Camera& GetCamera();
	static Window& GetWindow();

	// Added back because GetRenderCore was needed in minimap creation and is the ONLY function that is not static.
	static Renderer& Get();

	// TODO: Check if there is a better way of gaining access to the render core.
	RenderCore* GetRenderCore();

	static void SetTimelineState(bool future);
private:
	static Renderer* s_singleton;

	HINSTANCE m_instance;

	shared_ptr<Window> m_window; 
	unique_ptr<RenderHandler> m_renderHandler;
	unique_ptr<DebugGUI> m_gui;

};

