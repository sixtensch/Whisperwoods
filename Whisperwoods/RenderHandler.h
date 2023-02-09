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
	shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);

	shared_ptr<DirectionalLight> GetDirectionalLight();
	bool RegisterPointLight(shared_ptr<PointLight> pointLight);
	bool RegisterSpotLight(shared_ptr<SpotLight> spotLight);

private:
	unique_ptr<RenderCore> m_renderCore;

	uint m_renderableIDCounter;
	cs::List<shared_ptr<WorldRenderable>> m_worldRenderables;

	cs::Color3f m_lightAmbient;
	float m_lightAmbientIntensity;
	shared_ptr<DirectionalLight> m_lightDirectional;
	cs::List<shared_ptr<PointLight>> m_lightsPoint;
	cs::List<shared_ptr<SpotLight>> m_lightsSpot;

	Camera m_mainCamera;
};

