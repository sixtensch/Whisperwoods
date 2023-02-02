#pragma once
#include <CHSL/List.h>
#include "Vertex.h"

struct Mesh
{
	// Raw verticies and indicies from the model defining the mesh and submeshes
	cs::List<int> indicies;
	// cs::List<Material> materials; // Add when material exists.
	// These contain the startindex in the indexlist, the count, and materialindex for the submeshes of the mesh.
	cs::List<int> startIndicies;
	cs::List<int> indexCounts;
	cs::List<int> materialIndicies;
	// Constructor.
	Mesh(cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		indicies(p_indicies),
		startIndicies(p_startIndicies),
		indexCounts(p_indexCounts),
		materialIndicies(p_materialIndicies) {};
};

struct MeshStatic : Mesh
{
	// Raw verticies and indicies from the model defining the mesh and submeshes
	cs::List<VertexTextured> verticies;
	// Constructor.
	MeshStatic(cs::List<VertexTextured> p_verticies, cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		Mesh(p_indicies, p_startIndicies,  p_indexCounts, p_materialIndicies),
		verticies(p_verticies) {};
};


struct MeshRigged : Mesh
{
	// Raw verticies and indicies from the model defining the mesh and submeshes
	cs::List<VertexSkinned> verticies;
	// Constructor.
	MeshRigged(cs::List<VertexSkinned> p_verticies, cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		Mesh(p_indicies, p_startIndicies, p_indexCounts, p_materialIndicies),
		verticies(p_verticies) {};
};