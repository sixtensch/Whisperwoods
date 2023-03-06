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
	m_maxStamina(MAX_STAMINA_STARTING_VALUE),
	m_switchVals({ 0.3f, 0.5f, 3.0f, 0.0f }),
	m_detectionLevelGlobal(0.0f),
	m_detectionLevelFloor(0.0f),
	m_camFovChangeSpeed(cs::c_pi / 2.0f),
	m_envParams({}),
	m_reachedLowestStamina(false),
	m_coolDownCounter(m_timeAbilityCooldown),
	m_isCutscene(false)
{
}

Game::~Game() {}

// Stamina, pickups, detection etc
void Game::UpdateGameplayVars( Renderer* renderer )
{
	m_coolDownCounter += m_deltaTime;

	// Player vars
	m_player->playerInFuture = m_isInFuture;
	m_player->UpdateStamina( m_maxStamina );
	m_currentStamina = m_player->GetCurrentStamina();
	Renderer::SetPlayerMatrix( m_player->transform.worldMatrix );

	// Pickups
	for (int i = 0; i < m_pickups.Size(); ++i)
	{
		m_pickups[i]->Update( m_deltaTime );
		//Did player pick up essence bloom?
		if (m_pickups[i]->IsRemovable()) // if we add more types of pickups, add &&isEssenceBloom or whatever
		{
			m_maxStamina = MAX_STAMINA_STARTING_VALUE;
			m_player->ResetStaminaToMax( m_maxStamina );
			m_pickups.Remove( i );
			i--;
		}
	}

	// Stamina Logic
	m_maxStamina -= m_deltaTime * STAMINA_DECAY_MULTIPLIER * m_isInFuture * m_finishedCharging; // (duplicated code thing)
	if (m_maxStamina < 1.0f) // DO NOT CHANGE THIS
	{
		m_maxStamina = 1.0f; // DO NOT CHANGE THIS

		if (!m_reachedLowestStamina)
		{
			m_reachedLowestStamina = true;
		}
		else
		{
			m_dangerousTimeInFuture += m_deltaTime;
		}

		/// D E A T H ///
		if (m_dangerousTimeInFuture >= m_timeYouSurviveInFuture) // how long you can survive in future with 0 stamina (seconds)
		{
			ChangeTimeline( renderer );
			m_maxStamina = MAX_STAMINA_STARTING_VALUE;
			m_player->ResetStaminaToMax( m_maxStamina );
			UnLoadPrevious();
			LoadHubby();
			m_player->ReloadPlayer();
		}
	}
	if (!m_isInFuture)
	{
		m_dangerousTimeInFuture = 0.0f;
	}

	// Detection logic
	if (m_isSeen)
	{
		m_timeUnseen = 0.0f;
		if (IsDetected( m_deltaTime, m_closestDistance, m_enemies[0]->GetMaxDistance() ))
		{
			if (m_isSwitching)
			{
				m_switchVals.timeSinceSwitch = 2.0f;
				m_isInFuture = true;
				renderer->GetCamera().SetFov( m_initialCamFov );
				ChangeTimeline( renderer );
				m_switchVals.timeSinceSwitch = 0.0f;
				m_isSwitching = false;
				m_totalFovDelta = 0.0f;
			}

			// D E A T H
			m_maxStamina = MAX_STAMINA_STARTING_VALUE;
			//m_coolDownCounter = m_timeAbilityCooldown;
			m_player->ResetStaminaToMax( m_maxStamina );
			UnLoadPrevious();
			LoadHubby();
			m_player->ReloadPlayer();
			m_isSeen = false;
			m_detectionLevelGlobal = 0.0f;
			m_detectionLevelFloor = 0.0f;
		}
	}
	else
	{
		m_timeUnseen += m_deltaTime;
		if (m_timeUnseen > m_timeBeforeDetectionLowers)
		{
			LowerToFloor( m_deltaTime );
		}
	}
}

// Transform updates (Except enemies, which are updated in UpdateEnemies())
void Game::UpdateGameObjects()
{
	m_player->Update( m_deltaTime );
	m_currentRoom->Update( m_deltaTime );
	for (int i = 0; i < m_staticObjects.Size(); i++)
	{
		m_staticObjects[i]->Update( m_deltaTime );
	}
}



// Transform and AI Logic
void Game::UpdateEnemies( Renderer* renderer )
{
	//       $$                      $$
	//     $$$  $                  $  $$$
	//    $$$   $$                $$   $$$
	//    $$$$$$$$                $$$$$$$$
	//     $$$$$$                  $$$$$$
	//      $$$$    $$0$$$$$0$$$    $$$$
	//        $$  $$$$$$$$$$$$$$$$  $$
	//    $$   $$$$$$$$$$$$$$$$$$$$$$   $$
	//  $$  $$  $$$$$$$$$$$$$$$$$$$$  $$  $$
	// $      $$$$$$$$$$$$$$$$$$$$$$$$      $
	// $  $$$    $$$$$$$$$$$$$$$$$$    $$$  $
	//   $   $$$$ $$$$$$$$$$$$$$$$ $$$$   $
	//  $         $ $$$$$$$$$$$$ $         $
	//  $      $$$                $$$      $
	//        $                      $
	//       $                        $
	//       $                         $
	// 
	//     WHAT ARE THESE COPYPASTA TIGERS?

	/*    ("`-''-/").___..--''"`-._
		   `6_ 6  )   `-.  (     ).`-.__.`)
		   (_Y_.)'  ._   )  `._ `. ``-..-'
		 _..`--'_..-_/  /--'_.' ,'
		(il),-''  (li),'  ((!.-'   ~Nom nom nom~
	//Vec2 playerPosition2D = { m_player->transform.worldPosition.x, m_player->transform.worldPosition.z }; // this was not used.
	//if (m_enemies.Size() > 0) // apperently we call update before creating enemies. Dont touch this
	//{
	//	m_closestDistance = m_enemies[0]->GetMaxDistance();
	//}	*/

	m_isSeen = false;
	m_closestDistance = 100000.0f; // large start value to fix the above thing instead of branching.
	for (int i = 0; i < m_enemies.Size(); i++)
	{
		m_enemies[i]->Update( m_deltaTime ); // Would ideally want to put this in UpdateGameObjects(), but this makes one less loop
		if (m_enemies[i]->SeesPlayer( Vec2( m_player->transform.worldPosition.x, m_player->transform.worldPosition.z ), *m_currentRoom, *m_audioSource, m_isInFuture ))
		{
			m_isSeen = true;
			if (m_enemies[i]->GetDistance() < m_closestDistance)
			{
				m_closestDistance = m_enemies[i]->GetDistance();
			}
		}
	}
}

// Time logic and Detection logic
void Game::UpdateRoomAndTimeSwappingLogic( Renderer* renderer )
{
	// Time switch logic.
	if (!m_isHubby) // if not in hubby
	{
		if (Input::Get().IsKeyPressed( KeybindPower ) && IsAllowedToSwitch())
		{
			m_isSwitching = true;
			m_finishedCharging = false;
			m_initialCamFov = renderer->GetCamera().GetFov();
		}

		if (m_isSwitching)
		{
			if (!ChargeIsDone())
			{
				m_totalFovDelta += m_camFovChangeSpeed * m_deltaTime;
				float newFov = m_initialCamFov + m_totalFovDelta;

				// Max total fov cant exceed half circle.
				if (newFov > cs::c_pi)
				{
					newFov = cs::c_pi;
				}

				renderer->GetCamera().SetFov( newFov );
			}
			else
			{
				if (!m_finishedCharging)
				{
					ChangeTimeline( renderer );
					m_finishedCharging = true;
					renderer->GetCamera().SetFov( m_initialCamFov );
					m_totalFovDelta = 0.0f;

					if (!m_isInFuture) // time to cooldown
					{
						m_coolDownCounter = 0.0f;
					}
				}
			}

			if (!SwitchIsDone())
			{
				m_switchVals.timeSinceSwitch += m_deltaTime;
			}
			else
			{
				m_switchVals.timeSinceSwitch = 0.0f;
				m_isSwitching = false;
			}
		}

		//       $$                      $$
		//     $$$  $                  $  $$$
		//    $$$   $$                $$   $$$
		//    $$$$$$$$                $$$$$$$$
		//     $$$$$$                  $$$$$$
		//      $$$$    $$0$$$$$0$$$    $$$$
		//        $$  $$$$$$$$$$$$$$$$  $$
		//    $$   $$$$$$$$$$$$$$$$$$$$$$   $$
		//  $$  $$  $$$$$$$$$$$$$$$$$$$$  $$  $$
		// $      $$$$$$$$$$$$$$$$$$$$$$$$      $
		// $  $$$    $$$$$$$$$$$$$$$$$$    $$$  $
		//   $   $$$$ $$$$$$$$$$$$$$$$ $$$$   $
		//  $         $ $$$$$$$$$$$$ $         $
		//  $      $$$                $$$      $
		//        $                      $
		//       $                        $
		//       $                         $
		// 
		//     WHAT ARE THESE COPYPASTA TIGERS?

		/*    ("`-''-/").___..--''"`-._
			   `6_ 6  )   `-.  (     ).`-.__.`)
			   (_Y_.)'  ._   )  `._ `. ``-..-'
			 _..`--'_..-_/  /--'_.' ,'
			(il),-''  (li),'  ((!.-'   ~I think this is duplicated code~
		m_maxStamina -= m_deltaTime * STAMINA_DECAY_MULTIPLIER * m_isInFuture * m_finishedCharging;
		if (m_maxStamina < 1.0f)
		{
			m_maxStamina = 1.0f;
		}*/

		if (!m_isSwitching && !m_isInFuture)
		{
			//for (const LevelExitRef& : m_floor[room])
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
			LoadGame(0, 9);
			
			m_player->ReloadPlayer();
		}
		if (Input::Get().IsDXKeyPressed( DXKey::H ))
		{
			UnLoadPrevious();
			LoadHubby();
			m_player->ReloadPlayer();
		}
	}
}

// Debug stuff
void Game::DrawIMGUIWindows()
{
	#if WW_DEBUG

	// Gameplay variables window
	if (ImGui::Begin( "Gameplay Vars" ))
	{
		ImGui::Text( "Max Stamina: %f", m_maxStamina );
		ImGui::Text( "Current Stamina: %f", m_currentStamina );
		ImGui::DragFloat( "Detection Level Global", &m_detectionLevelGlobal, 0.1f, 0.0f, 1.0f );
		ImGui::Text( "Detection level Floor: %f", m_detectionLevelFloor );
		ImGui::Text( "Time left until future death: %f", m_timeYouSurviveInFuture - m_dangerousTimeInFuture );
		float cd = m_timeAbilityCooldown - m_coolDownCounter;
		if (cd < 0)
		{
			cd = 0;
		}
		ImGui::Text( "Time ability cooldown: %f", cd );
		ImGui::Checkbox( "Future", &m_isInFuture );
		
		ImGui::Separator();
		ImGui::InputFloat3("Player Position", (float*)&m_player->transform.position);
	}
	ImGui::End();

	// Environment generation variables window
	if (ImGui::Begin( "Environment Parameters" ))
	{
		ImGui::DragInt( "Spawn Seed", &m_envParams.spawnSeed, 0.1f );
		ImGui::DragInt( "Scale Seed", &m_envParams.scaleSeed, 0.1f );
		ImGui::DragInt( "Rotation Seed", &m_envParams.rotationSeed, 0.1f );
		ImGui::DragInt( "Diversity Seed", &m_envParams.diversitySeed, 0.1f );
		ImGui::DragFloat( "Rotation Multiplier", &m_envParams.rotateMult, 0.01f );
		ImGui::DragFloat( "X Multiplier", &m_envParams.xMult, 0.01f );
		ImGui::DragFloat( "Y Multiplier", &m_envParams.yMult, 0.01f );
		ImGui::DragFloat( "Scale base", &m_envParams.scaleBase, 0.01f );
		ImGui::DragFloat( "Scale multiplier", &m_envParams.scaleMult, 0.01f );
		ImGui::DragFloat( "Density Unwalkable Outer", &m_envParams.densityUnwalkableOuter, 0.01f );
		ImGui::DragFloat( "Density Unwalkable Inner", &m_envParams.densityUnwalkableInner, 0.01f );
		ImGui::DragFloat( "Density Walkable", &m_envParams.densityWalkable, 0.01f );
		ImGui::DragFloat( "Min Density Spawn", &m_envParams.minDensity, 0.01f );
		ImGui::DragFloat( "Scale multiplier Sones", &m_envParams.scaleMultiplierStones, 0.01f );
		ImGui::DragFloat( "Scale multiplier Trees", &m_envParams.scaleMultiplierTrees, 0.01f );
		ImGui::DragFloat( "Scale multiplier Foliage", &m_envParams.scaleMultiplierFoliage, 0.01f );
		ImGui::DragFloat( "Scale Effect Density", &m_envParams.scaleEffectDensity, 0.01f );
		ImGui::DragInt( "Edge Distance Trunks", &m_envParams.edgeSampleDistanceTrunks, 0.1f );
		ImGui::DragInt( "Edge Distance Trees", &m_envParams.edgeSampleDistanceTrees, 0.1f );
		ImGui::DragInt( "Edge Distance Stones", &m_envParams.edgeSampleDistanceStones, 0.1f );
	}
	ImGui::End();

	// Map generation window
	if (ImGui::Begin( "Map Debugger" ))
	{
		ImGui::InputInt( "Room Count", &m_testCount );
		ImGui::InputInt( "Separation Steps", &m_testRep );
		bool gen = false;
		if (ImGui::Button( "Generate" ))
		{
			gen = true;
		}

		ImGui::SameLine();
		if (ImGui::Button( "Generate New" ))
		{
			gen = true;
			m_testSeed++;
		}

		if (gen)
		{
			LevelFloor f;
			m_levelHandler->GenerateFloor( &f, { m_testSeed, (uint)m_testCount, (uint)m_testRep, 0 }, m_envParams );

			for (int i = 0; i < m_testRenderables.Size(); i++)
			{
				Renderer::DestroyMeshStatic( m_testRenderables[i] );
			}

			m_testRenderables.Clear();
			m_testMaterials.Clear();
			for (int i = 0; i < f.rooms.Size(); i++)
			{
				m_testMaterials.Add( MaterialResource() );
			}

			float distance = 0.5f / (BM_MAX_SIZE / BM_PIXELS_PER_UNIT);

			Mat4 halfturn = Quaternion::GetEuler( 0, cs::c_pi, 0 ).Matrix();

			for (int i = 0; i < f.rooms.Size(); i++)
			{
				Level& l = f.rooms[i];
				m_testRenderables.Add( Renderer::CreateMeshStatic( "room_plane.wwm" ) );
				m_testRenderables.Back()->worldMatrix = Mat::translation3( l.position.x * distance, 3.2f, l.position.z * distance ) * halfturn * l.rotation.Matrix() * Mat::scale3( 0.8f );

				m_testMaterials[i].specular = Vec3( 0.5f, 0.5f, 0.5f );
				m_testMaterials[i].textureDiffuse = l.resource->source;
				m_testRenderables.Back()->Materials().AddMaterial( &m_testMaterials[i] );
			}

			for (int i = 0; i < f.tunnels.Size(); i++)
			{
				LevelTunnel& t = f.tunnels[i];
				Level& start = f.rooms[t.startRoom];
				Level& end = f.rooms[t.endRoom];

				m_testRenderables.Add( Renderer::CreateMeshStatic( "Debug_Sphere.wwm" ) );
				m_testRenderables.Back()->worldMatrix =
					Mat::translation3( (start.position.x + end.position.x) * 0.5f * distance, 3, (start.position.z + end.position.z) * 0.5f * distance ) *
					Quaternion::GetDirection( (Vec3( end.position.x, 0, end.position.z ) - Vec3( start.position.x, 0, start.position.z )).Normalized() ).Matrix() *
					Mat::scale3( 0.3f, 0.3f, (end.position - start.position).Length() * 5.0f * distance);
			}
		}
	}
	ImGui::End();


	#endif
	}


void Game::CinematicUpdate()
{
	// More later
	m_player->CinematicUpdate( m_deltaTime ); // Only updates the matrix, allowing for cutscenecontroller control.
	m_currentRoom->Update( m_deltaTime );
	for (int i = 0; i < m_staticObjects.Size(); i++)
	{
		m_staticObjects[i]->Update( m_deltaTime );
	}
}

// Main update function.
void Game::Update(float deltaTime, Renderer* renderer)
{
	m_deltaTime = deltaTime;

	if (!m_isCutScene)
	{
		UpdateGameObjects();

		UpdateGameplayVars( renderer );

		UpdateEnemies( renderer );

		UpdateRoomAndTimeSwappingLogic( renderer );

		// Final steps
		UpdateTimeSwitchBuffers( renderer );
		UpdateEnemyConeBuffers( renderer );
	}
	else
	{
		CinematicUpdate();
	}

	DrawIMGUIWindows();

	if (Input::Get().IsKeybindDown(KeybindEscMenu))
	{
		Renderer::GetWindow().CloseProgram();
	}
}

void Game::Init()
{
	// Audio test startup
	FMOD::Sound* soundPtr = (Resources::Get().GetSound("Duck.mp3"))->currentSound;
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
	m_envParams.densityUnwalkableOuter = 0.100f;
	m_envParams.densityUnwalkableInner = 0.200f;
	m_envParams.densityWalkable = 0.15f;
	m_envParams.minDensity = 0.25f;
	m_envParams.scaleMultiplierStones = 0.4f;
	m_envParams.scaleMultiplierTrees = 0.2f;
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

	// Lighting
	m_directionalLight = Renderer::GetDirectionalLight();
	m_directionalLight->transform.position = { 0, 10, -10 };
	m_directionalLight->transform.SetRotationEuler({ -dx::XM_PIDIV4, 0.0f, 0.0f }); // Opposite direction of how the light should be directed
	m_directionalLight->diameter = 50.0f;
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
	m_player->transform.position = Vec3(0, 0, 0);
	Renderer::ExecuteShadowRender();
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
	Renderer::ExecuteShadowRender();
}

void Game::LoadGame(uint gameSeed, uint roomCount)
{
	FloorParameters params = {};
	params.seed = gameSeed;
	params.roomCount = roomCount;
	params.angleSteps = 0;
	params.pushSteps = 3;

	m_levelHandler->GenerateFloor(&m_floor, params, m_envParams);
	LoadRoom(&m_floor.rooms[m_floor.startRoom]);

	m_player->transform.position = m_floor.startPosition;

	m_isHubby = false;
	Renderer::ExecuteShadowRender();
}

void Game::UnLoadPrevious()
{
	UnloadRoom();
}

Player* Game::GetPlayer()
{
	return m_player.get();
}

void Game::SetCutSceneMode( bool value )
{
	m_isCutScene = value;
}

void Game::LoadRoom(Level* level)
{
	Mat4 roomOffset =
		Mat::translation3(0, -0.01f, 0) *
		Mat::scale3(level->resource->worldWidth, 1.0f, level->resource->worldHeight);

	Mat4 cylinderOffset =
		Mat::translation3( 0, -0.02f, 0 ) *
		Mat::scale3(level->resource->worldWidth * 1.2f, 1.0f, level->resource->worldHeight * 1.2f);

	m_currentRoom = shared_ptr<Room>(new Room(level, "room_plane.wwm", "room_walls_floor.wwm", roomOffset, cylinderOffset ));
	m_currentRoom->transform.position = level->position;
	m_currentRoom->transform.rotation = level->rotation;

	//Vec3 test = level->rotation * Vec3(0, 0, 1);

	Renderer::LoadEnvironment(m_currentRoom->m_level);

	m_player->currentRoom = m_currentRoom.get();

	for ( LevelPickup& pickup : level->resource->pickups )
	{
		Vec3 worldpos = m_player->currentRoom->bitMapToWorldPos(static_cast<Point2>(pickup.position));
		shared_ptr<Pickup> item = make_shared<EssenceBloom>(m_player.get(), Vec2(worldpos.x, worldpos.z));
		m_pickups.Add(item);
	}

	for (LevelPatrol& p : level->resource->patrolsClosed)
	{
		m_enemies.Add(shared_ptr<Enemy>(new Enemy(
			"Carcinian_Animated.wwm", 
			"Carcinian_Animations.wwa", 
			Mat::translation3(0, 0, 0) * 
			Mat::scale3(1.25f, 1.25f, 1.25f) * 
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
			Mat::translation3(0, 0, 0) *
			Mat::scale3(1.25f, 1.25f, 1.25f) *
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
	m_currentRoom.reset();
	m_enemies.Clear();
}

bool Game::IsDetected(float deltaTime, float enemyDistance, float maximalDistance)
{
	float rate = m_detectionRate;
	//if (Input::Get().IsKeybindDown(KeybindCrouch)) // is crouching
	//{
	//	rate = rate * m_detCrouchMultiplier;
	//}
	//else if (Input::Get().IsKeybindDown(KeybindSprint)) // is running
	//{
	//	rate = rate * m_detSprintMultiplier;
	//}

	// Alternate version that checks the player state instead of input state, should be the same result.
	if (m_player->IsCrouching()) // is crouching (player:m_crouch = true, used for animation etc)
	{
		rate = rate * m_detCrouchMultiplier;
	}
	else if (m_player->IsRunning()) // is running = (player speed (velocity.length) is more than m_walkingSpeed), could also use the speed as multiplier on the detection rate
	{
		rate = rate * m_detSprintMultiplier;
	}

	float distanceRate = enemyDistance / maximalDistance; // this goes from 0-1  where 1 is very far away and 0 is right on top of the enemy
	distanceRate = (1.0f - distanceRate) * m_detDistMultiplier;
	distanceRate = distanceRate * distanceRate * distanceRate;

	rate = rate + distanceRate;
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
		if (m_enemies[i]->enemyAlive == true)
		{
			m_enemies[i]->ChangeTimelineState(m_isInFuture);
		}
	}
	UpdateTimeSwitchBuffers(renderer);
}

void Game::UpdateTimeSwitchBuffers(Renderer* renderer)
{
	renderer->GetRenderCore()->WriteTimeSwitchInfo(
		m_switchVals.timeSinceSwitch,
		m_switchVals.chargeDuration,
		m_switchVals.falloffDuration,
		m_isInFuture,
		m_detectionLevelGlobal
	);
	
}

void Game::UpdateEnemyConeBuffers(Renderer* renderer)
{
	renderer->GetRenderCore()->WriteEnemyConeInfo(m_enemies);
}

bool Game::IsAllowedToSwitch()
{
	bool isAllowed = true;

	// Not allowed if under cooldown.
	isAllowed &= m_switchVals.timeSinceSwitch < m_switchVals.switchCooldown;

	// Allowed if NOT currently charging.
	isAllowed &= !m_isSwitching;

	isAllowed &= m_coolDownCounter > m_timeAbilityCooldown;

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
