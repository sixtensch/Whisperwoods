#pragma once

struct GameObject
{
	std::string name;
	Transform transform;
	// Maybe add parent?
	GameObject* parent; // Possible exension
	cs::List<GameObject*> gameObjects; // Children

	// Constructors
	GameObject() { name = "Nameless GameObject"; }
	GameObject(std::string p_name);
	GameObject(std::string p_name, Transform p_transform);
	
	// Destructor
	~GameObject();

	// Parent/Child functions
	void AddChild(GameObject* p_other);
	void RemoveChild(GameObject* p_other);
	// Operators overload for the add and remove functions
	GameObject& operator+ (GameObject& other);
	GameObject& operator- (GameObject& other);

	// Core update function
	void Update() {};
};