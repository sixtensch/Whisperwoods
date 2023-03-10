#pragma once

#include "MeshRenderable.h"
#include "GameObject.h"
#include "AnimationResource.h"
#include "Animator.h"
#include "Room.h"
#include "AudioSource.h"
#include "GUI.h"

class Grafiki : public GameObject
{
public:
	Grafiki();
	
	~Grafiki();

	void Reload();

	void Update(float delta_time) override;

	void CinematicUpdate( float delta_time );

	bool InteractPlayer(Vec2 playerPosition, GUI* gui);

	bool enabled = true;

	Mat4 modelSpecialMatrix;

	shared_ptr<Animator> characterAnimator;
private:
	shared_ptr<MeshRenderableRigged> characterModel;
	AnimationResource* animationSet;
	shared_ptr<AudioSource> m_talkSource;

	float m_animationSpeed;

	float m_range = 2.0f;
};