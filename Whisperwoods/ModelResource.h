#pragma once
#include <CHSL/List.h>
#include "Vertex.h"
#include "Armature.h"
#include <String>
#include "BasicResource.h"



// Move this into a model print function
	/*for (size_t i = 0; i < modelPointer->startIndicies.Size(); i++)
	{
		LOG_TRACE("SubMeshMaterial[%d]: %s", i, modelPointer->materialNames[i].c_str());
		for (size_t j = 0; j < modelPointer->indexCounts[i]; j++)
		{
			int indexIndex = modelPointer->startIndicies[i] + j;
			int indicie = modelPointer->indicies[indexIndex];
			Vec3 pos = modelPointer->verticies[indicie].pos;
			Vec3 nor = modelPointer->verticies[indicie].nor;
			LOG_TRACE("Index[%d]: %d Vertex[%d]: pos: (%.2f, %.2f, %.2f) nor: (%.2f, %.2f, %.2f)",
				j,
				indicie,
				indicie,
				pos.x, pos.y, pos.z,
				nor.x, nor.y, nor.z);
		}
	}*/



// TODO: Add instantiation of default values for every member.
struct ModelResource : public BasicResource
{
	// Raw verticies and indicies from the model defining the mesh and submeshes
	cs::List<int> indicies;

	// These contain the startindex in the indexlist, the count, and materialindex for the submeshes of the mesh.
	cs::List<int> startIndicies;
	cs::List<int> indexCounts;
	cs::List<int> materialIndicies;
	cs::List<int> materials; // TODO: Verify when material exists.
	cs::List<std::string> materialNames;

	// Vertex and index buffers
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;

protected:
	// Index buffer function (is called by CreateVertexBuffer)
	void CreateIndexBuffer(ID3D11Device* device);
public:

	// Constructors.
	ModelResource() = default;
	ModelResource(std::string name) : BasicResource(name) {}

	ModelResource(cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		BasicResource("Model"),
		indicies(p_indicies),
		startIndicies(p_startIndicies),
		indexCounts(p_indexCounts),
		materialIndicies(p_materialIndicies) {};
};

struct ModelStaticResource : public ModelResource
{
	// Textured verticies, that do not contain rigging data.
	cs::List<VertexTextured> verticies;

	// Vertex buffer function (unrigged (only textured) verticies)
	void CreateVertexBuffer(ID3D11Device* device);

	// Constructor.
	ModelStaticResource() = default;
	ModelStaticResource(std::string name) : ModelResource(name) {}

	ModelStaticResource(cs::List<VertexTextured> p_verticies, cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		ModelResource(p_indicies, p_startIndicies,  p_indexCounts, p_materialIndicies),
		verticies(p_verticies) {};

};


struct ModelRiggedResource : public ModelResource
{
	// Full spec verticies with rigging data.
	cs::List<VertexRigged> verticies;

	// Armature and vertex groups, bones and the verticies
	Armature armature;
	//cs::List<VertexGroup> vertexGroups;

	// Vertex buffer function (rigged (and textured) verticies)
	void CreateVertexBuffer(ID3D11Device* device);

	// Constructor.
	ModelRiggedResource() = default;
	ModelRiggedResource(std::string name) : ModelResource(name) {}

	ModelRiggedResource(cs::List<VertexRigged> p_verticies, cs::List<int> p_indicies, cs::List<int> p_startIndicies, cs::List<int> p_indexCounts, cs::List<int> p_materialIndicies) :
		ModelResource(p_indicies, p_startIndicies, p_indexCounts, p_materialIndicies),
		verticies(p_verticies) {};

};