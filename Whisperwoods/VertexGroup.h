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
	int index;
	cs::List<VertexWeightPair> verticies;
};