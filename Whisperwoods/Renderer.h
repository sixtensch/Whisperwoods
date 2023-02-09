#pragma once

#include "RenderHandler.h"
#include "RenderCore.h"
#include "Window.h"
#include "MeshRenderable.h"
#include "TextRenderable.h"
class GUI;

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
	void Present();

	static shared_ptr<MeshRenderableStatic> CreateMeshStatic(const string& subpath);
	static shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);

	static shared_ptr<TextRenderable> CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin);

	static shared_ptr<DirectionalLight> GetDirectionalLight();
	static bool RegisterLight(shared_ptr<PointLight> pointLight);
	static bool RegisterLight(shared_ptr<SpotLight> spotLight);

	static Camera& GetCamera();
	static Window& GetWindow();
	//static Renderer& Get();

	// TODO: Check if there is a better way of gaining access to the render core.
	const RenderCore* GetRenderCore();

private:
	static Renderer* s_singleton;

	HINSTANCE m_instance;

	shared_ptr<Window> m_window;
	unique_ptr<RenderHandler> m_renderHandler;
	unique_ptr<GUI> m_gui;

};

