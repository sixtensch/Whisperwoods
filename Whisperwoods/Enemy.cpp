#include "core.h"
#include "Enemy.h"
#include "Renderer.h"
#include "FBXImporter.h"

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
	m_rotationSpeed = 0.005f;
	m_offset = 0;


	m_carcinian = Renderer::CreateMeshRigged(modelResource);
	FBXImporter importer;

	m_modelOffset = modelOffset;
	// Import the animations
	m_animationSet = std::make_shared<AnimationResource>();
	importer.ImportFBXAnimations(animationsPath, m_animationSet.get());
	m_firstTrigger = false;
}

Enemy::~Enemy()
{
}

void Enemy::Update(float dTime)
{
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
	if (m_rotation == false) // keep this for stop->rotate, remove for rotate while moving?
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
	

	transform.CalculateWorldMatrix();

	//handle rotation:
	float angle = atan2(m_walkingDirection.y, m_walkingDirection.x) * 180 / cs::c_pi;
	float angleDecimal = angle / 180;
	//m_offset = angleDecimal;
	if (m_rotation == false)
	{
		m_rotationCounter = angleDecimal;
		m_offset = angleDecimal;
	}
	else
	{
		if (m_rotateClockWise == false)//counter clockwise, add onto the offset
		{
			if (m_rotationCounter > angleDecimal + m_rotationSpeed * 1.1) //DO NOT CHANGE THIS
			{
				angle = 90 - angle;
				angleDecimal = angle / 180;
			}
			m_rotationCounter += m_rotationSpeed;
			if (m_rotationCounter >= angleDecimal)
			{
				m_rotationCounter = angleDecimal;
				m_rotation = false;
			}
		}
		else // clockwise,   take away from offset
		{
			if (m_rotationCounter < angleDecimal - m_rotationSpeed * 1.1) // DO NOT CHANGE THIS
			{
				angle = 90 + angle;
				angleDecimal = angle / 180;
			}
			m_rotationCounter -= m_rotationSpeed;
			if (m_rotationCounter <= angleDecimal)
			{
				m_rotationCounter = angleDecimal;
				m_rotation = false;
			}
		}
		m_offset = m_rotationCounter;
	}
	
	m_carcinian->worldMatrix = transform.worldMatrix * m_modelOffset * Mat::rotation3(0, -cs::c_pi * m_offset + cs::c_pi * 0.5, 0);
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
	FBXImporter importer;

	m_modelOffset = modelOffset;
	// Import the animations
	m_animationSet = std::make_shared<AnimationResource>();
	importer.ImportFBXAnimations(animationsPath, m_animationSet.get());
}
