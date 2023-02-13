#pragma once
#include "GameObject.h"
#include "ModelResource.h"
#include "Animator.h"
#include "MeshRenderable.h"

class Player : public GameObject
{

public:
	
	shared_ptr<MeshRenderableRigged> characterModel;


	void Update(float delta_time) override {
		characterModel->worldMatrix = transform.worldMatrix;
	}
};