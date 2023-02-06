#pragma once
#include <CHSL/List.h>
#include "Vertex.h"
#include "Armature.h"
#include <String>

struct ModelResource
{
	// Raw verticies and indicies from the model defining the mesh and submeshes
	cs::List<int> indicies;
	cs::List<int> materials; // TODO: Verify when material exists.
	cs::List<std::string> materialNames;

	// These contain the startindex in the indexlist, the count, and materialindex for the submeshes of the mesh.
	cs::List<int> startIndicies;
	cs::List<int> indexCounts;
	cs::List<int> materialIndicies;
	cs::List<std::string> materialNames;

	// Vertex and index buffers
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;

	// Index buffer function
	void CreateIndexBuffer(ComPtr<ID3D11Device>& device);

	// Constructor.
	ModelResource(cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		indicies(p_indicies),
		startIndicies(p_startIndicies),
		indexCounts(p_indexCounts),
		materialIndicies(p_materialIndicies) {};
};

struct ModelStaticResource : ModelResource
{
	// Textured verticies, that do not contain rigging data.
	cs::List<VertexTextured> verticies;

	// Vertex buffer function (unrigged (only textured) verticies)
	void CreateVertexBuffer(ComPtr<ID3D11Device>& device);

	// Constructor.
	ModelStaticResource(cs::List<VertexTextured> p_verticies, cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		ModelResource(p_indicies, p_startIndicies,  p_indexCounts, p_materialIndicies),
		verticies(p_verticies) {};
};


struct ModelRiggedResource : ModelResource
{
	// Full spec verticies with rigging data.
	cs::List<VertexRigged> verticies;
	Armature armature;

	// Vertex buffer function (rigged (and textured) verticies)
	void CreateVertexBuffer(ComPtr<ID3D11Device>& device);

	// Constructor.
	ModelRiggedResource(cs::List<VertexRigged> p_verticies, cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		ModelResource(p_indicies, p_startIndicies, p_indexCounts, p_materialIndicies),
		verticies(p_verticies) {};
};