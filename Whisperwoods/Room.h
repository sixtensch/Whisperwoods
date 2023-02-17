#pragma once
#include "LevelResource.h"
#include "MeshRenderable.h"
#include "ModelResource.h"
#include "GameObject.h"
#include "Renderer.h"
#include "CHSL/Vector.h"

class Room : public GameObject
{
public:
	// Map resource here
	Mat4 m_modelOffset;
	LevelResource m_levelResource;
	ModelStaticResource* m_modelResource;
	shared_ptr<MeshRenderableStatic> m_renderable;

	Room() = default;
	Room(std::string modelResource, std::string mapResource, Mat4 modelOffset, Renderer* renderer);

	void Update(float deltaTime) override;


	Vec3 bitMapToWorldPos(Point2 samplePoint);
	Vec2 GetNineSampleVector(Point2 point);
	Point2 worldToBitmapPoint(Vec3 worldPos);
	LevelPixel sampleBitMap(Vec3 worldPos);
	Vec2 sampleBitMapCollision(Vec3 worldPos);
};