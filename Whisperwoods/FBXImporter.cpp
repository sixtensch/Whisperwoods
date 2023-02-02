#include "core.h"
// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#include "FBXImporter.h"
#include "Vertex.h"
#include "Mesh.h"

DirectX::XMFLOAT4X4 ConvertToDirectX(aiMatrix4x4* mat)
{
	DirectX::XMFLOAT4X4 returnMat;
	returnMat._11 = mat->a1;
	returnMat._12 = mat->a2;
	returnMat._13 = mat->a3;
	returnMat._14 = mat->a4;

	returnMat._21 = mat->b1;
	returnMat._22 = mat->b2;
	returnMat._23 = mat->b3;
	returnMat._24 = mat->b4;

	returnMat._31 = mat->c1;
	returnMat._32 = mat->c2;
	returnMat._33 = mat->c3;
	returnMat._34 = mat->c4;

	returnMat._41 = mat->d1;
	returnMat._42 = mat->d2;
	returnMat._43 = mat->d3;
	returnMat._44 = mat->d4;

	return returnMat;
}

DirectX::XMFLOAT4X4 ConvertToDirectX2(aiMatrix4x4* mat)
{
	DirectX::XMFLOAT4X4 returnMat;
	returnMat._11 = mat->a1;
	returnMat._12 = mat->b1;
	returnMat._13 = mat->c1;
	returnMat._14 = mat->d1;

	returnMat._21 = mat->a2;
	returnMat._22 = mat->b2;
	returnMat._23 = mat->c2;
	returnMat._24 = mat->d2;

	returnMat._31 = mat->a3;
	returnMat._32 = mat->b3;
	returnMat._33 = mat->c3;
	returnMat._34 = mat->d3;

	returnMat._41 = mat->a4;
	returnMat._42 = mat->b4;
	returnMat._43 = mat->c4;
	returnMat._44 = mat->d4;

	return returnMat;
}

aiMatrix4x4 ConvertToAssImp(DirectX::XMFLOAT4X4* mat)
{
	aiMatrix4x4 returnMat;
	returnMat.a1 = mat->_11;
	returnMat.a2 = mat->_12;
	returnMat.a3 = mat->_13;
	returnMat.a4 = mat->_14;
	returnMat.b1 = mat->_21;
	returnMat.b2 = mat->_22;
	returnMat.b3 = mat->_23;
	returnMat.b4 = mat->_24;
	returnMat.c1 = mat->_31;
	returnMat.c2 = mat->_32;
	returnMat.c3 = mat->_33;
	returnMat.c4 = mat->_34;
	returnMat.d1 = mat->_41;
	returnMat.d2 = mat->_42;
	returnMat.d3 = mat->_43;
	returnMat.d4 = mat->_44;
	return returnMat;
}


void FBXImporter::ImportFBXStatic(std::string filePath, MeshStatic* outMesh)
{
	Assimp::Importer importer;
	LOG_TRACE("Starting FBX Import for file:");
	LOG_TRACE(filePath.c_str());
	const aiScene* scene = importer.ReadFile(filePath,
		/*aiProcess_MakeLeftHanded |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType*/aiProcess_CalcTangentSpace | aiProcess_Triangulate);
	if (scene == nullptr) 
	{
		LOG_ERROR("THERE WAS AN FBX IMPORT ERROR:");
		LOG_TRACE(importer.GetErrorString());
		return;
	}
	std::string answerString = (scene->HasMeshes()) ? "True" : "False";
	std::string traceString1 = "ASSIMP: opened file: "  + filePath + ", has meshes? - " + answerString;
	std::string traceString2 = "Number of meshes: " + scene->mNumMeshes;
	LOG_TRACE(traceString1.c_str());
	LOG_TRACE(traceString2.c_str());

	if (!scene->HasMeshes())
	{
		LOG_WARN("proposed .fbx does not contain any meshes.");
		return;
	}

	int indexCounter = 0;
	int subMeshCounter = 0;

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* newMesh = scene->mMeshes[i];
		// In case of empty, continue
		if (!newMesh->HasFaces())
			continue;

		int startIndex = indexCounter;
		outMesh->startIndicies.Add(startIndex);

		for (int j = 0; j < newMesh->mNumVertices; j++)
		{
			aiVector3D currentVert = newMesh->mVertices[j];
			aiVector3D currentNorm = newMesh->mNormals[j];
			aiVector3D currentTan = newMesh->mTangents[j];
			aiVector3D currentBiTan = newMesh->mBitangents[j];
			aiVector3D currentUV = newMesh->mTextureCoords[0][j];
			//aiVector3D currentUV = currentUVSet[0];

			VertexTextured newVertex(
				Vec3(currentVert.x, currentVert.y, currentVert.z), // Position
				Vec3(currentNorm.x, currentNorm.y, currentNorm.z), // Normal
				Vec3(currentTan.x, currentTan.y, currentTan.z), // Tangent
				Vec3(currentBiTan.x, currentBiTan.y, currentBiTan.z), // Bitangent
				Vec2(currentUV.x, currentUV.y), // UV
				Vec2(0,0)); // Padding
			
			outMesh->verticies.Add(newVertex);
			outMesh->indicies.Add(indexCounter);
			indexCounter++;
		}
		outMesh->indexCounts.Add(indexCounter - startIndex);
		subMeshCounter++;
	}


	//return Mesh();
}

void FBXImporter::ImportFBXRigged(std::string filePath, MeshRigged* outMesh)
{
}
