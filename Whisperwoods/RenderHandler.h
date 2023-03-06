#pragma once

#include "RenderCore.h"
#include "Window.h"
#include "Camera.h"
#include "ConstantbufferData.h"
#include "ModelResource.h"
#include "MeshRenderable.h"
#include "TextRenderable.h"
#include "GUIRenderable.h"
#include "Light.h"
#include "Font.h"
#include "LevelResource.h"
#include "Level.h"
#include "QuadTree.h"

struct EnvMesh
{
	uint indexStarts[2];
	uint vertexStarts[2];

	cs::List<uint> submeshes[2];
	const ModelStaticResource* models[2];

	dx::BoundingBox boundedVolume;

	cs::List<Mat4> instances;

	// Hot-data
	cs::List<Mat4> hotInstances;
	uint instanceOffset;
	uint instanceCount;
};

class RenderHandler sealed
{
private:
	enum TimelineState // Readability only
	{
		TimelineStateCurrent = false,
		TimelineStateFuture = true
	};
public:
	RenderHandler();
	~RenderHandler();

	void InitCore(shared_ptr<Window> window);

	void LoadLevel(LevelResource* level, string image);

	void Draw();
	void UpdateGPUProfiler();
	void Present();

	void ExecuteDraw(TimelineState state, bool shadows);
	void ZPrepass(TimelineState state);

	void ExecuteDraw(const Camera& povCamera, TimelineState state, bool shadows);

	void RenderGUI();

	RenderCore* GetCore() const;
	Camera& GetCamera();

	void SetupEnvironmentAssets();
	void LoadEnvironment(const Level* level);
	void UnLoadEnvironment();


	shared_ptr<MeshRenderableStatic> CreateMeshStatic(const string& subpath);
	std::pair<shared_ptr<MeshRenderableStatic>, shared_ptr<MeshRenderableStatic>> CreateMeshStaticSwappable(const string& subpathCurrent,
								   const string& subpathFuture);
	shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);
	shared_ptr<TextRenderable> CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin);

	shared_ptr<GUIRenderable> CreateGUIRenderable(const string& subpath);

	void DestroyMeshStatic(shared_ptr<MeshRenderableStatic> renderable);

	void SetTimelineStateCurrent();
	void SetTimelineStateFuture();

	shared_ptr<DirectionalLight> GetDirectionalLight();
	bool RegisterPointLight(shared_ptr<PointLight> pointLight);
	bool RegisterSpotLight(shared_ptr<SpotLight> spotLight);

	void SetPlayerMatrix(const Mat4& matrix);

	void ClearShadowRenderables();
	void RegisterLastRenderableAsShadow();
	void ExecuteStaticShadowDraw();


private:
	void QuadCull(const Camera& camPOV);
	void DrawInstances(uint state, bool shadows, bool discardPipeline);


private:
	struct EnvMaterial
	{
		string materialName;
		const MaterialResource* material;

		// All submeshes that share this material, and exist within the buffers
		cs::List<uint> submeshes;
	};

	struct EnvSubmesh
	{
		uint indexCount;
		uint indexOffset;

		uint model;
		uint localSubmesh;
	};
	
	unique_ptr<RenderCore> m_renderCore;

	TimelineState m_timelineState;
	uint m_renderableIDCounter;
	cs::List<std::pair<shared_ptr<WorldRenderable>, shared_ptr<WorldRenderable>>> m_worldRenderables;
	cs::List<shared_ptr<GUIRenderable>> m_guiRenderables;
	cs::List<shared_ptr<TextRenderable>> m_texts;

	cs::Color3f m_lightAmbient;
	float m_lightAmbientIntensity;
	shared_ptr<DirectionalLight> m_lightDirectional;
	cs::List<shared_ptr<PointLight>> m_lightsPoint;
	cs::List<shared_ptr<SpotLight>> m_lightsSpot;

	Mat4 m_playerMatrix;

	const Level* m_currentLevel;

	// Indicies of the renderables to use when calculating the static shadowmap along the 
	cs::List<int> m_shadowRenderables;

	ComPtr<ID3D11Buffer> m_envIndices[2];
	ComPtr<ID3D11Buffer> m_envVertices[2];

	cs::List<EnvMaterial> m_envMaterials[2];
	cs::List<EnvSubmesh> m_envSubmeshes[2];
	EnvMesh m_envMeshes[LevelAssetCount];

	QuadTree<Mat4> m_envQuadTree;
	dx::BoundingBox boundingVolumes[LevelAssetCount];

	// Hot-data
	cs::List<Mat4> m_envInstances;
	ComPtr<ID3D11Buffer> m_envInstanceBuffer;

	Camera m_mainCamera;
};

