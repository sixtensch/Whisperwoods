#include "Constants.hlsli"

static const float MAX_CONE_HEIGHT = 2.0f;
static const float MIN_COLOR_STRENGTH = 1.0f;
static const float MAX_COLOR_STRENGTH = 7.0f;
static const float3 BASE_CONE_COLOR = float3(0.5f, 0.2f, 0.0f);
static const float3 DETECTED_CONE_COLOR = float3(1.0f, 0.0f, 0.0f);

static const float ENEMY_POS_MAX_DISTANCE = 0.5f;
static const float3 ENEMY_POS_COLOR = float3(0.6f, 0.4f, 0.03f);

float3 DrawEnemyCone(float3 enemyPos, float3 worldPos, float3 viewDirection, float coneLength, float coneAngle, float detectionLevel)
{
    float3 enemyToPoint = worldPos - enemyPos;
    float dirAlignment = dot(normalize(enemyToPoint), normalize(viewDirection));
    float isInCone = step(cos(coneAngle), dirAlignment); // If aligned within cone angle.
    
    float coneInfluence = 1.0f - smoothstep(0.0f, coneLength * 1.2f, length(enemyToPoint));
    float heightInfluence = 1.0f - smoothstep(0.0f, MAX_CONE_HEIGHT, worldPos.y);
	
    float totalInfluence = coneInfluence * isInCone * heightInfluence;
    
    float detectionScaling = 1.2f; // Makes detection reach max faster
    float detectionInfluence = detectionLevel * detectionScaling;
    float colorStrength = lerp(MIN_COLOR_STRENGTH, MAX_COLOR_STRENGTH, detectionInfluence);

    float3 coneColor = lerp(BASE_CONE_COLOR, DETECTED_CONE_COLOR, detectionInfluence) * colorStrength;

    return coneColor * totalInfluence;
}

float3 DrawEnemyPos(float3 enemyPos, float3 worldPos)
{
    float dist = distance(enemyPos, worldPos);
    float distInfluence = 1.0f - smoothstep(0.0f, ENEMY_POS_MAX_DISTANCE, dist);
    
    return ENEMY_POS_COLOR * distInfluence;
}