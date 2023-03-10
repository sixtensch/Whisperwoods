#include "core.h"
#include "Grafiki.h"
#include "Renderer.h"
#include "Resources.h"
#include "SoundResource.h"

Grafiki::Grafiki()
{
	Resources& resources = Resources::Get();

	//Model
	characterModel = Renderer::CreateMeshRigged("Grafiki_Animated.wwm");

	//Materials
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiBody.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiGum.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiWhite.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiPants.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiSpikes.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiWhite.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiPupil.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiBeard.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiWood.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiCrystal.wwmt"));

	//animations
	animationSet = resources.GetAnimation("Grafiki_Animations.wwa");

	characterAnimator = make_shared<Animator>(resources.GetModelRigged("Grafiki_Animated.wwm"), characterModel);

	Animation* idleAnimation = &animationSet->animations[0];

	m_animationSpeed = 1.0f;

	characterAnimator->AddAnimation(idleAnimation, 0, m_animationSpeed, 1.0f);
	characterAnimator->AddAnimation( &animationSet->animations[1], 0, m_animationSpeed, 0.0f );
	characterAnimator->AddAnimation( &animationSet->animations[2], 0, m_animationSpeed, 0.0f );
	characterAnimator->AddAnimation( &animationSet->animations[3], 0, m_animationSpeed, 0.0f );
	characterAnimator->AddAnimation( &animationSet->animations[4], 0, m_animationSpeed, 0.0f );

	modelSpecialMatrix = Mat::scale3( 1.4f, 1.4f, 1.4f );
	characterModel->worldMatrix = transform.worldMatrix * Mat::translation3(0.0f, 0.0f, 0.0f)* Mat::rotation3(cs::c_pi * -0.5f, 0, 0) * modelSpecialMatrix;
}

Grafiki::~Grafiki()
{
}

void Grafiki::Reload()
{
	Resources& resources = Resources::Get();
	//Model
	characterModel = Renderer::CreateMeshRigged("Grafiki_Animated.wwm");

	//Materials
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiBody.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiGum.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiWhite.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiPants.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiSpikes.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiWhite.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiPupil.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiBeard.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiWood.wwmt"));
	characterModel->Materials().AddMaterial(resources.GetMaterial("GrafikiCrystal.wwmt"));

	characterAnimator->instanceReference = characterModel;
}

void Grafiki::Update(float delta_time)
{
	if (enabled)
	{
		characterAnimator->playbackSpeed = m_animationSpeed;
		characterAnimator->Update(delta_time);
		transform.CalculateWorldMatrix();

		characterModel->worldMatrix = transform.worldMatrix * Mat::translation3(0.0f, 0.0f, 0.0f) * Mat::rotation3(cs::c_pi * -0.5f, 0, 0) * modelSpecialMatrix;
	}
}

void Grafiki::CinematicUpdate( float delta_time )
{
	if (enabled)
	{
		//characterAnimator->playbackSpeed = m_animationSpeed;
		characterAnimator->Update( delta_time );
		transform.CalculateWorldMatrix();
		characterModel->worldMatrix = transform.worldMatrix * Mat::translation3( 0.0f, 0.0f, 0.0f ) * Mat::rotation3( cs::c_pi * -0.5f, 0, 0 ) * modelSpecialMatrix;
	}
}

bool Grafiki::InteractPlayer(Vec2 playerPosition)
{
	if (!enabled) return false;
	//direction vector from enemy position to player position
	Vec2 playerDirection(playerPosition.x - transform.worldPosition.x, playerPosition.y - transform.worldPosition.z);

	float distance = std::abs(playerDirection.Length()); //distance from enemy to player

	return false; // temp
	return distance <= m_range;
}
