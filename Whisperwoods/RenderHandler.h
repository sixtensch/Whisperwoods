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
#include "LevelResource.h"
#include "Level.h"
#include "QuadTree.h"

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
		RenderableState& operator=(const RenderableState& other) noexcept
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

	void LoadLevel(LevelResource* level, string image);

	void Draw();
	void Present();

	void ExecuteDraw(const Camera& povCamera, TimelineState state, bool shadows);

	RenderCore* GetCore() const;
	Camera& GetCamera();

	void SetupEnvironmentAssets();
	void LoadEnvironment(const Level* level);

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

	void SetPlayerMatrix(const Mat4& matrix);

private:
	void DrawInstances(bool shadows);



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

	struct EnvMesh
	{
		uint indexStarts[2];
		uint vertexStarts[2];

		cs::List<uint> submeshes[2];
		const ModelStaticResource* models[2];

		//cs::Box2

		cs::List<Mat4> instances;

		// Hot-data
		cs::List<Mat4> hotInstances;
		uint instanceOffset;
		uint instanceCount;
	};

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

	Mat4 m_playerMatrix;

	const Level* m_currentLevel;

	ComPtr<ID3D11Buffer> m_envIndices[2];
	ComPtr<ID3D11Buffer> m_envVertices[2];

	cs::List<EnvMaterial> m_envMaterials[2];
	cs::List<EnvSubmesh> m_envSubmeshes[2];
	EnvMesh m_envMeshes[LevelAssetCount];

	//QuadTree<Mat4> m_envQuadTree;

	// Hot-data
	cs::List<Mat4> m_envInstances;
	ComPtr<ID3D11Buffer> m_envInstanceBuffer;

	Camera m_mainCamera;
};

