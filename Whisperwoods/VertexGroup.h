#pragma once
#include <string>
#include <CHSL/List.h>


struct VertexWeightPair
{
	int vertex;
	float weight;
};


struct VertexGroup
{
	std::string name;
	cs::List<VertexWeightPair> verticies;
};