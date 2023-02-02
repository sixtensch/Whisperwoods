#include "Core.h"
#include "Transform.h"
#include "GameObject.h"

GameObject::~GameObject()
{
	name = "Nameless GameObject";
}

GameObject::GameObject(std::string p_name) : name(p_name)
{	
}

GameObject::GameObject(std::string p_name, Transform p_transform) : name(p_name), transform(p_transform)
{
}


void GameObject::AddChild(GameObject* p_other)
{
	p_other->transform.parent = &transform;
	gameObjects.Add(p_other);
}

void GameObject::RemoveChild(GameObject* p_other)
{
	// Clear the child's transform parent
	p_other->transform.parent = nullptr;

	// Find and remove the reference in the gameObjects (children) list (if it exists)
	int index = -1;
	for (int i = 0; i < gameObjects.Size(); i++)
	{
		if (gameObjects[i] == p_other)
		{
			index = i;
			break;
		}
	}
	if (index >= 0)
	{
		gameObjects.Remove(index);
	}
}

// Operator overloads for the above
GameObject& GameObject::operator+(GameObject& other)
{
	this->AddChild(&other);
	return *this;
}

GameObject& GameObject::operator-(GameObject& other)
{
	this->RemoveChild(&other);
	return *this;
}


