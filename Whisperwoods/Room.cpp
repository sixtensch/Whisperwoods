#include "Core.h"
#include "Room.h"
#include "LevelImporter.h"
#include "Resources.h"


Room::Room(std::string modelResource, std::string mapResource, Mat4 modelOffset, Renderer* renderer)
{
	Resources& resources = Resources::Get();

	// Move to resource manager later.
	m_levelResource = {};
	LevelImporter::ImportImage("Examplemap.png", renderer->GetRenderCore(), &m_levelResource);

	//m_modelResource = (ModelStaticResource*)resources.GetResource(ResourceTypeModelStatic, modelResource);
	m_renderable = Renderer::CreateMeshStatic(modelResource);
	m_modelOffset = modelOffset;
	m_renderable->worldMatrix = m_modelOffset;
	m_renderable->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "MapTest.wwmt"));
}

void Room::Update(float deltaTime)
{
	transform.CalculateWorldMatrix();
	m_renderable->worldMatrix = transform.worldMatrix * m_modelOffset;
}

Point2 Room::worldToBitmapPoint(Vec3 worldPos)
{
	Vec3 roomWpos = transform.GetWorldPosition();
	Quaternion worldRotation = transform.GetWorldRotation();
	Vec3 right(-1, 0, 0);
	Vec3 forward(0, 0, 1);
	right = worldRotation * right;
	forward = worldRotation * forward;
	Vec3 normalizedPos =  worldPos - roomWpos;
	float relativeRight = right.Dot(normalizedPos) * BM_PIXELS_PER_UNIT;
	float realtiveForward = forward.Dot(normalizedPos) * BM_PIXELS_PER_UNIT;
	return Point2(relativeRight+(m_levelResource.pixelWidth/2), realtiveForward + (m_levelResource.pixelHeight/2));
}

LevelPixel Room::sampleBitMap(Vec3 worldPos)
{
	Point2 point = worldToBitmapPoint(worldPos);
	int x = cs::iclamp(point.x, 0, m_levelResource.pixelWidth-1);
	int y = cs::iclamp(point.y, 0, m_levelResource.pixelHeight-1);
	LevelPixel returnPixel = m_levelResource.bitmap[x + y * m_levelResource.pixelWidth];
	return returnPixel;
}

Vec3 Room::bitMapToWorldPos(Point2 samplePoint)
{
	if (!BM_PIXELS_PER_UNIT)
	{
		LOG_WARN("bitMapToWorldPos/defines: BM_PIXELS_PER_UNIT CAN NOT BE 0 -> DIVISION BY 0");
		return Vec3(0, 0, 0);
	}
	float normalizedX = (float)(samplePoint.x) / (float)BM_PIXELS_PER_UNIT;
	float normalizedY = (float)(samplePoint.y) / (float)BM_PIXELS_PER_UNIT;

	Vec3 normalization(
		(float)(m_levelResource.pixelWidth / 2) / (float)BM_PIXELS_PER_UNIT, 
		0, 
		(float)(m_levelResource.pixelHeight / 2) / (float)BM_PIXELS_PER_UNIT);

	Vec3 localPos(normalizedX, 0, normalizedY);
	localPos = localPos - normalization;
	localPos.x = -localPos.x;
	Vec3 worldPos = transform.GetWorldPosition(); 
	Vec3 rotationConverted (transform.GetWorldRotation() * localPos);

	return worldPos + rotationConverted;
}

Vec2 Room::GetNineSampleVector(Point2 point)
{
	int x = cs::iclamp(point.x, 0, m_levelResource.pixelWidth - 1);
	int y = cs::iclamp(point.y, 0, m_levelResource.pixelHeight - 1);
	int xP = cs::iclamp(point.x + 1, 0, m_levelResource.pixelWidth - 1);
	int xM = cs::iclamp(point.x - 1, 0, m_levelResource.pixelWidth - 1);
	int yP = cs::iclamp(point.y + 1, 0, m_levelResource.pixelWidth - 1);
	int yM = cs::iclamp(point.y - 1, 0, m_levelResource.pixelWidth - 1);
	LevelPixel p0 = m_levelResource.bitmap[x + y * m_levelResource.pixelWidth];
	
	LevelPixel p1 = m_levelResource.bitmap[x + yP * m_levelResource.pixelWidth];
	LevelPixel p2 = m_levelResource.bitmap[x + yM * m_levelResource.pixelWidth];

	LevelPixel p3 = m_levelResource.bitmap[xP + y * m_levelResource.pixelWidth];
	LevelPixel p4 = m_levelResource.bitmap[xP + yP * m_levelResource.pixelWidth];
	LevelPixel p5 = m_levelResource.bitmap[xP + yM * m_levelResource.pixelWidth];

	LevelPixel p6 = m_levelResource.bitmap[xM + y * m_levelResource.pixelWidth];
	LevelPixel p7 = m_levelResource.bitmap[xM + yP * m_levelResource.pixelWidth];
	LevelPixel p8 = m_levelResource.bitmap[xM + yM * m_levelResource.pixelWidth];

	return Vec2(
		(
			cs::iclamp(p3, 0, 1) + 
			cs::iclamp(p4, 0, 1) +
			cs::iclamp(p5, 0, 1)) -
		(
			cs::iclamp(p6, 0, 1) +
			cs::iclamp(p7, 0, 1) +
			cs::iclamp(p8, 0, 1)),
		(
			cs::iclamp(p1, 0, 1) +
			cs::iclamp(p4, 0, 1) +
			cs::iclamp(p7, 0, 1)) -
		(
			cs::iclamp(p2, 0, 1) +
			cs::iclamp(p5, 0, 1) +
			cs::iclamp(p8, 0, 1)
		)
	);
}

Vec2 Room::sampleBitMapCollision(Vec3 worldPos)
{
	Point2 point = worldToBitmapPoint(worldPos);
	int x = cs::iclamp(point.x, 0, m_levelResource.pixelWidth - 1);
	int y = cs::iclamp(point.y, 0, m_levelResource.pixelHeight - 1);
	int xP = cs::iclamp(point.x + 2, 0, m_levelResource.pixelWidth - 1);
	int xM = cs::iclamp(point.x - 2, 0, m_levelResource.pixelWidth - 1);
	int yP = cs::iclamp(point.y + 2, 0, m_levelResource.pixelWidth - 1);
	int yM = cs::iclamp(point.y - 2, 0, m_levelResource.pixelWidth - 1);

	Vec2 p0 = GetNineSampleVector({ x, y });
	Vec2 p1 = GetNineSampleVector({ x, yP });
	Vec2 p2 = GetNineSampleVector({ x, yM });

	Vec2 p3 = GetNineSampleVector({ xP, y });
	Vec2 p4 = GetNineSampleVector({ xP, yP }) * 0.5f;
	Vec2 p5 = GetNineSampleVector({ xP, yM }) * 0.5f;

	Vec2 p6 = GetNineSampleVector({ xM, y });
	Vec2 p7 = GetNineSampleVector({ xM, yP }) * 0.5f;
	Vec2 p8 = GetNineSampleVector({ xM, yM }) * 0.5f;

	Vec2 returnVal(p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8);

	Vec3 ret2(returnVal.x, 0, returnVal.y);
	ret2 = transform.GetWorldRotation().Conjugate() * ret2;

	return Vec2(ret2.x, ret2.z);
}
