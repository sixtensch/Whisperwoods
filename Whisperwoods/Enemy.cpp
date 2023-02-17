#include "core.h"
#include "Enemy.h"
#include "Renderer.h"
#include "FBXImporter.h"
#include "Resources.h"

Enemy::Enemy(std::string modelResource, std::string animationsPath, Mat4 modelOffset)
{
	m_currentPosition = Vec2(0.0f, 0.0f);
	m_currentPatrolIndex = 1; // Starts on patrol index 0 and walks towards index 1
	m_walkingSpeed = 2.0f;
	m_enclosedLoop = false; // Default value
	m_indexChanger = 1;
	m_distanceToPatrolPoint = 0.0f;
	m_walkingDirection = Vec2(0.0f, 0.0f);
	m_enemyAlive = false; // Default false, has to be manually turned on
	m_rotation = false;
	
	m_offset = 0;
	m_idleCounter = 0;
	m_timeToGivePlayerAChanceToRunAway = m_amountOfTimeToRunAway;

	m_lookBehind = false;
	m_seesPlayer = false;
	//m_currentRotation = Vec3(0, 0, 0);

	m_PatrolEnemy = false;
	m_triggerTurn = false;
	m_isMoving = true;


	m_carcinian = Renderer::CreateMeshRigged(modelResource);
	//FBXImporter importer;
	m_characterAnimator = std::make_unique<Animator>((ModelRiggedResource*)Resources::Get().GetResource(ResourceTypeModelRigged, "Carcinian_Animated.wwm"));
	


	m_modelOffset = modelOffset;
	// Import the animations
	Resources& resources = Resources::Get();
	m_animationSet = (AnimationResource*)resources.GetResource( ResourceTypeAnimations, animationsPath );
	//importer.ImportFBXAnimations(animationsPath, m_animationSet.get());
	m_firstTrigger = false;

	Animation* carcinAnim0 = &m_animationSet->animations[0];
	Animation* carcinAnim1 = &m_animationSet->animations[1];
	Animation* carcinAnim2 = &m_animationSet->animations[3];
	Animation* carcinAnim3 = &m_animationSet->animations[4];

	float speed3 = 1.0f;
	
	m_characterAnimator->AddAnimation(carcinAnim0, 0, speed3, 0.0f);
	m_characterAnimator->AddAnimation(carcinAnim1, 0, speed3, 0.0f);
	m_characterAnimator->AddAnimation(carcinAnim2, 0, speed3, 0.0f);
	m_characterAnimator->AddAnimation(carcinAnim3, 0, speed3, 0.0f);


	m_characterAnimator->PlayAnimation(0, 0, 1, true, true);

	m_carcinian->Materials().AddMaterial((const MaterialResource*)Resources::Get().GetResource(ResourceTypeMaterial, "Carcinian.wwmt"));
}

Enemy::~Enemy()
{
}

void Enemy::Update(float dTime)
{
	m_timeToGivePlayerAChanceToRunAway += dTime;
	//This has to be in update because we re-use enemy objects between rooms! Will cost like O(1), it's fine
	if (m_patrolPath.size() < 3 && m_firstTrigger == false)
	{
		m_PatrolEnemy = true;
		m_characterAnimator->StopAnimation(0);
		m_characterAnimator->PlayAnimation(2, 0, 1, false, true);
		m_lastPlayedAnimation = 2;
		m_isMoving = false;
		m_characterAnimator->playbackSpeed = 0.2f;
	}



	m_characterAnimator->Update(dTime);
	bool first = false;
	if (m_firstTrigger == false) // Default on first update
	{
		m_currentPosition = m_patrolPath[0]; // like an init value
		m_firstTrigger = true;
		first = true;
	}
	// Movement:
	if (first || (m_currentPosition.x == m_patrolPath[m_currentPatrolIndex].x && m_currentPosition.y == m_patrolPath[m_currentPatrolIndex].y)) // Enemy reached patrol point, find new one to walk towards
	{
		if (!first)
		{
			m_rotation = true;
		}

		if (!first && !m_enclosedLoop && (m_currentPatrolIndex == 0 || m_currentPatrolIndex == m_patrolPath.size() - 1)) // start index or last index with stopping and 180 degree turn. Run animation here
		{
			m_isMoving = false;
		}

		if (m_currentPatrolIndex == m_patrolPath.size() - 1 && !first) // Current position is at the end of the patrol index vector
		{
			if (m_enclosedLoop) // Enemy walks in a circle
			{
				m_indexChanger = 1;
				m_currentPatrolIndex = 0; // new index to walk towards
			}
			else
			{
				m_indexChanger = -m_indexChanger; // Opposite direction
				m_currentPatrolIndex = m_currentPatrolIndex + m_indexChanger; // new index to walk towards
				if (m_currentPatrolIndex < 0)
				{
					m_currentPatrolIndex = 1;
				}
			}
			
		}
		else if(!first)// in the middle of the loop, find next index
		{

			m_currentPatrolIndex += m_indexChanger;
			if (m_currentPatrolIndex < 0)
			{
				m_currentPatrolIndex = 1;
				m_indexChanger = -m_indexChanger;
			}
		}

		if (!first)
		{
			m_lastWalkingDirection = m_walkingDirection;
		}
		m_walkingDirection = m_patrolPath[m_currentPatrolIndex] - m_currentPosition; // Direction to walk towards
		m_walkingDirection.Normalize();
		if (first)
		{
			m_lastWalkingDirection = m_walkingDirection;
		}


		
		
		//rotate clockwise or counter clockwise?   if result is positive, then v is on the left side of u. If negative, right side. If 0, parallell(doesn't matter)
		//u x v = u1 * v2 - u2 * v1
		//old x new 
		float cross = m_lastWalkingDirection.x * m_walkingDirection.y - m_lastWalkingDirection.y * m_walkingDirection.x;
		if (cross >= 0) //rotate counter clockwise, new vector is to the left of old one
		{
			m_rotateClockWise = false;
		}
		else //rotate clockwise as the vector is to the right of the new one
		{
			m_rotateClockWise = true;
		}
	}

	
		
	Vec2 newPosition = m_currentPosition;
	// Time to walk
	if(m_isMoving && m_seesPlayer == false && m_timeToGivePlayerAChanceToRunAway >= m_amountOfTimeToRunAway)
	{
		newPosition = Vec2(m_currentPosition.x + m_walkingDirection.x * m_walkingSpeed * dTime, m_currentPosition.y + m_walkingDirection.y * m_walkingSpeed * dTime);
	}
	
	
	// Did we walk too far? 
	Vec2 tempDirection = m_patrolPath[m_currentPatrolIndex] - newPosition;
	tempDirection.Normalize();
	if (tempDirection.Dot(m_walkingDirection) < 0) // Enenmy has walked too far and the patrol point is now behind the enemy
	{
		newPosition = m_patrolPath[m_currentPatrolIndex]; // Set the position to the control point
	}

	// Set new position for enemy
	m_currentPosition = newPosition;

	//update transform
	transform.position.x = m_currentPosition.x;
	transform.position.z = m_currentPosition.y; //because transform is Vec3, y would be "height"
	

	//transform.CalculateWorldMatrix();

	//handle rotation:
	float angle = atan2(m_walkingDirection.y, m_walkingDirection.x) * 180 / cs::c_pi;
	float angleDecimal = angle / 180;

	if (m_PatrolEnemy == true && first == true) //for default, ignore this and just accept it
	{
		m_idleAngle = angleDecimal;
		m_offset = angleDecimal;
	}



	//m_offset = angleDecimal;
	if (m_rotation == false && m_isMoving == true)
	{
		m_rotationCounter = angleDecimal;
		m_offset = angleDecimal;
	}
	else if(m_isMoving == true)
	{
		if (m_rotateClockWise == false)//counter clockwise, add onto the offset
		{
			if (m_rotationCounter > angleDecimal + m_rotationSpeed * dTime * 1.1) //DO NOT CHANGE THIS
			{
				angle = 180 + angle;
				angleDecimal = angle / 180;
			}
			m_rotationCounter += m_rotationSpeed * dTime;
			if (m_rotationCounter >= angleDecimal)
			{
				m_rotationCounter = angleDecimal;
				m_rotation = false;
			}
		}
		else // clockwise,   take away from offset
		{

			if (m_rotationCounter < angleDecimal - m_rotationSpeed * dTime * 1.1) // DO NOT CHANGE THIS
			{
				angle = 180 - angle;
				angleDecimal = angle / 180; //something here might get messed up with an angle. look for teleport

			}
			m_rotationCounter -= m_rotationSpeed * dTime;
			if (m_rotationCounter <= angleDecimal)
			{
				m_rotationCounter = angleDecimal;
				m_rotation = false;
			}
		}
		m_offset = m_rotationCounter;
	}
	if(m_isMoving == false && m_PatrolEnemy == false)// 180 degree turn. Play animation here
	{
		float angle2 = atan2(m_lastWalkingDirection.y, m_lastWalkingDirection.x) * 180 / cs::c_pi;
		float angleDecimal2 = angle2 / 180;
		m_offset = angleDecimal2;
		if (m_rotation == true)
		{
			m_rotation = false;
		}
		else //if false
		{
			//turn animation	
			//m_characterAnimator->loadedAnimations[3].
			if (m_triggerTurn == false)
			{
				m_characterAnimator->StopAnimation(0);
				m_characterAnimator->playbackSpeed = 0.35f;
				m_characterAnimator->PlayAnimation(3, 0, 1, false, true);
				m_lastPlayedAnimation = 3;
				m_triggerTurn = true;
			}
			else if (m_triggerTurn == true && m_characterAnimator->AnimationsFinished() /*m_characterAnimator->loadedAnimations[3].time == 1*/) //animation is over
			{
				m_characterAnimator->StopAnimation(3);
				m_offset = angleDecimal;
				m_isMoving = true;
				m_triggerTurn = false;
				m_characterAnimator->playbackSpeed = 1.0f;
				m_characterAnimator->PlayAnimation(0, 0, 1, true, true);
				m_lastPlayedAnimation = 0;
			}
		}
		
	}
	if (m_PatrolEnemy == true) // behavior for idle enemy
	{
		if (m_idleCounter < 4 && m_characterAnimator->AnimationsFinished() && m_triggerTurn == false) // run idle animation
		{
			m_characterAnimator->playbackSpeed = 0.2f;
			m_characterAnimator->PlayAnimation(2, 0, 1, false, true);
			m_lastPlayedAnimation = 2;
			m_idleCounter++;
		}
		else if (m_idleCounter == 4 && m_characterAnimator->AnimationsFinished()) // has been idle long enough, run rotation animation
		{
			m_characterAnimator->StopAnimation(2);
			m_characterAnimator->playbackSpeed = 0.35f;
			m_characterAnimator->PlayAnimation(3, 0, 1, false, true);
			m_lastPlayedAnimation = 3;
			m_idleCounter = 0;
			m_triggerTurn = true;
			if (m_lookBehind == true) //these if/else is to change the direction 180 degrees
			{
				m_lookBehind = false;
			}
			else
			{
				m_lookBehind = true;
			}
		}
		if (m_triggerTurn == true && m_characterAnimator->AnimationsFinished())   // this section triggers ONLY when the roration animation is OVER
		{
			float angleReverse = atan2(m_walkingDirection.y, m_walkingDirection.x) * 180 / cs::c_pi;
			if (m_lookBehind == true)
			{
				angleReverse = atan2(-m_walkingDirection.y, -m_walkingDirection.x) * 180 / cs::c_pi;
			}
			float angleDecimalReverse = angleReverse / 180;
			m_idleAngle = angleDecimalReverse;


			m_triggerTurn = false;

		}
		m_offset = m_idleAngle;
	}
	Vec3 test = transform.GetWorldRotation() * Vec3(1, 0, 0);
	transform.SetRotationEuler(Vec3(0.0f, -cs::c_pi * m_offset + cs::c_pi * 0.5f, 0.0f));
	//Vec3 test = transform.rotation * Vec3(0, 0, 1);
	transform.CalculateWorldMatrix();
	test = transform.GetWorldRotation() * Vec3(0, 0, 1);
	m_carcinian->worldMatrix = transform.worldMatrix * m_modelOffset;

}

void Enemy::AddCoordinateToPatrolPath(Vec2 coord, bool enclosed) // Make sure the Coordinates are sent in the correct order of the path
{
	m_patrolPath.push_back(coord);
	m_enclosedLoop = enclosed;
}

void Enemy::EmptyPatrolPath()
{
	m_patrolPath.clear();
}

void Enemy::AddModel(std::string modelResource, std::string animationsPath, Mat4 modelOffset)
{
	m_carcinian = Renderer::CreateMeshRigged(modelResource);
	//FBXImporter importer;

	m_modelOffset = modelOffset;
	// Import the animations (Now using the resource manager)
	Resources& resources = Resources::Get();
	m_animationSet = (AnimationResource*)resources.GetResource(ResourceTypeAnimations, animationsPath);
	//m_animationSet = std::make_shared<AnimationResource>();
	//importer.ImportFBXAnimations(animationsPath, m_animationSet.get());
}

int SomeFunction(Vec3 pos)
{
	return 0;
}

bool Enemy::SeesPlayer(Vec2 playerPosition, AudioSource& quack, Room &room)
{
	// Let's start with if the enemy can see the player at all without TRUE line of sight

	//forward vector (which direction enemy is looking)
	Vec3 tempV = transform.rotation * Vec3(0, 0, 1);
	m_forwardVector = Vec2(tempV.x, tempV.z);
	m_forwardVector.Normalize();

	//direction vector from enemy position to player position
	Vec2 playerDirection(playerPosition.x - transform.worldPosition.x, playerPosition.y - transform.worldPosition.z);

	float distance = playerDirection.Length(); //distance from enemy to player
	//Vec2 vectorForLineOfSight = playerDirection;

	playerDirection.Normalize();
	float angle = acos(m_forwardVector.Dot(playerDirection)); // angle in radians
	angle = angle * (180.0f / cs::c_pi); //angle in acutal degrees

	m_seesPlayer = false;

	if (angle < m_enemyViewAngle && distance < m_enemyViewDistance) // is the player within range and circle sector of the enemy?
	{

		//Now we check if the enemy has true line of sight to the player
		m_seesPlayer = true;
		room.sampleBitMap(Vec3(playerPosition.x, 0.0f, playerPosition.y));
		room.sampleBitMap(Vec3(transform.worldPosition.x, 0.0f, transform.worldPosition.z));

		for (float i = 0; i < distance; i += 0.025f)// for each 10 cm in "distance"
		{
			Vec3 pointOnLineOfSightVector = transform.worldPosition + Vec3(playerDirection.x , 0.0f, playerDirection.y) * i;
			//pointOnLineOfSightVector = pointOnLineOfSightVector * i;

			LevelPixel bitmapPoint = room.sampleBitMap(Vec3(pointOnLineOfSightVector.x, 0.0f, pointOnLineOfSightVector.z));
			if (bitmapPoint != 0) // If terrain is not passible
			{ 

				m_seesPlayer = false;
				break;
			}
		} //worldToBitmapPoint(Vec3 worldPos)
	}
	if(m_seesPlayer)  //if the enemy sees the player
	{
		m_timeToGivePlayerAChanceToRunAway = 0.0f; //reset the counter
		if (quack.IsPlaying() == false) // audio cue
		{
			quack.pitch = distance / 4; //lmao funny noise
			quack.Play();
		}

		//stop other animations
		if (m_characterAnimator->IsPlaying(0))
		{
			m_characterAnimator->StopAnimation(0);
		}
		if (m_characterAnimator->IsPlaying(2))
		{
			m_characterAnimator->StopAnimation(2);
		}
		if (m_characterAnimator->IsPlaying(3))
		{
			m_characterAnimator->StopAnimation(3);
		}
		
		//play the detected animation on loop
		if(!m_characterAnimator->IsPlaying(1))
		{
			m_characterAnimator->PlayAnimation(1, 0, 1, true, true);
			m_characterAnimator->playbackSpeed = 0.5f;
		}
	}
	else // if not seen
	{
		quack.Stop(); // no more noises
		if (m_characterAnimator->IsPlaying(1)) //is the detected animation running?
		{
			if (m_timeToGivePlayerAChanceToRunAway >= m_amountOfTimeToRunAway) // has the player been given the time to run away?
			{
				m_characterAnimator->StopAnimation(1);
				if (m_lastPlayedAnimation == 0)
				{
					m_characterAnimator->playbackSpeed = 1.0f;
					m_characterAnimator->PlayAnimation(m_lastPlayedAnimation, 0, 1, true, true);
				}
				else if (m_lastPlayedAnimation == 2)
				{
					m_characterAnimator->playbackSpeed = 0.2f;
					m_characterAnimator->PlayAnimation(m_lastPlayedAnimation, 0, 1, false, true);
				}
				else if (m_lastPlayedAnimation == 3)
				{
					m_characterAnimator->playbackSpeed = 0.35f;
					m_characterAnimator->PlayAnimation(m_lastPlayedAnimation, 0, 1, false, true);
				}
			}
		}
		else if (m_timeToGivePlayerAChanceToRunAway < m_amountOfTimeToRunAway) // keep playing the animation of detection 
		{
			m_characterAnimator->PlayAnimation(1, 0, 1, true, true);
			m_characterAnimator->playbackSpeed = 0.5f;
		}

	}

	return m_seesPlayer; 
}

void Enemy::PlayEnemyActiveNoise()
{
	if (m_characterAnimator->IsPlaying(0))
	{
		//if(sound is not playing for this animation
			//play sound
	}
	else if (m_characterAnimator->IsPlaying(0))
	{
		//if(sound is not playing for this animation
			//play sound
	}
	else if (m_characterAnimator->IsPlaying(0))
	{
		//if(sound is not playing for this animation
			//play sound
	}
	else if (m_characterAnimator->IsPlaying(0))
	{
		//if(sound is not playing for this animation
			//play sound
	}
}

