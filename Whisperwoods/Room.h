#pragma once
#include "LevelResource.h"
#include "MeshRenderable.h"
#include "ModelResource.h"
#include "GameObject.h"
#include "Renderer.h"
#include "CHSL/Vector.h"
#include "Level.h"

class Room : public GameObject
{
public:
	// Map resource here
	Mat4 m_modelOffset;
	ModelStaticResource* m_modelResource;
	shared_ptr<MeshRenderableStatic> m_renderable;

	const Level* m_level;
	const LevelResource* m_levelResource;

	Room() = default;
	Room(const Level* level, std::string modelResource, Mat4 modelOffset);

	void Update(float deltaTime) override;

	cs::List<GameObject*> traverseAndGenerateHiearchy;

	Vec3 bitMapToWorldPos(Point2 samplePoint);
	Vec2 GetNineSampleVector(Point2 point);
	Point2 worldToBitmapPoint(Vec3 worldPos);
	LevelPixel sampleBitMap(Vec3 worldPos);
	Vec2 sampleBitMapCollision(Vec3 worldPos);

private:
	MaterialResource m_material;
};