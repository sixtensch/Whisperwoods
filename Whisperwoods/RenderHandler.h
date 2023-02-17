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
private:
	struct RenderableState
	{
		const shared_ptr<WorldRenderable> Current;
		const shared_ptr<WorldRenderable> Future;
		shared_ptr<WorldRenderable> operator[](int input)
		{
			return input ? Future : Current;
		}
		RenderableState& operator=(RenderableState& other) noexcept
		{
			return *this;
		}
	};
	enum TimelineState // Readability only
	{
		TimelineStateCurrent = false,
		TimelineStateFuture = true
	};
public:
	RenderHandler();
	~RenderHandler();

	void InitCore(shared_ptr<Window> window);

	void Draw();
	void Present();

	void ExecuteDraw(const Camera& povCamera, TimelineState state, bool shadows);

	const RenderCore* GetCore() const;

	Camera& GetCamera();


	shared_ptr<MeshRenderableStatic> CreateMeshStatic(const string& subpath);
	void CreateMeshStaticSwappable(const string& subpathCurrent, 
								   const string& subpathFuture, 
								   const MeshRenderableStatic& data);
	shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);
	shared_ptr<TextRenderable> CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin);

	void SetTimelineStateCurrent();
	void SetTimelineStateFuture();

	shared_ptr<DirectionalLight> GetDirectionalLight();
	bool RegisterPointLight(shared_ptr<PointLight> pointLight);
	bool RegisterSpotLight(shared_ptr<SpotLight> spotLight);

private:
	unique_ptr<RenderCore> m_renderCore;

	TimelineState m_timelineState;
	uint m_renderableIDCounter;
	cs::List<RenderableState> m_worldRenderables;
	cs::List<shared_ptr<TextRenderable>> m_texts;

	cs::Color3f m_lightAmbient;
	float m_lightAmbientIntensity;
	shared_ptr<DirectionalLight> m_lightDirectional;
	cs::List<shared_ptr<PointLight>> m_lightsPoint;
	cs::List<shared_ptr<SpotLight>> m_lightsSpot;
	
	Camera m_mainCamera;


};

