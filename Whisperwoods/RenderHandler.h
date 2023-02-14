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

	void ExecuteDraw(const Camera& povCamera, bool shadows);

	const RenderCore* GetCore() const;

	Camera& GetCamera();


	shared_ptr<MeshRenderableStatic> CreateMeshStatic(const string& subpath);
	shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);
	shared_ptr<TextRenderable> CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin);



	shared_ptr<DirectionalLight> GetDirectionalLight();
	bool RegisterPointLight(shared_ptr<PointLight> pointLight);
	bool RegisterSpotLight(shared_ptr<SpotLight> spotLight);

private:
	unique_ptr<RenderCore> m_renderCore;

	uint m_renderableIDCounter;
	cs::List<shared_ptr<WorldRenderable>> m_worldRenderables;
	cs::List<shared_ptr<TextRenderable>> m_texts;

	cs::Color3f m_lightAmbient;
	float m_lightAmbientIntensity;
	shared_ptr<DirectionalLight> m_lightDirectional;
	cs::List<shared_ptr<PointLight>> m_lightsPoint;
	cs::List<shared_ptr<SpotLight>> m_lightsSpot;
	
	Camera m_mainCamera;


};

