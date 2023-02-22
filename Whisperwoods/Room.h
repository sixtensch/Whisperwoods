#pragma once
#include "LevelResource.h"
#include "MeshRenderable.h"
#include "ModelResource.h"
#include "GameObject.h"
#include "Renderer.h"
#include "CHSL/Vector.h"
#include "Level.h"
#include "AudioSource.h"
#include "Sound.h"

class Room : public GameObject
{
public:
	// Map resource here
	Mat4 m_modelOffset;
	ModelStaticResource* m_modelResource;
	shared_ptr<MeshRenderableStatic> m_renderable;

	cs::List<shared_ptr<AudioSource>> m_ambianceSources;

	// Might have to move
	Mat4 m_wallsFloorOffset;
	ModelStaticResource* m_roomWallsAndFloor;
	shared_ptr<MeshRenderableStatic> m_wallsAndFloorRenderable;

	const Level* m_level;
	const LevelResource* m_levelResource;

	Room() = default;
	Room(const Level* level, std::string modelResource, Mat4 modelOffset);
	Room( const Level* level, std::string modelResource, std::string modelResource2, Mat4 modelOffset, Mat4 modelOffset2 );


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