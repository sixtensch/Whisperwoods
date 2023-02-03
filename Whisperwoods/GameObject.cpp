#include "Core.h"
#include "GameObject.h"

// Constructors
GameObject::GameObject(std::string p_name) : name(p_name) 
{
	parent = nullptr;
}

GameObject::GameObject(std::string p_name, Transform p_transform) : name(p_name), transform(p_transform) 
{
	parent = nullptr;
}

// Destructor
GameObject::~GameObject() {}

// Adds a child to the gameObject and adjusts transform acordingly
void GameObject::AddChild(GameObject* p_other)
{
	p_other->parent = this;
	gameObjects.Add(p_other);

	p_other->transform.parent = &transform;
	transform.transforms.Add(&p_other->transform); 
}

void GameObject::RemoveChild(GameObject* p_other)
{
	// Find and remove the reference in the gameObjects (children) list (if it exists)
	p_other->parent = nullptr; // TODO: Maybe remove
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

	// Clear the child's transform parent
	p_other->transform.parent = nullptr;
	index = -1;
	for (int i = 0; i < transform.transforms.Size(); i++)
	{
		if (transform.transforms[i] == &p_other->transform)
		{
			index = i;
			break;
		}
	}
	if (index >= 0)
	{
		transform.transforms.Remove(index);
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


