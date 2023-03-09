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
	m_isCutscene(false),
	m_isSeen(false)

{
}

Game::~Game() {}

// Stamina, pickups, detection etc
void Game::UpdateGameplayVars( Renderer* renderer )
{
	if (m_deathEnemy == true)
	{
		m_deathEnemy = false;
		m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
		EndRunDueToEnemy(renderer);
		activeTutorialLevel = 8;
	}
	else if (m_deathPoison == true)
	{
		m_deathPoison = false;
		m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
		EndRunDueToPoison(renderer);
		activeTutorialLevel = 8;
	}

	m_coolDownCounter += m_deltaTime * (m_godMode ? 5.0f : 1.0f);
	showTextForPickupBloom = false;
	// Player vars
	m_player->SetGodMode(m_godMode);
	m_player->playerInFuture = m_isInFuture;
	m_player->UpdateStamina( m_maxStamina );
	m_currentStamina = m_player->GetCurrentStamina();
	Renderer::SetPlayerMatrix( m_player->compassMatrix );

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
	m_maxStamina -= 
		m_deltaTime * STAMINA_DECAY_MULTIPLIER * m_isInFuture * m_finishedCharging * 
		(m_godMode ? 0.05f : 1.0f); // (duplicated code thing)
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
			m_deathPoison = true;
		}
	}
	if (!m_isInFuture)
	{
		m_dangerousTimeInFuture = 0.0f;
	}


	if (!m_enemies.Empty())
	{
		// Detection logic
		if (m_isSeen)
		{
			m_timeUnseen = 0.0f;

			if (!m_godMode && IsDetected(m_deltaTime, m_closestDistance, m_enemies[0]->GetMaxDistance()))
			{
				// D E A T H
				m_deathEnemy = true;
			}
		}
		else
		{
			m_timeUnseen += m_deltaTime;
			if (m_timeUnseen > m_timeBeforeDetectionLowers)
			{
				LowerToFloor(m_deltaTime);
			}
		}
	}
}

// Transform updates (Except enemies, which are updated in UpdateEnemies())
void Game::UpdateGameObjects()
{
	m_player->Update( m_deltaTime );
	m_grafiki->Update(m_deltaTime);
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
	m_closestDistance = FLT_MAX; // large start value to fix the above thing instead of branching.
	for (int i = 0; i < m_enemies.Size(); i++)
	{
		m_enemies[i]->Update( m_deltaTime ); // Would ideally want to put this in UpdateGameObjects(), but this makes one less loop
		if (m_enemies[i]->SeesPlayer( Vec2( m_player->transform.worldPosition.x, m_player->transform.worldPosition.z ), *m_currentRoom, m_isInFuture ))
		{
			m_isSeen = true;
			if (m_enemies[i]->GetDistance() < m_closestDistance)
			{
				m_closestDistance = m_enemies[i]->GetDistance();
			}
		}

		m_enemies[i]->EnemySoundUpdate(m_deltaTime, Vec2(m_player->transform.worldPosition.x, m_player->transform.worldPosition.z), m_detectionLevelGlobal);
	}
}

// Time logic and Detection logic
void Game::UpdateRoomAndTimeSwappingLogic( Renderer* renderer )
{
	if (m_loadNewFloor)
	{
		m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
		LoadGame(1, 9);
		m_player->hasPickedUpEssenceBloom = false;
		tutorial = false;
		m_loadNewFloor = false;
		return;
	}

	// Time switch logic.
	if (!m_isHubby) // if not in hubby
	{
		
		if (Input::Get().IsKeyPressed( KeybindPower ) && IsAllowedToSwitch())
		{
			if (tutorial && activeTutorialLevel < 6)
			{
				// no power yet lmao
			}
			else
			{
				m_isSwitching = true;
				m_finishedCharging = false;
				m_initialCamFov = renderer->GetCamera().GetFov();
				m_player->m_switchSource->Play();
			}
		}

		if (m_isSwitching)
		{
			
			if (!ChargeIsDone())
			{
				float fovSpeed = (m_timeSwitchTargetFov - m_initialCamFov) / m_switchVals.chargeDuration;
				// Makes sure that the difference results in a positive speed.
				if (fovSpeed >= 0.0f)
				{
					Camera& cam = renderer->GetCamera();
					cam.SetFov(cam.GetFov() + fovSpeed * m_deltaTime);
				}			
			}
			else
			{
				if (!m_finishedCharging)
				{
					SwapTimeline( renderer );
					m_finishedCharging = true;
					renderer->GetCamera().SetFov( m_initialCamFov );

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

		m_testTunnel = false;

		if (!m_isSwitching && !m_isInFuture)
		{
			for (const LevelTunnelRef& r : m_currentRoom->m_level->connections)
			{
				Vec3 sideDir = Vec3(r.direction.z, 0.0f, -r.direction.x);
				Vec3 relative = m_player->transform.worldPosition - r.position;

				if (relative * r.direction > -TUNNEL_TRIGGER_DISTANCE && abs(relative * sideDir) < r.width * 1.1f)
				{
					/*m_testTunnel = true;*/

					// Go to next room
					if (r.targetRoom >= 0)
					{
						activeTutorialLevel = r.targetRoom + 1;
						uint targetIndex = (r.tunnelSubIndex + 1) % 2;
						const LevelTunnel& t = m_floor.tunnels[r.tunnel];
						UnLoadPrevious();
						LoadRoom(&m_floor.rooms[r.targetRoom]);

						m_currentRoom->transform.CalculateWorldMatrix();
						m_directionalLight->transform.parent = &m_currentRoom->transform;
						m_directionalLight->Update( 0 );

						MovePlayer(t.positions[targetIndex] - t.directions[targetIndex] * TUNNEL_SPAWN_DISTANCE, -t.directions[targetIndex]);
						m_player->ReloadPlayer();

						Renderer::ExecuteShadowRender();

						
						break;
					}

					// Floor entrance
					if (r.targetRoom == -1)
					{
						
					}
					
					// Floor exit 
					if (r.targetRoom == -2)  
					{
						if (m_player->hasPickedUpEssenceBloom && tutorial)
						{
							m_detectionLevelFloor = 0.0f; // resets detection
							m_loadNewFloor = true;
							activeTutorialLevel = 8;
							m_loadScreen->GetElement(0)->uiRenderable->enabled = true;
							
						}
						else if (!m_player->hasPickedUpEssenceBloom)
						{
							//show text for not picking up essence bloom yet
							showTextForPickupBloom = true;
						}
						else if (m_player->hasPickedUpEssenceBloom && !tutorial)
						{
							// you win!
							m_loadScreen->GetElement(1)->uiRenderable->enabled = true;
							youWin = true;
						}
						
					}
				}
			}
		}

		if (m_loadingHubby == true)
		{
			EndRun(renderer);
			m_loadingHubby = false;
			m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
		}
		if (Input::Get().IsDXKeyPressed( DXKey::H ))
		{
			m_loadingHubby = true;
			tutorial = false;
			activeTutorialLevel = 8;
			m_loadScreen->GetElement(0)->uiRenderable->enabled = true;
		}
	}
	else // If in hubby
	{
		if (m_loadingHubby == true)
		{
			m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
			LoadHubby();
			m_loadingHubby = false;
		}
		else if (m_loadingTutorial == true)
		{
			m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
			m_loadingTutorial = false;
			m_player->hasPickedUpEssenceBloom = false;
			LoadTutorial();
			tutorial = true;
			activeTutorialLevel = 1;
		}



		if (m_grafiki->InteractPlayer(Vec2(m_player->transform.worldPosition.x, m_player->transform.worldPosition.z)))
		{
			m_grafiki->enabled = false;
			m_loadScreen->GetElement(2)->uiRenderable->enabled = true;
			m_skipTutorialQuestion = true;
		}
		else if (Input::Get().IsDXKeyPressed( DXKey::H ))
		{
			m_loadScreen->GetElement(0)->uiRenderable->enabled = true;
			m_loadingHubby = true;
			activeTutorialLevel = 8;
			tutorial = false;
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
		ImGui::Text( "Time ability cooldown: %f", GetPowerCooldown());
		ImGui::Checkbox( "Future", &m_isInFuture );
		
		ImGui::Separator();
		ImGui::Checkbox("God Mode", &m_godMode);
		ImGui::InputFloat3("Player Position", (float*)&m_player->transform.position);
		ImGui::Text("Transition: %s", m_testTunnel ? "YES" : "NO");

		if (m_isHubby)
		{
			ImGui::Separator();
			ImGui::InputFloat3("Fog Focus", (float*)&m_fogFocus);
			ImGui::DragFloat("Fog Radius", &m_fogRadius, 0.1f, 0.1f, 100.0f);
			Renderer::SetFogParameters(m_fogFocus, m_fogRadius);
		}
	}
	ImGui::End();

	static Vec3 tempRot;
	if (ImGui::Begin( "Room Rot" ))
	{
		ImGui::DragFloat3( "Room Additive Rot", (float*)&tempRot );
		m_player->currentRoom->m_testOffset = Quaternion::GetEuler( tempRot ).Matrix();
	}
	ImGui::End();

	static Vec2 vignette = Vec2(0.5f, 1.0f);
	static Vec2 contrast = Vec2(1.0f, 0.4f);
	static float brightness = 0.0f;
	static float saturation = 1.25f;
	static bool firstSet = true;

	if (ImGui::Begin("Color Settings"))
	{
		float speed = 0.01f;
		bool changed = false;
		changed |= ImGui::DragFloat2("Vignette Radius & Strength", (float*)&vignette, speed, 0.0f, FLT_MAX);
		changed |= ImGui::DragFloat2("Contrast Amount & Midpoint", (float*)&contrast, speed, 0.0f);
		changed |= ImGui::DragFloat("Brightness", &brightness, speed, 0.0f, FLT_MAX);
		changed |= ImGui::DragFloat("Saturation", &saturation, speed, 0.0f, FLT_MAX);

		if (changed || firstSet)
		{
			firstSet = false;
			Renderer::UpdatePPFXInfo(vignette, contrast, brightness, saturation);
		}

		ImGui::Separator();
		ImGui::ColorEdit3("Ambient Color", (float*)&m_ambientColor);
		ImGui::DragFloat("Ambient Intensity", &m_ambientIntensity, 0.01f, 0.0f, 1.0f);
		ImGui::ColorEdit3("Dir Color", (float*)&m_directionalColor);
		ImGui::DragFloat("Dir Intensity", (float*)&m_directionalIntensity, 0.01f, 0.0f, 4.0f);

		ImGui::Spacing();
		ImGui::ColorEdit3("Future Ambient Color", (float*)&m_futureAmbientColor);
		ImGui::DragFloat("Future Ambient Intensity", &m_futureAmbientIntensity, 0.01f, 0.0f, 1.0f);
		ImGui::ColorEdit3("Future Dir Color", (float*)&m_futureDirectionalColor);
		ImGui::DragFloat("Future Dir Intensity", (float*)&m_futureDirectionalIntensity, 0.01f, 0.0f, 4.0f);
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

			for (int i = 0; i < f.rooms.Size(); i++)
			{
				Level& l = f.rooms[i];
				m_testRenderables.Add( Renderer::CreateMeshStatic( "room_plane.wwm" ) );
				m_testRenderables.Back()->worldMatrix = Mat::translation3( l.position.x * distance, 3.2f, l.position.z * distance ) * l.rotation.Matrix() * Mat::scale3( 0.8f );

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


	if (m_skipTutorialQuestion)
	{
		if (Input::Get().IsDXKeyPressed(DXKey::Y)) // does not skip tutorial
		{
			m_loadScreen->GetElement(0)->uiRenderable->enabled = true;
			m_loadingTutorial = true;
			m_loadScreen->GetElement(2)->uiRenderable->enabled = false;
			m_skipTutorialQuestion = false;
		}
		else if (Input::Get().IsDXKeyPressed(DXKey::N)) // skips tutorial
		{
			m_loadScreen->GetElement(0)->uiRenderable->enabled = true;
			m_loadScreen->GetElement(2)->uiRenderable->enabled = false;
			m_loadNewFloor = true;
			m_skipTutorialQuestion = false;
			tutorial = false;
		}
		
		return;
		
	}

	if (youWin)
	{
		
		if (Input::Get().IsDXKeyPressed(DXKey::Space))
		{
			//player pressed spacebar when outside while loop
			m_loadScreen->GetElement(1)->uiRenderable->enabled = false;
			youWin = false;
			EndRun(renderer);
		}
		else
		{
			return;
		}
	}
	if (!m_isCutScene)
	{
		UpdateGameObjects();

		UpdateGameplayVars( renderer );

		UpdateEnemies( renderer );

		UpdateRoomAndTimeSwappingLogic( renderer);

		// Final steps
		UpdateTimeSwitchBuffers( renderer );
		UpdateEnemyConeBuffers( renderer );
	}
	else
	{
		CinematicUpdate();
	}

	if (m_isInFuture)
	{
		Renderer::SetAmbientLight(m_futureAmbientColor, m_futureAmbientIntensity);
		m_directionalLight->color = m_futureDirectionalColor;
		m_directionalLight->intensity = m_futureDirectionalIntensity;
	}
	else
	{
		Renderer::SetAmbientLight(m_ambientColor, m_ambientIntensity);
		m_directionalLight->color = m_directionalColor;
		m_directionalLight->intensity = m_directionalIntensity;
	}

	DrawIMGUIWindows();

	if (Input::Get().IsKeybindDown(KeybindEscMenu))
	{
		Renderer::GetWindow().CloseProgram();
	}

	SoundUpdate(deltaTime);
}

void Game::Init()
{
	m_godMode = false;

	m_loadScreen = shared_ptr<GUI> (new GUI());

	// loading screen
	m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	m_loadScreen->GetElement(0)->colorTint = Vec3(1, 1, 1);
	m_loadScreen->GetElement(0)->alpha = 1.0f;
	m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
	m_loadScreen->GetElement(0)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	m_loadScreen->GetElement(0)->firstTexture = Resources::Get().GetTexture("loadingScreen.png");


	//winning screen 
	m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	m_loadScreen->GetElement(1)->colorTint = Vec3(1, 1, 1);
	m_loadScreen->GetElement(1)->alpha = 1.0f;
	m_loadScreen->GetElement(1)->uiRenderable->enabled = false;
	m_loadScreen->GetElement(1)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	m_loadScreen->GetElement(1)->firstTexture = Resources::Get().GetTexture("winScreen.png");


	//skip tutorial screen 
	m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	m_loadScreen->GetElement(2)->colorTint = Vec3(1, 1, 1);
	m_loadScreen->GetElement(2)->alpha = 1.0f;
	m_loadScreen->GetElement(2)->uiRenderable->enabled = false;
	m_loadScreen->GetElement(2)->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	m_loadScreen->GetElement(2)->firstTexture = Resources::Get().GetTexture("skipTutorial.png");


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

	m_fogFocus = Vec3(0, 0, 0);
	m_fogRadius = 10.0f;
	Renderer::SetFogParameters(m_fogFocus, m_fogRadius);

	// Level handling
	m_levelHandler = std::make_unique<LevelHandler>();
	m_levelHandler->LoadFloors();
	

	// In-world objects and entities
	m_player = shared_ptr<Player>(new Player("Shadii_Rigged_Optimized.wwm", "Shadii_Animations.wwa", Mat::translation3(0.0f, 0.0f, 0.0f) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0)));
	m_grafiki = make_shared<Grafiki>();

	//Music
	m_musicPresent = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), m_musicVol, 1.0f, 15.0f, 20.0f, (Resources::Get().GetSound("Strange_Beings.mp3"))->currentSound);
	m_musicFuture = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f, 15.0f, 20.0f, (Resources::Get().GetSound("Strange_Beings_Who_Left.mp3"))->currentSound);
	m_musicDetected = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f, 15.0f, 20.0f, (Resources::Get().GetSound("Strange_Beings_on_Your_Tail.mp3"))->currentSound);
	m_musicPresent->loop = true;
	m_musicFuture->loop = true;
	m_musicDetected->loop = true;
	m_musicPresent->mix2d3d = 0.0f;
	m_musicFuture->mix2d3d = 0.0f;
	m_musicDetected->mix2d3d = 0.0f;

	m_enemyHorn = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), m_hornVol, 0.8f, 20.0f, 30.0f, (Resources::Get().GetSound("HornHeavyReverb.wav"))->currentSound);
	m_enemyHorn->mix2d3d = 0.0f;

	// Lighting
	dirLightOffset = Vec3( 0, 20, -20 ); // TODO: Investigate why other values here don't work. << CULLING ON THE SHADOWS WACKY
	m_directionalLight = Renderer::GetDirectionalLight();
	m_directionalLight->transform.position = dirLightOffset; 
	m_directionalLight->transform.SetRotationEuler({ dx::XM_PIDIV4, 0.0f, 0.0f }); // Opposite direction of how the light should be directed
	m_directionalLight->diameter = 45.0f;

	m_directionalIntensity = 1.25f;
	m_directionalColor = cs::Color3f(0xD0D0D0);

	m_ambientColor = cs::Color3f(0xC0C0FF);
	m_ambientIntensity = 0.26f;

	m_futureDirectionalIntensity = 2.4f;
	m_futureDirectionalColor = cs::Color3f(0xFFF4C3);

	m_futureAmbientColor = cs::Color3f(0xFFFFC0);
	m_futureAmbientIntensity = 0.35f;
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
	UnLoadPrevious();
	m_levelHandler->GenerateHubby( &m_floor, m_envParams );
	LoadRoom( &m_floor.rooms[0] );
	m_detectionLevelFloor = 0.0f;

	m_directionalLight->transform.parent = &m_currentRoom->transform;
	m_directionalLight->Update( 0 );
	m_currentRoom->transform.CalculateWorldMatrix();
	MovePlayer(Vec3(0, 0, -9.0f), Vec3(0, 0, 1));
	m_player->ReloadPlayer();

	m_isHubby = true;
	Renderer::ExecuteShadowRender();

	m_grafiki->Reload();
	m_grafiki->enabled = true;
	m_grafiki->transform.SetRotationEuler(Vec3(0.0f, DEG2RAD * 180.0f, 0.0f));
	m_currentRoom->AddChild((GameObject*)m_grafiki.get());
}

void Game::LoadTest()
{
	m_levelHandler->GenerateTestFloor(&m_floor, m_envParams);
	LoadRoom(&m_floor.rooms[0]);

	m_isHubby = false;
	Renderer::ExecuteShadowRender();
}

void Game::LoadTutorial()
{
	UnLoadPrevious();
	m_levelHandler->GenerateTutorial(&m_floor, m_envParams);
	LoadRoom(&m_floor.rooms[m_floor.startRoom]);

	m_directionalLight->transform.parent = &m_currentRoom->transform;
	m_directionalLight->Update( 0 );
	MovePlayer(m_floor.startPosition, m_floor.startDirection);
	m_currentRoom->transform.CalculateWorldMatrix();

	m_player->ReloadPlayer();

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

	UnLoadPrevious();
	m_levelHandler->GenerateFloor(&m_floor, params, m_envParams);
	LoadRoom(&m_floor.rooms[m_floor.startRoom]);
	m_currentRoom->transform.CalculateWorldMatrix();

	MovePlayer(m_floor.startPosition, m_floor.startDirection);
	m_player->ReloadPlayer();

	m_isHubby = false;
	Renderer::ExecuteShadowRender();
}

void Game::UnLoadPrevious()
{
	m_pickups.Clear();
	UnloadRoom();
}

Player* Game::GetPlayer()
{
	return m_player.get();
}

void Game::MovePlayer(Vec3 position, Vec3 direction)
{
	m_player->transform.position = position;

	m_player->ResetCamera(direction);
	
	Camera& c = Renderer::GetCamera();
	c.SetPosition(m_player->cameraFollowTarget);
	c.SetRotation(m_player->cameraLookRotationTarget);
}

void Game::SetCutSceneMode( bool value )
{
	m_isCutScene = value;
}

float Game::GetPowerCooldown()
{
	float cd = m_timeAbilityCooldown - m_coolDownCounter;
	if (cd < 0)
	{
		cd = 0;
	}
	return cd;
}

float Game::GetMaxPowerCooldown()
{
	return m_timeAbilityCooldown;
}

float Game::GetMaxStamina()
{
	return m_maxStamina;
}

void Game::GodMode(bool godMode)
{
	m_godMode = godMode;
}

void Game::LoadRoom(Level* level)
{
	Mat4 roomOffset =
		Mat::translation3(0, -0.01f, 0) *
		Mat::scale3(level->resource->worldWidth, 1.0f, level->resource->worldHeight);

	Mat4 cylinderOffset =
		Mat::translation3( 0, -0.02f, 0 ) *
		Mat::scale3(level->resource->worldWidth * 1.3f, 1.0f, level->resource->worldHeight * 1.6f);

	m_currentRoom = shared_ptr<Room>(new Room(level, "room_plane.wwm", "room_walls_floor.wwm", roomOffset, cylinderOffset ));
	m_currentRoom->transform.position = level->position;
	m_currentRoom->transform.rotation = level->rotation;

	m_currentRoom->transform.CalculateWorldMatrix();
	m_directionalLight->transform.parent = &m_currentRoom->transform;
	m_directionalLight->Update(0);

	Renderer::SetFogParameters(level->position, level->resource->worldWidth * 0.55f);
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

	return m_detectionLevelGlobal >= 1.0f;
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

void Game::SoundUpdate(float deltaTime)
{
	m_musicPresent->Update(deltaTime);
	m_musicFuture->Update(deltaTime);
	m_musicDetected->Update(deltaTime);

	if (!m_musicPresent->IsPlaying())
	{
		if (!m_isHubby)
		{
			m_musicPresent->Play();
			m_musicFuture->Play();
			m_musicDetected->Play();

			m_musicPresent->SetVolume(m_musicVol);
			m_musicFuture->SetVolume(0.0f);
			m_musicDetected->SetVolume(0.0f);
		}
	}
	else
	{
		if (m_isHubby)
		{
			m_musicPresent->Stop();
			m_musicFuture->Stop();
			m_musicDetected->Stop();
		}

		if (m_isInFuture)
		{
			m_musicPresent->SetVolume(0.0f);
			m_musicFuture->SetVolume(m_musicVol * 0.7f);
			m_musicDetected->SetVolume(0.0f);
		}
		else
		{
			m_musicPresent->SetVolume(m_musicVol);
			m_musicFuture->SetVolume(0.0f);
			float temp = m_detectionLevelGlobal / 0.75f;
			m_musicDetected->SetVolume(m_musicVol * (temp > 1.0f ? 1.0f : temp));
		}
	}
}

void Game::ResetGameplayValues()
{
	m_isInFuture = false;
	m_isSwitching = false;
	m_isSeen = false;
	m_reachedLowestStamina = false;

	m_switchVals.timeSinceSwitch = 0.0f;
	m_timeUnseen = 0.0f;
	m_detectionLevelGlobal = 0.0f;
	m_detectionLevelFloor = 0.0f;
	m_dangerousTimeInFuture = 0.0f;
	
	m_maxStamina = MAX_STAMINA_STARTING_VALUE;
	m_coolDownCounter = m_timeAbilityCooldown;
}

void Game::EndRun(Renderer* renderer)
{
	if (m_isSwitching)
	{
		Renderer::GetCamera().SetFov(m_initialCamFov);
	}

	ResetGameplayValues();
	ChangeToPresentTimeline(renderer);
	
	LoadHubby();

	// Has to happen after loading hubby for some reason?
	m_player->ResetStaminaToMax(MAX_STAMINA_STARTING_VALUE);
}

void Game::EndRunDueToEnemy(Renderer* renderer)
{
	m_enemyHorn->Play();

	// More logic for dying from enemy here.

	EndRun(renderer);
}

void Game::EndRunDueToPoison(Renderer* renderer)
{
	// Logic for dying from future poison here.

	EndRun(renderer);
}

void Game::SwapTimeline(Renderer* renderer)
{
	m_isInFuture = !m_isInFuture;

	ApplyTimelineState(renderer);
}

void Game::ApplyTimelineState(Renderer* renderer)
{
	Renderer::SetTimelineState(m_isInFuture);
	m_currentRoom->SetTimeline(m_isInFuture);

	for (int i = 0; i < m_enemies.Size(); i++)
	{
		if (m_enemies[i]->enemyAlive == true)
		{
			m_enemies[i]->ChangeTimelineState(m_isInFuture);
		}
	}

	UpdateTimeSwitchBuffers(renderer);
}

void Game::ChangeToFutureTimeline(Renderer* renderer)
{
	m_isInFuture = true;
	ApplyTimelineState(renderer);
}

void Game::ChangeToPresentTimeline(Renderer* renderer)
{
	m_isInFuture = false;
	ApplyTimelineState(renderer);
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
