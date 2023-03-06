#include "Core.h"
#include "Room.h"
#include "LevelImporter.h"
#include "Resources.h"
#include "Renderer.h"
#include "SoundResource.h"


Room::Room(const Level* level, std::string modelResource, Mat4 modelOffset)
{
	m_level = level;
	m_levelResource = level->resource;

	//// Add ambiance sounds around the room
	//Resources& resources = Resources::Get();
	//int numSounds = 3;
	//float radius = m_levelResource->worldWidth / 2.0f;
	//for (int i = 0; i < numSounds; i++)
	//{
	//	float angle = ((cs::c_pi * 2.0f) / numSounds) * (float)i;

	//	Vec3 pos( cos( angle ), 0.0f, sin( angle ) );
	//	pos = pos * radius*1.5f;
	//	pos.y = 8.0f;
	//	std::string soundName = "Jungle_AmbianceLoop0" + std::to_string( (i+1)%6 ) + ".wav";
	//	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource( ResourceTypeSound, soundName ))->currentSound;
	//	shared_ptr<AudioSource> audioSource = make_shared<AudioSource>( pos, 0.5f, 1.0f, 0.0f, radius*3.0f, soundPtr );
	//	audioSource->mix2d3d = 0.75f;
	//	audioSource->loop = true;
	//	audioSource->Play();
	//	//this->AddChild( audioSource.get() );
	//	m_ambianceSources.Add( audioSource );
	//}

	/*m_renderable = Renderer::CreateMeshStatic(modelResource);
	m_modelOffset = modelOffset;
	m_renderable->worldMatrix = m_modelOffset;*/

	m_material = MaterialResource();
	m_material.specular = Vec3(0.5f, 0.5f, 0.5f);
	m_material.textureDiffuse = level->resource->source;
	m_renderable->Materials().AddMaterial( &m_material );
}

Room::Room( const Level* level, std::string modelResource, std::string modelResource2, Mat4 modelOffset, Mat4 modelOffset2 )
{
	m_level = level;
	m_levelResource = level->resource;

	Renderer::ClearShadowRenderables();

	// Stop previous sounds and clear
	for (int i = 0; i < m_ambianceSources.Size(); i++)
	{
		m_ambianceSources[i]->Stop();
	}
	if (m_ambianceSources.Size())
		m_ambianceSources.Clear();

	// Add ambiance sounds around the room
	Resources& resources = Resources::Get();
	int numSounds = 3;
	float radius = m_levelResource->worldWidth / 2.0f;
	for (int i = 0; i < numSounds; i++)
	{
		float angle = ((cs::c_pi * 2.0f) / numSounds) * (float)i;

		Vec3 pos( cos( angle ), 0.0f, sin( angle ) );
		pos = pos * radius * 1.5f;
		pos.y = 8.0f;
		std::string soundName = "Jungle_AmbianceLoop0" + std::to_string( (i + 1) % 6 ) + ".wav";
		FMOD::Sound* soundPtr = (Resources::Get().GetSound(soundName))->currentSound;
		shared_ptr<AudioSource> audioSource = make_shared<AudioSource>( pos, 0.5f, 1.0f, 0.0f, radius * 3.0f, soundPtr );
		audioSource->mix2d3d = 0.75f;
		audioSource->loop = true;
		audioSource->Play();
		//this->AddChild( audioSource.get() );
		m_ambianceSources.Add( audioSource );
	}

	// Plane
	m_renderable = Renderer::CreateMeshStatic( modelResource );
	m_modelOffset = modelOffset;
	m_renderable->worldMatrix = /*transform.worldMatrix * */m_modelOffset;
	m_material = MaterialResource();
	m_material.specular = Vec3( 0.5f, 0.5f, 0.5f );
	m_material.textureDiffuse = level->resource->source;
	m_renderable->Materials().AddMaterial( &m_material );
	
	
	// Cylinder thing
	m_wallsFloorOffset = modelOffset2;

	m_wallsAndFloorRenderable = Renderer::CreateMeshStatic( modelResource2 );

	// Registers the last added renderable as one to do shadows on
	Renderer::RegisterShadowRenderable();

	m_wallsAndFloorRenderable->worldMatrix = modelOffset2;
	m_wallsAndFloorRenderable->Materials().AddMaterial(resources.GetMaterial("TestSceneBigTree.wwmt"));
	m_wallsAndFloorRenderable->Materials().AddMaterial(resources.GetMaterial("TestSceneGround.wwmt"));
	
	
	//GenerateRoomShadowMap();
}

void Room::GenerateRoomShadowMap()
{
	Renderer::ExecuteShadowRender();
}


void Room::Update(float deltaTime)
{
	transform.CalculateWorldMatrix();
	m_renderable->worldMatrix = transform.worldMatrix * m_modelOffset;
	m_wallsAndFloorRenderable->worldMatrix = transform.worldMatrix * m_wallsFloorOffset;
}

Point2 Room::worldToBitmapPoint(Vec3 worldPos)
{
	Vec3 roomWpos = transform.GetWorldPosition();
	Quaternion worldRotation = transform.GetWorldRotation()/*.Conjugate()*/;
	Vec3 right(-1, 0, 0);
	Vec3 forward(0, 0, 1);
	right = worldRotation * right;
	forward = worldRotation * forward;
	Vec3 normalizedPos =  worldPos - roomWpos;
	float relativeRight = right.Dot(normalizedPos) * BM_PIXELS_PER_UNIT;
	float realtiveForward = forward.Dot(normalizedPos) * BM_PIXELS_PER_UNIT;
	return Point2((int)((m_levelResource->pixelWidth/2) - relativeRight), (int)((m_levelResource->pixelHeight/2) - realtiveForward));
}

LevelPixel Room::sampleBitMap(Vec3 worldPos)
{
	Point2 point = worldToBitmapPoint(worldPos);
	int x = cs::iclamp(point.x, 0, m_levelResource->pixelWidth-1);
	int y = cs::iclamp(point.y, 0, m_levelResource->pixelHeight-1);
	LevelPixel returnPixel = m_levelResource->bitmap[x + y * m_levelResource->pixelWidth];
	return returnPixel;
}

Vec3 Room::bitMapToWorldPos(Point2 samplePoint)
{
	if (!BM_PIXELS_PER_UNIT)
	{
		LOG_WARN("bitMapToWorldPos/defines: BM_PIXELS_PER_UNIT CAN NOT BE 0 -> DIVISION BY 0");
		return Vec3(0, 0, 0);
	}

	Vec2 relativePosition = (Vec2)samplePoint - Vec2(m_levelResource->pixelWidth, m_levelResource->pixelHeight) * 0.5f;
	relativePosition /= BM_PIXELS_PER_UNIT;

	//float normalizedX = (float)(samplePoint.x) / (float)BM_PIXELS_PER_UNIT;
	//float normalizedY = (float)(samplePoint.y) / (float)BM_PIXELS_PER_UNIT;

	//Vec3 normalization(
	//	(float)(m_levelResource->pixelWidth / 2) / (float)BM_PIXELS_PER_UNIT, 
	//	0, 
	//	(float)(m_levelResource->pixelHeight / 2) / (float)BM_PIXELS_PER_UNIT);

	//Vec3 localPos(normalizedX, 0, normalizedY);
	//localPos = localPos - normalization;
	////localPos.y = -localPos.y;

	Vec3 worldPos = transform.GetWorldPosition(); 
	Vec3 rotationConverted = transform.GetWorldRotation().Conjugate() * Vec3(relativePosition.x, 0, relativePosition.y);

	return worldPos + rotationConverted;
}

Vec2 Room::GetNineSampleVector(Point2 point)
{
	int x = cs::iclamp(point.x, 0, m_levelResource->pixelWidth - 1);
	int y = cs::iclamp(point.y, 0, m_levelResource->pixelHeight - 1);
	int xP = cs::iclamp(point.x + 1, 0, m_levelResource->pixelWidth - 1);
	int xM = cs::iclamp(point.x - 1, 0, m_levelResource->pixelWidth - 1);
	int yP = cs::iclamp(point.y + 1, 0, m_levelResource->pixelWidth - 1);
	int yM = cs::iclamp(point.y - 1, 0, m_levelResource->pixelWidth - 1);
	LevelPixelFlag p0 = m_levelResource->bitmap[x + y * m_levelResource->pixelWidth].flags;
	
	LevelPixelFlag p1 = m_levelResource->bitmap[x + yP * m_levelResource->pixelWidth].flags;
	LevelPixelFlag p2 = m_levelResource->bitmap[x + yM * m_levelResource->pixelWidth].flags;

	LevelPixelFlag p3 = m_levelResource->bitmap[xP + y * m_levelResource->pixelWidth].flags;
	LevelPixelFlag p4 = m_levelResource->bitmap[xP + yP * m_levelResource->pixelWidth].flags;
	LevelPixelFlag p5 = m_levelResource->bitmap[xP + yM * m_levelResource->pixelWidth].flags;

	LevelPixelFlag p6 = m_levelResource->bitmap[xM + y * m_levelResource->pixelWidth].flags;
	LevelPixelFlag p7 = m_levelResource->bitmap[xM + yP * m_levelResource->pixelWidth].flags;
	LevelPixelFlag p8 = m_levelResource->bitmap[xM + yM * m_levelResource->pixelWidth].flags;

	return Vec2(
		(float)(
			cs::iclamp(p3, 0, 1) + 
			cs::iclamp(p4, 0, 1) +
			cs::iclamp(p5, 0, 1)
		) -
		(
			cs::iclamp(p6, 0, 1) +
			cs::iclamp(p7, 0, 1) +
			cs::iclamp(p8, 0, 1)
		),
		(float)(
			cs::iclamp(p1, 0, 1) +
			cs::iclamp(p4, 0, 1) +
			cs::iclamp(p7, 0, 1)
		) -
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
	int x = cs::iclamp(point.x, 0, m_levelResource->pixelWidth - 1);
	int y = cs::iclamp(point.y, 0, m_levelResource->pixelHeight - 1);
	int xP = cs::iclamp(point.x + 2, 0, m_levelResource->pixelWidth - 1);
	int xM = cs::iclamp(point.x - 2, 0, m_levelResource->pixelWidth - 1);
	int yP = cs::iclamp(point.y + 2, 0, m_levelResource->pixelWidth - 1);
	int yM = cs::iclamp(point.y - 2, 0, m_levelResource->pixelWidth - 1);

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
