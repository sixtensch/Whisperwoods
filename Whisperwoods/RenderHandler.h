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

class RenderHandler sealed
{
public:
	RenderHandler();
	~RenderHandler();

	void InitCore(shared_ptr<Window> window);

	void LoadLevel(LevelResource* level, string image);

	void Draw();
	void Present();

	const RenderCore* GetCore() const;

	Camera& GetCamera();

	void SetupEnvironmentAssets();
	void LoadEnvironment(const Level* level);

	shared_ptr<MeshRenderableStatic> CreateMeshStatic(const string& subpath);
	shared_ptr<MeshRenderableRigged> CreateMeshRigged(const string& subpath);
	shared_ptr<TextRenderable> CreateTextRenderable(const wchar_t* text, dx::SimpleMath::Vector2 fontPos, Font font, cs::Color4f color, Vec2 origin);

	shared_ptr<DirectionalLight> GetDirectionalLight();
	bool RegisterPointLight(shared_ptr<PointLight> pointLight);
	bool RegisterSpotLight(shared_ptr<SpotLight> spotLight);

private:
	void DrawInstances();



private:
	struct EnvMaterial
	{
		ComPtr<ID3D11Buffer> allVertices;
		ComPtr<ID3D11Buffer> allIndices;

		string materialName;
		const MaterialResource* material;

		// All submeshes that share this material, and exist within the buffers
		cs::List<uint> submeshes;
	};

	struct EnvSubmesh
	{
		uint indexOffset;

		uint model;
		uint localSubmesh;
	};

	struct EnvMesh
	{
		cs::List<uint> submeshes[2];
		const ModelStaticResource* models[2];

		ComPtr<ID3D11Buffer> instanceBuffer;
		cs::List<Mat4> instances;
	};

	unique_ptr<RenderCore> m_renderCore;

	uint m_renderableIDCounter;
	cs::List<shared_ptr<WorldRenderable>> m_worldRenderables;
	cs::List<shared_ptr<TextRenderable>> m_texts;

	cs::Color3f m_lightAmbient;
	float m_lightAmbientIntensity;
	shared_ptr<DirectionalLight> m_lightDirectional;
	cs::List<shared_ptr<PointLight>> m_lightsPoint;
	cs::List<shared_ptr<SpotLight>> m_lightsSpot;

	const Level* m_currentLevel;

	cs::List<EnvMaterial> m_envMaterials[2];
	cs::List<EnvSubmesh> m_envSubmeshes[2];
	EnvMesh m_envMeshes[LevelAssetCount];

	Camera m_mainCamera;
};

