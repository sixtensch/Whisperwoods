#include "core.h"
#include "Game.h"
#include "LevelImporter.h"
#include "LevelHandler.h"
#include "SoundResource.h"
#include "Resources.h"
#include "Input.h"
#include <imgui.h>

Game::Game() :
	m_floor(),
	m_isHubby(false),
	m_currentRoom(nullptr),
	m_isInFuture(false),
	m_isSwitching(false),
	m_finishedCharging(false),
	m_maxStamina(10.0f),
	m_switchVals({ 1.0f, 0.5f, 3.0f, 0.0f }),
	m_detectionLevelGlobal(0.0f),
	m_detectionLevelFloor(0.0f),
	m_camFovChangeSpeed(cs::c_pi / 4.0f),
	m_envParams({})
	m_reachedLowestStamina(false)
{}

Game::~Game() {}

void Game::Update(float deltaTime, Renderer* renderer)
{
	// Always do the following:
	Camera& cameraRef = renderer->GetCamera();

	// Player update
	m_player->Update(deltaTime);
	m_currentRoom->Update(deltaTime);
	bool isSeen = false;
	m_player->UpdateStamina( m_maxStamina );
	float currentStamina = m_player->GetCurrentStamina();
	Renderer::SetPlayerMatrix( m_player->transform.worldMatrix );

	for (int i = 0; i < m_enemies.Size(); i++)
	{
		m_enemies[i]->Update(deltaTime);
		if (m_enemies[i]->m_carcinian->enabled == true)
		{
			if (m_enemies[i]->SeesPlayer(Vec2(m_player->transform.worldPosition.x, m_player->transform.worldPosition.z), *m_currentRoom, *m_audioSource) == true)
			{
				isSeen = true;
			}
		}
	}
	static float initialCamFov;
	if (isSeen == true)
	{
		m_timeUnseen = 0.0f;
		if (IsDetected(deltaTime))
		{
			// D E A T H
			m_maxStamina = 10.0f;
			m_player->ResetStaminaToMax(m_maxStamina);
			UnLoadPrevious();
			LoadHubby();
			m_player->ReloadPlayer();
			isSeen = false;
			m_detectionLevelGlobal = 0.0f;
			m_detectionLevelFloor = 0.0f;
		}
	}
	else
	{
		m_timeUnseen += deltaTime;
		if (m_timeUnseen > m_timeBeforeDetectionLowers)
		{
			LowerToFloor(deltaTime);
		}
	}

	for (int i = 0; i < m_staticObjects.Size(); i++)
	{
		m_staticObjects[i]->Update(deltaTime);
	}
	
	Renderer::SetPlayerMatrix(m_player->transform.worldMatrix);
	if (!m_isHubby) // if not in hubby
	// Time switch logic.
	{
		
		if (Input::Get().IsKeyPressed(KeybindPower) && IsAllowedToSwitch() /*&& m_reachedLowestStamina == false*/)  
		{
			m_isSwitching = true;
			m_finishedCharging = false;
			initialCamFov = cameraRef.GetFov();
		}

		//if (m_reachedLowestStamina == true && m_forcedBackToPresent == false)
		//{
		//	m_isSwitching = true;
		//	m_finishedCharging = false;
		//	initialCamFov = cameraRef.GetFov();
		//	m_forcedBackToPresent = true;
		//}
		

		static float totalFovDelta = 0.0f;
		if (m_isSwitching )
		{
			if (!ChargeIsDone())
			{
				totalFovDelta += m_camFovChangeSpeed * deltaTime;
				float newFov = initialCamFov + totalFovDelta;

					// Max total fov cant exceed half circle.
					if (newFov > cs::c_pi)
					{
						newFov = cs::c_pi;
					}

					cameraRef.SetFov( newFov );
			}
			else
			{
				if (!m_finishedCharging)
				{
					ChangeTimeline( renderer );
					m_finishedCharging = true;
					cameraRef.SetFov( initialCamFov );
					totalFovDelta = 0.0f;
				}
			}

			if (!SwitchIsDone())
			{
				m_switchVals.timeSinceSwitch += deltaTime;
			}
			else
			{
				m_switchVals.timeSinceSwitch = 0.0f;
				m_isSwitching = false;
			}


			UpdateTimeSwitchBuffers( renderer );
			
		}

		m_maxStamina -= deltaTime * STAMINA_DECAY_MULTIPLIER * m_isInFuture * m_finishedCharging;
		if (m_maxStamina < 1.0f)
		{
			m_maxStamina = 1.0f;
		}

		if (Input::Get().IsDXKeyPressed( DXKey::H ) && !m_isInFuture)
		{
			UnLoadPrevious();
			LoadHubby();
			m_player->ReloadPlayer();
		}
	}
	else // If in hubby
	{
		if (Input::Get().IsDXKeyPressed( DXKey::L ))
		{
			UnLoadPrevious();
			LoadTest();
			m_player->ReloadPlayer();
		}
		if (Input::Get().IsDXKeyPressed(DXKey::H))
		{
			UnLoadPrevious();
			LoadHubby();
			m_player->ReloadPlayer();
		}
	}

#if WW_DEBUG
	if (ImGui::Begin("Gameplay Vars"))
	{
		ImGui::Text("Max Stamina: %f", m_maxStamina);
		ImGui::Text("Current Stamina: %f", currentStamina);
		ImGui::DragFloat( "Detection Level Global", &m_detectionLevelGlobal, 0.1f, 0.0f, 1.0f );
		ImGui::Text( "Detection level Floor: %f", m_detectionLevelFloor );
		ImGui::Text("Time left until future death: %f", m_timeYouSurviveInFuture - m_dangerousTimeInFuture);
		ImGui::Checkbox( "Future", &m_isInFuture );
	}
	ImGui::End();

	if (ImGui::Begin("Environment Parameters"))
	{
		ImGui::DragInt("Spawn Seed", &m_envParams.spawnSeed, 0.1f);
		ImGui::DragInt("Scale Seed", &m_envParams.scaleSeed, 0.1f);
		ImGui::DragInt("Rotation Seed", &m_envParams.rotationSeed, 0.1f);
		ImGui::DragInt("Diversity Seed", &m_envParams.diversitySeed, 0.1f);
		ImGui::DragFloat("Rotation Multiplier", &m_envParams.rotateMult, 0.01f);
		ImGui::DragFloat("X Multiplier", &m_envParams.xMult, 0.01f);
		ImGui::DragFloat("Y Multiplier", &m_envParams.yMult, 0.01f);
		ImGui::DragFloat("Scale base", &m_envParams.scaleBase, 0.01f);
		ImGui::DragFloat("Scale multiplier", &m_envParams.scaleMult, 0.01f);
		ImGui::DragFloat("Density Unwalkable Outer", &m_envParams.densityUnwalkableOuter, 0.01f);
		ImGui::DragFloat("Density Unwalkable Inner", &m_envParams.densityUnwalkableInner, 0.01f);
		ImGui::DragFloat("Density Walkable", &m_envParams.densityWalkable, 0.01f);
		ImGui::DragFloat("Min Density Spawn", &m_envParams.minDensity, 0.01f);
		ImGui::DragFloat("Scale multiplier Sones", &m_envParams.scaleMultiplierStones, 0.01f);
		ImGui::DragFloat("Scale multiplier Trees", &m_envParams.scaleMultiplierTrees, 0.01f);
		ImGui::DragFloat("Scale multiplier Foliage", &m_envParams.scaleMultiplierFoliage, 0.01f);
		ImGui::DragFloat("Scale Effect Density", &m_envParams.scaleEffectDensity, 0.01f);
		ImGui::DragInt("Edge Distance Trunks", &m_envParams.edgeSampleDistanceTrunks, 0.1f);
		ImGui::DragInt("Edge Distance Trees", &m_envParams.edgeSampleDistanceTrees, 0.1f);
		ImGui::DragInt("Edge Distance Stones", &m_envParams.edgeSampleDistanceStones, 0.1f);
	}
	ImGui::End();
#endif

	

	m_maxStamina -= deltaTime * STAMINA_DECAY_MULTIPLIER * m_isInFuture * m_finishedCharging;
	
	if ( m_maxStamina < 1.0f ) // DO NOT CHANGE THIS
	{
		m_maxStamina = 1.0f; // DO NOT CHANGE THIS
		
		
		if (m_reachedLowestStamina == false )
		{
			m_reachedLowestStamina = true;
		}
		else
		{
			m_dangerousTimeInFuture += deltaTime;
		}

		/// D E A T H ///
		if (m_dangerousTimeInFuture >= m_timeYouSurviveInFuture) // how long you can survive in future with 0 stamina (seconds)
		{
			ChangeTimeline(renderer);
			m_maxStamina = 10.0f;
			m_player->ResetStaminaToMax(m_maxStamina);
			UnLoadPrevious();
			LoadHubby();
			m_player->ReloadPlayer();
		}
	}
	if (!m_isInFuture)
	{
		m_dangerousTimeInFuture = 0.0f;
	}
}

void Game::Init()
{
	// Audio test startup
	FMOD::Sound* soundPtr = ((SoundResource*)Resources::Get().GetWritableResource(ResourceTypeSound, "Duck.mp3"))->currentSound;
	m_audioSource = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 0.2f, 1.1f, 0.0f, 10.0f, soundPtr);

	m_audioSource->Play();

	// Environment parameters
	m_envParams.spawnSeed = 652;
	m_envParams.scaleSeed = 635;
	m_envParams.rotationSeed = 425;
	m_envParams.diversitySeed = 1337;
	m_envParams.rotateMult = 1.350f;
	m_envParams.xMult = 3.0f;
	m_envParams.yMult = 3.0f;
	m_envParams.scaleBase = 0.1f;
	m_envParams.scaleMult = 1.1f;
	m_envParams.densityUnwalkableOuter = 0.150f;
	m_envParams.densityUnwalkableInner = 0.190f;
	m_envParams.densityWalkable = 0.15f;
	m_envParams.minDensity = 0.25f;
	m_envParams.scaleMultiplierStones = 0.4f;
	m_envParams.scaleMultiplierTrees = 0.05f;
	m_envParams.scaleMultiplierFoliage = 0.3f;
	m_envParams.scaleEffectDensity = -0.7f;
	m_envParams.edgeSampleDistanceTrunks = 10;
	m_envParams.edgeSampleDistanceTrees = 8;
	m_envParams.edgeSampleDistanceStones = 2;

	// Level handling
	m_levelHandler = std::make_unique<LevelHandler>();
	m_levelHandler->LoadFloors();

	// In-world objects and entities
	m_player = shared_ptr<Player>(new Player("Shadii_Rigged_Optimized.wwm", "Shadii_Animations.wwa", Mat::translation3(0.0f, 0.0f, 0.0f) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0)));
	m_directionalLight = Renderer::GetDirectionalLight();
	m_directionalLight->transform.position = { 0, 10, 0 };
	m_directionalLight->transform.SetRotationEuler({ -dx::XM_PIDIV4, 0.0f, 0.0f }); // Opposite direction of how the light should be directed
	m_directionalLight->diameter = 22.0f;
	m_directionalLight->intensity = 2.0f;
	m_directionalLight->color = cs::Color3f(0xFFFFD0);


}

void Game::DeInit()
{
	// Audio test 2 shutdown
	m_audioSource->pitch = 0.75f;
	m_audioSource->Play();

	int indexer = 0;
	while (m_audioSource->IsPlaying())
	{
		Sound::Get().Update();
		indexer++;
	}
}

void Game::LoadHubby()
{
	m_levelHandler->GenerateHubby( &m_floor, m_envParams );
	LoadRoom( &m_floor.rooms[0] );
	Mat4 worldScale = Mat::scale3( 0.15f, 0.15f, 0.15f );
	Mat4 worldPos = Mat::translation3( 0.0f, 0.0f, -2 );
	Mat4 worldRot = Mat::rotation3( cs::c_pi * -0.5f, cs::c_pi * 0.5f, 0 );
	Mat4 worldCombined = worldScale * worldPos * worldRot;
	m_isHubby = true;
}

void Game::LoadTest()
{
	m_levelHandler->GenerateTestFloor(&m_floor, m_envParams);
	LoadRoom(&m_floor.rooms[0]);
	Mat4 worldScale = Mat::scale3(0.15f, 0.15f, 0.15f);
	Mat4 worldPos = Mat::translation3(0.0f, 0.0f, -2);
	Mat4 worldRot = Mat::rotation3(cs::c_pi * -0.5f, cs::c_pi * 0.5f, 0);
	Mat4 worldCombined = worldScale * worldPos * worldRot;
	m_isHubby = false;
}

void Game::LoadGame(uint gameSeed)
{
	m_levelHandler->GenerateTestFloor(&m_floor, m_envParams);
	LoadRoom(&m_floor.rooms[m_floor.startRoom]);
	m_player->transform.position = m_floor.startPosition;
	Mat4 worldScale = Mat::scale3(0.15f, 0.15f, 0.15f);
	Mat4 worldPos = Mat::translation3(0.0f, 0.0f, -2);
	Mat4 worldRot = Mat::rotation3(cs::c_pi * -0.5f, cs::c_pi * 0.5f, 0);
	Mat4 worldCombined = worldScale * worldPos * worldRot;
	m_isHubby = false;
}

void Game::UnLoadPrevious()
{
	UnloadRoom();
}

Player* Game::GetPlayer()
{
	return m_player.get();
}

void Game::LoadRoom(Level* level)
{
	Mat4 roomMatrix =
		Mat::scale3(level->resource->worldWidth, 1.0f, level->resource->worldHeight) *
		Mat::translation3(level->position.x, level->position.x - 0.01f, level->position.x)*
		level->rotation.Matrix();

	Mat4 roomCylinderMatrix =
		Mat::scale3( level->resource->worldWidth*1.2f, 1.0f, level->resource->worldHeight * 1.2f ) *
		Mat::translation3( level->position.x, level->position.x, level->position.x ) *
		level->rotation.Matrix();

	m_currentRoom = shared_ptr<Room>(new Room(level,"room_plane.wwm", "room_walls_floor.wwm", roomMatrix, roomCylinderMatrix ));
	m_currentRoom->transform.rotation = Quaternion::GetEuler({ 0, 0, 0 });

	Renderer::LoadEnvironment(m_currentRoom->m_level);

	m_player->currentRoom = m_currentRoom.get();

	for (LevelPatrol& p : level->resource->patrolsClosed)
	{
		m_enemies.Add(shared_ptr<Enemy>(new Enemy(
			"Carcinian_Animated.wwm", 
			"Carcinian_Animations.wwa", 
			Mat::scale3(1.25f, 1.25f, 1.25f) * 
			Mat::translation3(0, 0, 0) * 
			Mat::rotation3(cs::c_pi * -0.5f, 0, 0))));
		
		for (int j = 0; j < p.controlPoints.Size(); j++)
		{
			Point2 bitPos = Point2(p.controlPoints[j]);
			Vec3 enemyPos = m_currentRoom->bitMapToWorldPos(bitPos);
			m_enemies.Back()->AddCoordinateToPatrolPath(Vec2(enemyPos.x, enemyPos.z), true);
		}
	} 

	for (LevelPatrol& p : level->resource->patrolsOpen)
	{
		m_enemies.Add(shared_ptr<Enemy>(new Enemy(
			"Carcinian_Animated.wwm",
			"Carcinian_Animations.wwa",
			Mat::scale3(1.25f, 1.25f, 1.25f) *
			Mat::translation3(0, 0, 0) *
			Mat::rotation3(cs::c_pi * -0.5f, 0, 0))));

		for (int j = 0; j < p.controlPoints.Size(); j++)
		{
			Point2 bitPos = Point2(p.controlPoints[j]);
			Vec3 enemyPos = m_currentRoom->bitMapToWorldPos(bitPos);
			m_enemies.Back()->AddCoordinateToPatrolPath(Vec2(enemyPos.x, enemyPos.z), false);
		}
	}
}

void Game::UnloadRoom()
{
	Renderer::UnLoadEnvironment();
	m_player->currentRoom = nullptr;
	m_currentRoom = nullptr;
	m_enemies.Clear();
}

bool Game::IsDetected(float deltaTime)
{
	float rate = m_detectionRate;
	if (Input::Get().IsKeybindDown(KeybindCrouch)) // is crouching 
	{
		rate = rate * 0.75f;
	}
	else if (Input::Get().IsKeybindDown(KeybindSprint)) // is running
	{
		rate = rate * 1.3f;
	}

	m_detectionLevelGlobal += rate * deltaTime;
	m_detectionLevelFloor += (rate / 3) * deltaTime;

	if (m_detectionLevelGlobal >= 1.0f)
	{
		return true; //game over
	}

	return false; // game is not over
}

void Game::LowerToFloor(float deltaTime)
{
	if (m_detectionLevelGlobal > m_detectionLevelFloor)
	{
		m_detectionLevelGlobal -= (m_detectionRate / 2) * deltaTime;
	}
	if (m_detectionLevelGlobal < m_detectionLevelFloor)
	{
		m_detectionLevelGlobal = m_detectionLevelFloor;
	}
}

void Game::ChangeTimeline(Renderer* renderer)
{
	m_isInFuture = !m_isInFuture;
	renderer->SetTimelineState(m_isInFuture);

	for (int i = 0; i < m_enemies.Size(); i++)
	{
		if (m_enemies[i]->m_enemyAlive == true)
		{
			m_enemies[i]->ChangeTimelineState(m_isInFuture);
		}
	}
}

void Game::UpdateTimeSwitchBuffers(Renderer* renderer)
{
	renderer->GetRenderCore()->WriteTimeSwitchInfo(
		m_switchVals.timeSinceSwitch,
		m_switchVals.chargeDuration,
		m_switchVals.falloffDuration
	);
}

bool Game::IsAllowedToSwitch()
{
	bool isAllowed = true;

	// Not allowed if under cooldown.
	isAllowed &= m_switchVals.timeSinceSwitch < m_switchVals.switchCooldown;

	// Allowed if NOT currently charging.
	isAllowed &= !m_isSwitching;

	return isAllowed;
}

bool Game::ChargeIsDone()
{
	return m_switchVals.timeSinceSwitch >= m_switchVals.chargeDuration;
}

bool Game::SwitchIsDone()
{
	return m_switchVals.timeSinceSwitch >= (m_switchVals.chargeDuration + m_switchVals.falloffDuration);
}
