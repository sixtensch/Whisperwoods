#pragma once

#include "RenderCore.h"
#include "Window.h"
#include "Camera.h"
#include "ConstantbufferData.h"
#include "ModelResource.h"
#include "MeshRenderable.h"
#include "Light.h"

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

private:
	unique_ptr<RenderCore> m_renderCore;

	uint m_renderableIDCounter;
	cs::List<shared_ptr<WorldRenderable>> m_worldRenderables;

	cs::Color3f m_lightAmbient;
	float m_lightAmbientIntensity;
	DirectionalLight m_lightDirectional;
	cs::List<PointLight> m_lightsPoint;
	cs::List<SpotLight> m_lightsSpot;

	Camera m_mainCamera;
};

