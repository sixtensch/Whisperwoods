#pragma once

#include "RenderCore.h"
#include "Window.h"
#include "Camera.h"
#include "ConstantbufferData.h"
#include "ModelResource.h"
#include "MeshRenderable.h"
#include "TextRenderable.h"
#include "Light.h"
#include "Font.h"

class RenderHandler sealed
{
public:
	RenderHandler();
	~RenderHandler();

	void InitCore(shared_ptr<Window> window);

	void Draw();
	void Present();

	const RenderCore* GetCore() const;

	Camera& GetCamera();


	shared_ptr<MeshRenderableStatic> CreateMeshStatic(const string& subpath);
	shared_ptr<TextRenderable> CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin);


	shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);

private:
	unique_ptr<RenderCore> m_renderCore;

	uint m_renderableIDCounter;
	cs::List<shared_ptr<WorldRenderable>> m_worldRenderables;
	cs::List<shared_ptr<TextRenderable>> m_texts;

	cs::Color3f m_lightAmbient;
	float m_lightAmbientIntensity;
	DirectionalLight m_lightDirectional;
	cs::List<PointLight> m_lightsPoint;
	cs::List<SpotLight> m_lightsSpot;

	Camera m_mainCamera;


};

