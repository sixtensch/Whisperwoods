#pragma once

struct GameObject
{
	std::string name;
	Transform transform;
	cs::List<GameObject*> gameObjects; // Children

	// Constructors
	GameObject() = default;
	GameObject(std::string p_name);
	GameObject(std::string p_name, Transform p_transform);
	
	// Destructor
	~GameObject();

	// Operators overload for the add and remove functions
	void AddChild(GameObject* p_other);
	void RemoveChild(GameObject* p_other);
	GameObject& operator+ (GameObject& other);
	GameObject& operator- (GameObject& other);

	// Core update function
	void Update() {};
};