#include "core.h"
#include "Game.h"
#include "LevelImporter.h"
#include "LevelHandler.h"
#include "SoundResource.h"
#include "Resources.h"
#include "WWCBuilder.h"
#include "Input.h"
#include <imgui.h>


Game::Game() :
	m_floor(),
	m_isHubby( false ),
	m_currentRoom( nullptr ),
	m_isInFuture( false ),
	m_isSwitching( false ),
	m_finishedCharging( false ),
	m_maxStamina( MAX_STAMINA_STARTING_VALUE ),
	m_switchVals( { 0.3f, 0.5f, 3.0f, 0.0f } ),
	m_detectionLevelGlobal( 0.0f ),
	m_detectionLevelFloor( 0.0f ),
	m_camFovChangeSpeed( cs::c_pi / 2.0f ),
	m_envParams( {} ),
	m_reachedLowestStamina( false ),
	m_coolDownCounter( m_timeAbilityCooldown ),
	m_isCutscene( false ),
	m_isSeen( false ),
	noiseVal1( 0, 0, 0 ),
	noiseVal2( 1, 1, 1 ),
	m_initialCamFov(0.0f)
{
}

Game::~Game() {}

// Stamina, pickups, detection etc
void Game::UpdateGameplayVars( Renderer* renderer )
{
	if (m_deathEnemy == true && m_deathTransition == false)
	{
		m_deathEnemy = false;
		m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
		EndRunDueToEnemy(renderer);
		activeTutorialLevel = 8;
	}
	else if (m_deathPoison == true && m_deathTransition == false)
	{
		m_deathPoison = false;
		m_loadScreen->GetElement(0)->uiRenderable->enabled = false;
		EndRunDueToPoison(renderer);
		activeTutorialLevel = 8;
	}

	m_coolDownCounter += m_deltaTime * (m_godMode ? 5.0f : 1.0f);
	
	// Player vars
	m_player->SetGodMode(m_godMode);
	m_player->playerInFuture = m_isInFuture;
	m_player->UpdateStamina( m_maxStamina );
	m_currentStamina = m_player->GetCurrentStamina();
	Renderer::SetPlayerMatrix( m_player->compassMatrix );

	cs::NoiseSimplex noise1( 2 );
	cs::NoiseSimplex noise2( 123 );
	// Noise Generation for breathing glows and vegetation wind TODO: Check performance on this, possibly remove if bad.
	float scanMultiplier = 0.1f;
	noiseVal1 += Vec3( m_deltaTime * scanMultiplier*5, -m_deltaTime* scanMultiplier, m_deltaTime * scanMultiplier );
	noiseVal2 += Vec3( m_deltaTime * scanMultiplier*2, m_deltaTime * scanMultiplier, m_deltaTime * scanMultiplier*3);
	Vec4 noiseVector = Vec4(
		noise1.Gen1D( noiseVal1.x ), 
		noise2.Gen1D( noiseVal1.x ), 
		noise1.Gen2D( noiseVal1.y, noiseVal1.x ), 
		noise1.Gen2D( noiseVal1.z, noiseVal1.y ));
	Renderer::SetWorldParameters( noiseVector, Vec4( noiseVal2, 0));
#if WW_IMGUI 1
	//if (ImGui::Begin( "Noise Debug" ))
	//{
	//	ImGui::Text( "NoiseVector: %f %f %f %f", noiseVector.x, noiseVector.y, noiseVector.z, noiseVector.w );
	//	ImGui::Text( "NoiseVal1: %f %f %f", noiseVal1.x, noiseVal1.y, noiseVal1.z );
	//	ImGui::Text( "NoiseVal2: %f %f %f", noiseVal2.x, noiseVal2.y, noiseVal2.z );
	//}
	//ImGui::End();
#endif
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
				m_player->m_switchSource->SetPitch(1.5f);
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
					if (m_deathTransition)
					{
						EndRun(renderer);
					}
					else
					{
						SwapTimeline(renderer);
						m_finishedCharging = true;
						renderer->GetCamera().SetFov(m_initialCamFov);

						if (!m_isInFuture) // time to cooldown
						{
							m_coolDownCounter = 0.0f;
						}
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
		showTextForPickupBloom = false;
		m_testTunnel = false;
		m_loadScreen->GetElement(3)->uiRenderable->enabled = false;

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
						//ExecuteLoad(r.targetRoom, t.positions[targetIndex], t.directions[targetIndex]);
						TransitionStart(r.position, r.direction, r.targetRoom, t.positions[targetIndex], t.directions[targetIndex]);
						break;
					}

					// Floor entrance
					if (r.targetRoom == -1)
					{
						m_loadScreen->GetElement(3)->uiRenderable->enabled = true;
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
							//m_loadScreen->GetElement(1)->uiRenderable->enabled = true; // this old win screen
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



		if (m_grafiki->InteractPlayer(Vec2(m_player->transform.worldPosition.x, m_player->transform.worldPosition.z), m_gui))
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
			return;
		}
	}
}

// Debug stuff
void Game::DrawIMGUIWindows()
{

#if WW_IMGUI 1

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
			ImGui::DragFloat("Fog Radius", &m_fogRadius, 0.1f, -100.0f, 100.0f);
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

	if (ImGui::Begin("Color Settings"))
	{
		float speed = 0.01f;
		bool changed = false;
		
		changed |= ImGui::DragFloat2("Vignette Radius & Strength", (float*)&m_vignetteStrengthAndRadius, speed, 0.0f, FLT_MAX);
		changed |= ImGui::DragFloat2("Contrast Amount & Midpoint", (float*)&m_contrastStrengthAndMidpoint, speed, 0.0f);
		changed |= ImGui::DragFloat("Brightness", &m_finalBrightness, speed, 0.0f, FLT_MAX);
		changed |= ImGui::DragFloat("Saturation", &m_finalSaturation, speed, 0.0f, FLT_MAX);

		/*if (changed || firstSet)
		{
			firstSet = false;
			Renderer::UpdatePPFXInfo(vignette, contrast, brightness, saturation);
		}*/

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

			// Distance is in world units.
			float distance = 0.5f / (BM_MAX_SIZE / BM_PIXELS_PER_UNIT);

			m_player->currentRoom;
			for (int i = 0; i < f.rooms.Size(); i++)
			{
				Level& l = f.rooms[i];
				m_testRenderables.Add( Renderer::CreateMeshStatic( "room_plane.wwm" ) );
				const Vec3 levelPos = Vec3(l.position.x * distance, 3.2f, l.position.z * distance);
				m_testRenderables.Back()->worldMatrix = Mat::translation3(levelPos) * l.rotation.Matrix() * Mat::scale3( 0.8f );

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
	m_gui->GetElement(15)->alpha = 0.0f;
	// Test of cinematics // TODO: IMPORTANT: LATER DON'T DO THIS WHEN THE GAME IS RUNNING, ITS PROBABLY FATASS-HEAVY ON THE CPU.
	//m_cutsceneController->Update(m_deltaTime);
	if (m_cutsceneController->CutsceneActive())
	{
		m_gui->GetElement(13)->uiRenderable->enabled = true;
		m_gui->GetElement(14)->uiRenderable->enabled = true;
	}
	else
	{
		m_gui->GetElement(13)->uiRenderable->enabled = false;
		m_gui->GetElement(14)->uiRenderable->enabled = false;
	}

	// More later
	m_player->CinematicUpdate( m_deltaTime ); // Only updates the matrix and animator, allowing for cutscenecontroller control.
	m_grafiki->CinematicUpdate( m_deltaTime ); // Only updates the matrix and animator, allowing for cutscenecontroller control.
	m_currentRoom->Update( m_deltaTime );
	for (int i = 0; i < m_staticObjects.Size(); i++)
	{
		m_staticObjects[i]->Update( m_deltaTime );
	}
}

static bool firstFrame = false;
static bool secondFrame = false;
// Main update function.
void Game::Update(float deltaTime, Renderer* renderer)
{
	m_deltaTime = deltaTime;

	if (!firstFrame)
	{
		firstFrame = true;
	}
	else if (!secondFrame)
	{
		secondFrame = true;
		m_cutsceneController->m_cutSceneActive = true;
		m_cutsceneController->m_isPlaying = true;
	}
	m_cutsceneController->Update(m_deltaTime);
	m_gui->GetElement(14)->alpha = 0;
	m_gui->GetElement(14)->uiRenderable->enabled = false;

	if (!m_cutsceneController->m_cutSceneActive)
	{
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
	}

	if (youWin)
	{
		m_winTimer += deltaTime; // for slideshow
		int slideShowIndex = std::floor(m_winTimer / m_timePerEndSlideShow);
		if (slideShowIndex > 4)
		{
			m_winTimer -= deltaTime / 2;
		}

		if (slideShowIndex >= 5)
		{
			m_loadScreen->GetElement(8)->uiRenderable->enabled = false;
			m_loadScreen->GetElement(9)->uiRenderable->enabled = true;
		}
		if (Input::Get().IsDXKeyPressed(DXKey::Space) && slideShowIndex >= 5)
		{
			//player pressed spacebar when outside while loop
			//m_loadScreen->GetElement(1)->uiRenderable->enabled = false;
			m_loadScreen->GetElement(9)->uiRenderable->enabled = false;
			youWin = false;
			m_winTimer = 0.0f;
			EndRun(renderer);
		}
		else if (slideShowIndex == 0)
		{
			m_loadScreen->GetElement(4)->uiRenderable->enabled = true;
		}
		else if (slideShowIndex > 0 && slideShowIndex < 5)
		{
			m_loadScreen->GetElement(slideShowIndex + 3 )->uiRenderable->enabled = false;
			m_loadScreen->GetElement(slideShowIndex + 4)->uiRenderable->enabled = true;
			return;
		}
		else
		{
			return;
		}
	}

	if (!m_cutsceneController->m_cutSceneActive)
	{
		if (secondFrame)
		{
			m_gui->GetElement(12)->alpha = 0.0f;
			m_gui->GetElement(13)->alpha = 0.0f;
			m_gui->GetElement(12)->uiRenderable->enabled = false;
			m_gui->GetElement(13)->uiRenderable->enabled = false;

		}

		if (m_transitionTarget != TransitionTargetNone)
		{
			TransitionUpdate(deltaTime);
		}
		else
		{
			UpdateRoomAndTimeSwappingLogic(renderer);
		}

		UpdateEnemies(renderer);
		UpdateGameObjects();
		UpdateGameplayVars(renderer);

		// Final steps
		UpdateTimeSwitchBuffers( renderer );
		UpdateEnemyConeBuffers( renderer );
		Renderer::UpdatePPFXInfo(
			m_vignetteStrengthAndRadius, 
			m_contrastStrengthAndMidpoint, 
			m_finalBrightness, 
			m_finalSaturation
		);
	}
	else
	{
		CinematicUpdate();
	}

	Renderer::UpdatePPFXInfo(
		m_vignetteStrengthAndRadius,
		m_contrastStrengthAndMidpoint,
		m_finalBrightness,
		m_finalSaturation
	);

	Move(deltaTime, m_player.get(), m_cutsceneController.get());

	Renderer::SetFogParameters(m_fogFocus, m_fogRadius);

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

	m_cameraPlayer = true;
	m_cameraLock = false;

	m_loadScreen = shared_ptr<GUI> (new GUI());

	// loading screen
	std::shared_ptr<GUIElement> loadingScreenGUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	loadingScreenGUI->colorTint = Vec3(1, 1, 1);
	loadingScreenGUI->alpha = 1.0f;
	loadingScreenGUI->uiRenderable->enabled = false;
	loadingScreenGUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	loadingScreenGUI->firstTexture = Resources::Get().GetTexture("loadingScreen.png");


	//winning screen 
	std::shared_ptr<GUIElement> winningScreenGUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	winningScreenGUI->colorTint = Vec3(1, 1, 1);
	winningScreenGUI->alpha = 1.0f;
	winningScreenGUI->uiRenderable->enabled = false;
	winningScreenGUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	winningScreenGUI->firstTexture = Resources::Get().GetTexture("winScreen.png");


	//skip tutorial screen 
	std::shared_ptr<GUIElement> skipTutorialScreenGUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	skipTutorialScreenGUI->colorTint = Vec3(1, 1, 1);
	skipTutorialScreenGUI->alpha = 1.0f;
	skipTutorialScreenGUI->uiRenderable->enabled = false;
	skipTutorialScreenGUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	skipTutorialScreenGUI->firstTexture = Resources::Get().GetTexture("skipTutorial.png");


	// text for not running backwards
	std::shared_ptr<GUIElement> runningBackwardsTextGUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	runningBackwardsTextGUI->colorTint = Vec3(1, 1, 1);
	runningBackwardsTextGUI->alpha = 1.0f;
	runningBackwardsTextGUI->uiRenderable->enabled = false;
	runningBackwardsTextGUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	runningBackwardsTextGUI->firstTexture = Resources::Get().GetTexture("TextWhite.png");
	runningBackwardsTextGUI->secondTexture = Resources::Get().GetTexture("coward.png");


	// THIS IS NEW WIN SCREENS
	std::shared_ptr<GUIElement> endClip1GUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	endClip1GUI->colorTint = Vec3(1, 1, 1);
	endClip1GUI->alpha = 1.0f;
	endClip1GUI->uiRenderable->enabled = false;
	endClip1GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	endClip1GUI->firstTexture = Resources::Get().GetTexture("endClip1.png");

	std::shared_ptr<GUIElement> endClip2GUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	endClip2GUI->colorTint = Vec3(1, 1, 1);
	endClip2GUI->alpha = 1.0f;
	endClip2GUI->uiRenderable->enabled = false;
	endClip2GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	endClip2GUI->firstTexture = Resources::Get().GetTexture("endClip2.png");

	std::shared_ptr<GUIElement> endClip3GUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	endClip3GUI->colorTint = Vec3(1, 1, 1);
	endClip3GUI->alpha = 1.0f;
	endClip3GUI->uiRenderable->enabled = false;
	endClip3GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	endClip3GUI->firstTexture = Resources::Get().GetTexture("endClip3.png");

	std::shared_ptr<GUIElement> endClip4GUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	endClip4GUI->colorTint = Vec3(1, 1, 1);
	endClip4GUI->alpha = 1.0f;
	endClip4GUI->uiRenderable->enabled = false;
	endClip4GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	endClip4GUI->firstTexture = Resources::Get().GetTexture("endClip4.png");

	std::shared_ptr<GUIElement> endClip5GUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	endClip5GUI->colorTint = Vec3(1, 1, 1);
	endClip5GUI->alpha = 1.0f;
	endClip5GUI->uiRenderable->enabled = false;
	endClip5GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	endClip5GUI->firstTexture = Resources::Get().GetTexture("endClip5.png");

	std::shared_ptr<GUIElement> endClip6GUI = m_loadScreen->AddGUIElement({ -1.0f,-1.0f }, { 2.0f, 2.0f }, nullptr, nullptr);
	endClip6GUI->colorTint = Vec3(1, 1, 1);
	endClip6GUI->alpha = 1.0f;
	endClip6GUI->uiRenderable->enabled = false;
	endClip6GUI->intData = Point4(0, 0, 0, 0); // No special flags, just the image
	endClip6GUI->firstTexture = Resources::Get().GetTexture("endClip6.png");

	//****

	// Audio test startup
	/*FMOD::Sound* soundPtr = (Resources::Get().GetSound("Duck.mp3"))->currentSound;
	m_audioSource = make_shared<AudioSource>(Vec3(0.0f, 0.0f, 0.0f), 0.2f, 1.1f, 0.0f, 10.0f, soundPtr);
	m_audioSource->Play();*/

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

	m_vignetteStrengthAndRadius = Vec2(0.5f, 1.0f);
	m_contrastStrengthAndMidpoint = Vec2(1.0f, 0.4f);
	m_finalBrightness = 0.0f;
	m_finalSaturation = 1.25f;
	firstSet = true;



	m_timeSwooshIn = 0.5f;
	m_timeSwooshOut = 0.5f;
}

void Game::DeInit()
{
	// Audio test 2 shutdown
	//m_audioSource->pitch = 0.75f;
	//m_audioSource->Play();

	/*int indexer = 0;
	while (m_audioSource->IsPlaying())
	{
		Sound::Get().Update();
		indexer++;
	}*/
}

void Game::InitCutscene()
{
	// Cutscenes

	m_cutsceneController = make_shared<CutsceneController>();
	m_testCutScene = make_shared<Cutscene>("Intro Cutscene");
	LoadWWC(m_testCutScene.get(), "Assets/Cutscenes/Intro Cutscene.wwc");

	// Setup channels for creation, comment out when loading later.
	//m_testCutScene->AddChannel( std::shared_ptr<CutsceneCameraChannel>( new CutsceneCameraChannel( "Main camera", &Renderer::GetCamera())));
	//m_testCutScene->AddChannel( std::shared_ptr<CutsceneAnimatorChannel>( new CutsceneAnimatorChannel( "Player Animator", m_game->GetPlayer()->characterAnimator.get())));
	//m_testCutScene->AddChannel( std::shared_ptr<CutsceneAnimatorChannel>( new CutsceneAnimatorChannel( "Grafiki Animator", m_game->m_grafiki->characterAnimator.get() ) ) );
	//m_testCutScene->AddChannel( std::shared_ptr<CutsceneTransformChannel>( new CutsceneTransformChannel( "Player Transform", &m_game->GetPlayer()->transform )));
	//m_testCutScene->AddChannel( std::shared_ptr<CutsceneTransformChannel>( new CutsceneTransformChannel( "Grafiki Transform", &m_game->m_grafiki->transform ) ) );
	//m_testCutScene->AddChannel(std::shared_ptr<CutsceneGUIChannel>(new CutsceneGUIChannel("GUI Channel 2", m_gui)));
	m_cutsceneController->m_cutscenes.Add(m_testCutScene);
	m_cutsceneController->ActivateCutscene(0);

	// Setup for cutscene playback.
	CutsceneCameraChannel* channel = (CutsceneCameraChannel*)m_cutsceneController->m_cutscenes[0]->channels[0].get();
	channel->targetCamera = &Renderer::GetCamera();
	CutsceneAnimatorChannel* animatorChannel = (CutsceneAnimatorChannel*)m_cutsceneController->m_cutscenes[0]->channels[1].get(); // Player
	animatorChannel->targetAnimator = m_player->characterAnimator.get();
	CutsceneAnimatorChannel* animatorChannel2 = (CutsceneAnimatorChannel*)m_cutsceneController->m_cutscenes[0]->channels[2].get(); // Grafiki
	animatorChannel2->targetAnimator = m_grafiki->characterAnimator.get();
	CutsceneTransformChannel* transformChannel = (CutsceneTransformChannel*)m_cutsceneController->m_cutscenes[0]->channels[3].get(); // Player
	transformChannel->targetTransform = &m_player->transform;
	CutsceneTransformChannel* transformChannel2 = (CutsceneTransformChannel*)m_cutsceneController->m_cutscenes[0]->channels[4].get(); // Grafiki
	transformChannel2->targetTransform = &m_grafiki->transform;
	CutsceneGUIChannel* guiChannel = (CutsceneGUIChannel*)m_cutsceneController->m_cutscenes[0]->channels[5].get(); // GUI 1
	guiChannel->targetGUI = m_gui;
	m_gui->GetElement(14)->uiRenderable->enabled = false;
	guiChannel->targetGUIElement = 12; // Index

	CutsceneGUIChannel* guiChannel2 = (CutsceneGUIChannel*)m_cutsceneController->m_cutscenes[0]->channels[6].get(); // GUI 1
	guiChannel2->targetGUI = m_gui;
	guiChannel2->targetGUIElement = 13; // Index

	m_cutsceneController->m_cutSceneActive = false;
	m_cutsceneController->m_isPlaying = false;
}

void Game::LoadHubby()
{
	UnLoadPrevious();
	m_levelHandler->GenerateHubby( &m_floor, m_envParams );
	uint roomIndex = 0;
	LoadRoom(roomIndex);

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
	uint roomIndex = 0;
	LoadRoom(roomIndex);

	m_isHubby = false;
	Renderer::ExecuteShadowRender();
}

void Game::LoadTutorial()
{
	UnLoadPrevious();
	m_levelHandler->GenerateTutorial(&m_floor, m_envParams);
	LoadRoom(m_floor.startRoom);

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
	LoadRoom(m_floor.startRoom);

	m_player->transform.position = m_floor.startPosition;

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

Vec3 Lerp(Vec3 a, Vec3 b, float t)
{
	return a * (1.0f - t) + b * t;
}

Quaternion QuaternionLookRotation(Vec3 forward, Vec3 up)
{
	forward.Normalize();

	Vec3 vector = forward.Normalize();
	Vec3 vector2 = up.Cross(vector).Normalize();
	Vec3 vector3 = vector.Cross(vector2);
	float m00 = vector2.x;
	float m01 = vector2.y;
	float m02 = vector2.z;
	float m10 = vector3.x;
	float m11 = vector3.y;
	float m12 = vector3.z;
	float m20 = vector.x;
	float m21 = vector.y;
	float m22 = vector.z;


	float num8 = (m00 + m11) + m22;
	Quaternion quaternion;
	if (num8 > 0.0f)
	{
		float num = (float)std::sqrt(num8 + 1.0f);
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m12 - m21) * num;
		quaternion.y = (m20 - m02) * num;
		quaternion.z = (m01 - m10) * num;
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)std::sqrt(((1.0f + m00) - m11) - m22);
		float num4 = 0.5f / num7;
		quaternion.x = 0.5f * num7;
		quaternion.y = (m01 + m10) * num4;
		quaternion.z = (m02 + m20) * num4;
		quaternion.w = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		float num6 = (float)std::sqrt(((1.0f + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quaternion.x = (m10 + m01) * num3;
		quaternion.y = 0.5f * num6;
		quaternion.z = (m21 + m12) * num3;
		quaternion.w = (m20 - m02) * num3;
		return quaternion;
	}
	float num5 = (float)std::sqrt(((1.0f + m22) - m00) - m11);
	float num2 = 0.5f / num5;
	quaternion.x = (m20 + m02) * num2;
	quaternion.y = (m21 + m12) * num2;
	quaternion.z = 0.5f * num5;
	quaternion.w = (m01 - m10) * num2;
	return quaternion;
}

Quaternion Lerp(Quaternion q0, Quaternion q1, float t)
{
	DirectX::XMVECTOR Q0 = DirectX::XMVectorSet((float)q0.x, (float)q0.y, (float)q0.z, (float)q0.w);
	DirectX::XMVECTOR Q1 = DirectX::XMVectorSet((float)q1.x, (float)q1.y, (float)q1.z, (float)q1.w);
	DirectX::XMVECTOR OUTPUT = DirectX::XMQuaternionSlerp(Q0, Q1, t);
	DirectX::XMFLOAT4 FL4;
	DirectX::XMStoreFloat4(&FL4, OUTPUT);
	return Quaternion(FL4.x, FL4.y, FL4.z, FL4.w);
}

Vec3 EulerAngles(Quaternion q)
{
	Vec3 returnValue;
	// roll (x-axis rotation)
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	returnValue.x = std::atan2f(sinr_cosp, cosr_cosp);

	// pitch (z-axis rotation)
	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		returnValue.y = std::copysignf(DirectX::XM_PI / 2, sinp); // use 90 degrees if out of range
	else
		returnValue.y = std::asinf(sinp);

	// yaw (y-axis rotation)
	float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	returnValue.z = std::atan2f(siny_cosp, cosy_cosp);

	return returnValue;
}

void Game::Move(float dTime, Player* player, CutsceneController* cutSceneController)
{
	if (dTime > 0.2f)
	{
		dTime = 0.001f;
	}

	Camera& camera = Renderer::GetCamera();
	Input& inputRef = Input::Get();

	if (m_transitionTarget == TransitionTargetNone && !cutSceneController->CutsceneActive())
	{
		MouseState mouseState = inputRef.GetMouseState();

		if (inputRef.IsDXKeyPressed(DXKey::R))
		{
			if (mouseState.positionMode == dx::Mouse::MODE_RELATIVE)
			{
				inputRef.SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
			}
			else
			{
				inputRef.SetMouseMode(dx::Mouse::MODE_RELATIVE);
			}
		}

		if (inputRef.IsDXKeyPressed(DXKey::P))
		{
			m_cameraPlayer = !m_cameraPlayer;
			player->cameraIsLocked = m_cameraPlayer;
		}

		if (!m_cameraPlayer)
		{
			// Debug Camera Movement
			Vec3 movement = Vec3(0, 0, 0);
			Vec3 forwardDirection = camera.GetDirection();
			forwardDirection.y = 0;
			forwardDirection.Normalize();
			Vec3 rightDirection = camera.GetRight();
			Vec3 upDirection = Vec3(0.0f, 1.0f, 0.0f);
			if (inputRef.IsKeybindDown(KeybindForward))		movement += forwardDirection;
			if (inputRef.IsKeybindDown(KeybindBackward))	movement -= forwardDirection;
			if (inputRef.IsKeybindDown(KeybindRight))		movement += rightDirection;
			if (inputRef.IsKeybindDown(KeybindLeft))		movement -= rightDirection;
			if (inputRef.IsKeybindDown(KeybindUp))			movement += upDirection;
			if (inputRef.IsKeybindDown(KeybindDown))		movement -= upDirection;
			if (inputRef.IsKeybindDown(KeybindSprint))
			{
				movement *= 5.0f;
			}
			static Vec3 rotationVec = {};
			if (mouseState.positionMode == dx::Mouse::MODE_RELATIVE)
			{
				cs::Vec3 delta = Vec3((float)mouseState.y, (float)mouseState.x, 0.0f);
				rotationVec -= delta * dTime * 2.0f;
				camera.SetRotation(Quaternion::GetEuler({ rotationVec.x, rotationVec.y, rotationVec.z }).Conjugate());
#if WW_IMGUI 1
				if (ImGui::Begin("Camera rotation dev"))
				{
					ImGui::Text("Rot Vec: %f, %f, %f", rotationVec.x, rotationVec.y, rotationVec.z);
					ImGui::Text("Rot: %f, %f, %f, %f", camera.GetRotation().x, camera.GetRotation().y, camera.GetRotation().z, camera.GetRotation().w);
					ImGui::Text("Dir: %f, %f, %f", camera.GetDirection().x, camera.GetDirection().y, camera.GetDirection().z);
					ImGui::Text("Delta: %f, %f, %f", delta.x, delta.y, delta.z);

				}
				ImGui::End();
#endif
				//camera.SetRotation(Quaternion::GetEuler(rotationVec));
			}
			camera.SetPosition(camera.GetPosition() + movement * dTime);
		}
		else
		{
			Vec3 cameraCurrentPos = camera.GetPosition();
			Vec3 cameraTargetPos = player->cameraFollowTarget;
			Quaternion cameraCurrentRot = camera.GetRotation();
			Quaternion cameraTargetRot = player->cameraLookRotationTarget;

			Vec3 lerped = Lerp(cameraCurrentPos, cameraTargetPos, dTime * 5);
			camera.SetPosition(lerped);
			if (!(std::isnan(cameraTargetRot.x) || std::isnan(cameraTargetRot.y) || std::isnan(cameraTargetRot.z) || std::isnan(cameraTargetRot.w)))
			{
				Quaternion slerped;
				slerped = Lerp(cameraCurrentRot, cameraTargetRot, cs::fclamp(dTime * 5.0f, 0.0001f, 1.0f));
				slerped.NormalizeThis();
				camera.SetRotation(slerped);
			}
#if WW_IMGUI 1
			if (ImGui::Begin("Camera rotation player"))
			{
				Vec3 playPos = player->transform.GetWorldPosition();
				Quaternion playerRot = player->transform.GetWorldRotation();
				Vec3 playerRotEuler = EulerAngles(playerRot);
				ImGui::Text("Player Pos: %f, %f, %f", playPos.x, playPos.y, playPos.z);
				ImGui::Text("Player Rot: %f, %f, %f, %f", playerRot.x, playerRot.y, playerRot.z, playerRot.w);
				ImGui::Text("Player Rot Euler deg: %f, %f, %f", playerRotEuler.x * RAD2DEG, playerRotEuler.y * RAD2DEG, playerRotEuler.z * RAD2DEG);
				//ImGui::Text("Dir: %f, %f, %f", direction.x, direction.y, direction.z);
				ImGui::Text("RotP: %f, %f, %f, %f", cameraTargetRot.x, cameraTargetRot.y, cameraTargetRot.z, cameraTargetRot.w);
				ImGui::DragFloat("Camera Follow Distance", &player->cameraFollowDistance, 0.05f, 0.1f, 10.0f);
				ImGui::DragFloat("Camera Follow Tilt", &player->cameraFollowTilt, 0.05f, 0.1f, cs::c_pi / 2 - 0.1f);
				ImGui::DragFloat3("Camera lookAt offset", (float*)&player->cameraLookTargetOffset, 0.1f);
			}
			ImGui::End();
#endif
		}
	}

	camera.Update();
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

void Game::SetGUI(GUI* gui)
{
	m_gui = gui;
}

void Game::GodMode(bool godMode)
{
	m_godMode = godMode;
}

bool Game::IsInHubby()
{
	return m_isHubby;
}

void Game::LoadRoom(uint levelIndex)
{
	Level& level = m_floor.rooms[levelIndex];

	m_levelHandler->SetFloormapFocusRoom(&level);

	Mat4 roomOffset =
		Mat::translation3(0, -0.01f, 0) *
		Mat::scale3(level.resource->worldWidth, 1.0f, level.resource->worldHeight);

	Mat4 cylinderOffset =
		Mat::translation3( 0, -0.02f, 0 ) *
		Mat::scale3(level.resource->worldWidth * 1.3f, 1.0f, level.resource->worldHeight * 1.3f);

	m_currentRoom = shared_ptr<Room>(new Room(&level, "room_plane.wwm", "room_walls_floor.wwm", roomOffset, cylinderOffset ));
	m_currentRoom->transform.position = level.position;
	m_currentRoom->transform.rotation = level.rotation;

	
	m_currentRoom->transform.CalculateWorldMatrix();
	m_directionalLight->transform.parent = &m_currentRoom->transform;
	m_directionalLight->Update(0);

	m_fogFocus = level.position;
	m_fogRadius = level.resource->worldWidth * 0.55f;

	Renderer::LoadEnvironment(m_currentRoom->m_level);

	m_player->currentRoom = m_currentRoom.get();
	
	for ( LevelPickup& pickup : level.resource->pickups )
	{
		Vec3 worldpos = m_player->currentRoom->bitMapToWorldPos(static_cast<Point2>(pickup.position));
		shared_ptr<Pickup> item = make_shared<EssenceBloom>(m_player.get(), Vec2(worldpos.x, worldpos.z));
		m_pickups.Add(item);
	}

	for (LevelPatrol& p : level.resource->patrolsClosed)
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

	for (LevelPatrol& p : level.resource->patrolsOpen)
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
	m_deathTransition = false;
	m_deathEnemy = false;
	m_deathPoison = false;
	m_isSwitching = false;
	Renderer::GetCamera().SetFov(m_initialCamFov);

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

	m_isSwitching = true;
	m_deathTransition = true;
	m_finishedCharging = false;
	if (m_initialCamFov == 0.0f)
		m_initialCamFov = renderer->GetCamera().GetFov();
}

void Game::EndRunDueToPoison(Renderer* renderer)
{
	// Logic for dying from future poison here.

	m_isSwitching = true;
	m_deathTransition = true;
	m_finishedCharging = false;
	m_player->m_switchSource->Play();
	if (m_initialCamFov == 0.0f)
		m_initialCamFov = renderer->GetCamera().GetFov();
}


void Game::TransitionStart(Vec3 exitPosition, Vec3 exitDirection, uint targetRoom, Vec3 targetPosition, Vec3 targetDirection)
{
	Camera& c = Renderer::GetCamera();

	m_transitionTarget = TransitionTargetLoadRoom;
	m_transitionTimeTarget = 0.3f;

	m_initialFogFocus = m_fogFocus;
	m_initialFogRadius = m_fogRadius;
	m_initialCameraPosition = c.GetPosition();
	m_initialCameraRotation = c.GetRotation();

	m_targetFogFocus = exitPosition + exitDirection * 4.0f;
	m_targetFogRadius = -30.0f;
	m_targetCameraPosition = exitPosition + exitDirection * 5.0f + Vec3(0.0f, 2.0f, 0.0f);
	m_targetCameraRotation = Quaternion::GetDirection((exitDirection + Vec3(0.0f, -0.2f, 0.0f)).Normalized());
	m_targetRoom = targetRoom;

	m_targetSpawnPosition = targetPosition;
	m_targetSpawnDirection = targetDirection;

	m_transitionTimer.Lap();

	//m_player->SetMovementLock(true);
}

void Game::TransitionLoad()
{
	ExecuteLoad(m_targetRoom, m_targetSpawnPosition, m_targetSpawnDirection);
	m_transitionTarget = TransitionTargetFree;
	m_transitionTimeTarget = 0.8f;

	m_initialFogFocus = m_targetSpawnPosition + m_targetSpawnDirection * 3.0f;
	m_initialFogRadius = -30.0f;
	m_initialCameraPosition = m_targetSpawnPosition + m_targetSpawnDirection * 1.5f + Vec3(0.0f, 2.0f, 0.0f);
	m_initialCameraRotation = Quaternion::GetDirection((-m_targetSpawnDirection + Vec3(0.0f, -0.2f, 0.0f)).Normalized());

	m_targetFogFocus = m_fogFocus;
	m_targetFogRadius = m_fogRadius;
	m_targetCameraPosition = m_player->cameraFollowTarget;
	m_targetCameraRotation = m_player->cameraLookRotationTarget;

	m_transitionTimer.Lap();
}

void Game::TransitionEnd()
{
	//m_player->SetMovementLock(false);
	m_transitionTarget = TransitionTargetNone;
}

void Game::TransitionUpdate(float deltaTime)
{
	Camera& c = Renderer::GetCamera();

	float lapsed = m_transitionTimer.Peek();
	float lerp = cs::fclamp(lapsed, 0.0f, m_transitionTimeTarget) / m_transitionTimeTarget;
	float ease = m_transitionTarget == TransitionTargetLoadRoom ?
		cs::ease::in(lerp) :
		cs::ease::outCubic(lerp);

	if (m_transitionTarget == TransitionTargetFree)
	{
		m_player->UpdateCameraVars();
		m_targetCameraPosition = m_player->cameraFollowTarget;
		m_targetCameraRotation = m_player->cameraLookRotationTarget;
	}

	m_fogFocus = m_initialFogFocus + (m_targetFogFocus - m_initialFogFocus) * ease;
	m_fogRadius = m_initialFogRadius + (m_targetFogRadius - m_initialFogRadius) * ease;

	c.SetPosition(m_initialCameraPosition + (m_targetCameraPosition - m_initialCameraPosition) * ease);

	Vec3 fromDirection = m_initialCameraRotation * Vec3(0.0f, 0.0f, 1.0f);
	Vec3 toDirection = m_targetCameraRotation * Vec3(0.0f, 0.0f, 1.0f);
	Vec3 lerpDirection = fromDirection + (toDirection - fromDirection) * ease;

	Vec3 fromUp = (fromDirection % Vec3(0.0f, 0.0f, 1.0f)) % fromDirection;
	Vec3 toUp = (toDirection % Vec3(0.0f, 0.0f, 1.0f)) % toDirection;
	Vec3 lerpUp = fromUp + (toUp - fromUp) * ease;

	Vec3 val = (lerpUp % lerpDirection) % lerpUp;

	c.SetRotation(Quaternion::GetDirection(val.Normalized()));

	if (lapsed > m_transitionTimeTarget)
	{
		switch (m_transitionTarget) 
		{
		case TransitionTargetFree:
			TransitionEnd();
			break;

		case TransitionTargetLoadRoom:
			TransitionLoad();
			TransitionUpdate(deltaTime);
			break;

		case TransitionTargetExit:
			break;
		}
	}
}

void Game::ExecuteLoad(uint targetRoom, Vec3 position, Vec3 direction)
{
	UnLoadPrevious();
	// TODO: Check this
	LoadRoom(targetRoom);

	m_currentRoom->transform.CalculateWorldMatrix();
	m_directionalLight->transform.parent = &m_currentRoom->transform;
	m_directionalLight->Update(0);

	MovePlayer(position - direction * TUNNEL_SPAWN_DISTANCE, -direction);
	m_player->ReloadPlayer();

	Renderer::ExecuteShadowRender();
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
