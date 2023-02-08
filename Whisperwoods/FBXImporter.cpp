#include "core.h"
// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#include "FBXImporter.h"
#include "Vertex.h"
#include "ModelResource.h"
#include "Armature.h"

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
Mat4 ConvertToMat4(aiMatrix4x4* mat)
{
	Mat4 returnMat;
	returnMat(0, 0) = mat->a1;
	returnMat(0, 1) = mat->a2;
	returnMat(0, 2) = mat->a3;
	returnMat(0, 3) = mat->a4;

	returnMat(1, 0) = mat->b1;
	returnMat(1, 1) = mat->b2;
	returnMat(1, 2) = mat->b3;
	returnMat(1, 3) = mat->b4;

	returnMat(2, 0) = mat->c1;
	returnMat(2, 1) = mat->c2;
	returnMat(2, 2) = mat->c3;
	returnMat(2, 3) = mat->c4;

	returnMat(3, 0) = mat->d1;
	returnMat(3, 1) = mat->d2;
	returnMat(3, 2) = mat->d3;
	returnMat(3, 3) = mat->d4;
	return returnMat;
}
Mat4 ConvertToMat4_2(aiMatrix4x4* mat)
{
	Mat4 returnMat;
	returnMat(0, 0) = mat->a1;
	returnMat(0, 1) = mat->b1;
	returnMat(0, 2) = mat->c1;
	returnMat(0, 3) = mat->d1;
	returnMat(1, 0) = mat->a2;
	returnMat(1, 1) = mat->b2;
	returnMat(1, 2) = mat->c2;
	returnMat(1, 3) = mat->d2;
	returnMat(2, 0) = mat->a3;
	returnMat(2, 1) = mat->b3;
	returnMat(2, 2) = mat->c3;
	returnMat(2, 3) = mat->d3;
	returnMat(3, 0) = mat->a4;
	returnMat(3, 1) = mat->b4;
	returnMat(3, 2) = mat->c4;
	returnMat(3, 3) = mat->d4;
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
aiMatrix4x4 ConvertToAssImp(Mat4& mat)
{
	aiMatrix4x4 returnMat;
	returnMat.a1 = mat(0, 0);
	returnMat.a2 = mat(0, 1);
	returnMat.a3 = mat(0, 2);
	returnMat.a4 = mat(0, 3);
	returnMat.b1 = mat(1, 0);
	returnMat.b2 = mat(1, 1);
	returnMat.b3 = mat(1, 2);
	returnMat.b4 = mat(1, 3);
	returnMat.c1 = mat(2, 0);
	returnMat.c2 = mat(2, 1);
	returnMat.c3 = mat(2, 2);
	returnMat.c4 = mat(2, 3);
	returnMat.d1 = mat(3, 0);
	returnMat.d2 = mat(3, 1);
	returnMat.d3 = mat(3, 2);
	returnMat.d4 = mat(3, 3);
	return returnMat;
}

#define DXMAT DirectX::XMMATRIX
#define DX4X4 DirectX::XMFLOAT4X4
#define DX DirextX
#define MAT XMMATRIX
#define XMIDENT DirectX::XMMatrixIdentity()

bool FBXImporter::ImportFBXStatic(std::string filePath, ModelStaticResource* const outMesh)
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
		return false;
	}
	std::string answerString = (scene->HasMeshes()) ? "True" : "False";
	std::string traceString1 = "ASSIMP: opened file: "  + filePath + ", has meshes? - " + answerString;
	std::string traceString2 = "Number of meshes: " + scene->mNumMeshes;
	LOG_TRACE(traceString1.c_str());
	LOG_TRACE(traceString2.c_str());

	if (!scene->HasMeshes())
	{
		LOG_WARN("proposed .fbx does not contain any meshes.");
		return false;
	}

	int indexCounter = 0;
	int numIndexCounter = 0;
	int subMeshCounter = 0;

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* newMesh = scene->mMeshes[i];
		// In case of empty, continue
		if (!newMesh->HasFaces())
			continue;
		
		std::string traceString3 = "Processing sub-mesh: " + std::string(newMesh->mName.C_Str());
		LOG_TRACE(traceString3.c_str());

		// Add material name to the material name list.
		outMesh->materialNames.Add(std::string(scene->mMaterials[newMesh->mMaterialIndex]->GetName().C_Str()));

		int startIndex = numIndexCounter;
		outMesh->startIndicies.Add(startIndex);

		for (unsigned int j = 0; j < newMesh->mNumVertices; j++)
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
			
			// do existing vertex checks.
			int existingVertIndex = -1;
			for (unsigned int k = 0; k < outMesh->verticies.Size(); k++)
			{
				if (outMesh->verticies[k] == newVertex)
				{
					existingVertIndex = k;
					break;
				}
			}

			// If it's a new vertex, add it and the index.
			if (existingVertIndex == -1)
			{
				outMesh->verticies.Add(newVertex);
				outMesh->indicies.Add(indexCounter);
				indexCounter++;
			}
			else
			{
				outMesh->indicies.Add(existingVertIndex);
			}
			numIndexCounter++;
		}
		outMesh->indexCounts.Add(numIndexCounter - startIndex);
		subMeshCounter++;
	}

	return true;
}

void RecursiveNodeFetch(aiNode* node, std::vector<aiNode*>& outputVector)
{
	for (int i = 0; i < node->mNumChildren; i++)
	{
		RecursiveNodeFetch(node->mChildren[i], outputVector);
	}
	outputVector.push_back(node);
}

std::vector<aiNode*> GetNodeVector(const aiScene* scene)
{
	std::vector<aiNode*> output;
	RecursiveNodeFetch(scene->mRootNode, output);
	return output;
}

bool FBXImporter::ImportFBXRigged(std::string filePath, ModelRiggedResource* const outMesh)
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
		return false;
	}
	std::string answerString = (scene->HasMeshes()) ? "True" : "False";
	std::string traceString1 = "ASSIMP: opened file: " + filePath + ", has meshes? - " + answerString;
	std::string traceString2 = "Number of meshes: " + scene->mNumMeshes;
	LOG_TRACE(traceString1.c_str());
	LOG_TRACE(traceString2.c_str());

	if (!scene->HasMeshes())
	{
		LOG_WARN("proposed .fbx does not contain any meshes.");
		return false;
	}

	int indexCounter = 0;
	int numIndexCounter = 0;
	int subMeshCounter = 0;

	outMesh->armature.globalInverseTransform = ConvertToMat4(&scene->mRootNode->mTransformation.Inverse().Transpose());


	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* newMesh = scene->mMeshes[i];

		if (!newMesh->HasFaces())
			continue;

		if (!newMesh->HasBones())
			continue;


		std::string traceString3 = "Processing sub-mesh: " + std::string(newMesh->mName.C_Str());
		LOG_TRACE(traceString3.c_str());

		std::vector<aiNode*> nodes = GetNodeVector(scene);

		outMesh->armature.InitializeMatrix(newMesh->mNumBones);

		std::vector<aiBone*> boneList;
		std::vector<std::string> boneNames;
		std::vector<std::string> parentNames;
		std::vector<int> parentIndicies;
		std::vector<Mat4> bindLocalTxList;
		std::vector<Mat4> bindModelTxList;
		std::vector<Mat4> inverseBindTxList;

		aiMatrix4x4 identity = aiMatrix4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		Mat4 identityDx = ConvertToMat4(&identity);
		int numBones = newMesh->mNumBones;

		// Fill Vectors as makeshift arrays
		for (int j = 0; j < numBones; j++)
		{
			boneList.push_back(newMesh->mBones[j]);
			boneNames.push_back(std::string(newMesh->mBones[j]->mName.C_Str()));
			parentNames.push_back(std::string(newMesh->mBones[j]->mNode->mParent->mName.C_Str()));
			parentIndicies.push_back(-1);
			bindLocalTxList.push_back(identityDx);
			bindModelTxList.push_back(identityDx);
			inverseBindTxList.push_back(identityDx);
		}

		// Fill local matricies
		for (int j = 0; j < numBones; j++)
		{
			aiMatrix4x4 localMat = newMesh->mBones[j]->mNode->mTransformation.Transpose();
			bindLocalTxList[j] = ConvertToMat4(&localMat);
		}

		// Resolve parent indicies
		for (int j = 1; j < numBones; j++)
		{
			for (size_t k = 0; k < numBones; k++)
			{
				if (parentNames[j] == boneNames[k])
				{
					parentIndicies[j] = k;
					break;
				}
			}
		}

		// bindModelTx = parentBindModelTx * bindLocalTx;
		// inverseBindModelTx = inverse(bindModelTx)

		// Special case for root
		bindModelTxList[0] = bindLocalTxList[0];
		//Mat4 ibMat = bindModelTxList[0].in
		//DXMAT mMatXM = DirectX::XMLoadFloat4x4(&bindModelTxList[0]);
		DXMAT ibMatXM = DirectX::XMMatrixInverse(nullptr, bindModelTxList[0].XMMatrix());
		inverseBindTxList[0] = Mat4(ibMatXM);
		// Resolve rest of bones
		for (int j = 1; j < numBones; j++)
		{
			//PrintMatrix(bindModelTxList[parentIndicies[j]], boneNames[j] + " - Parent Tx");
			DXMAT pMatXM = bindModelTxList[parentIndicies[j]].XMMatrix();
			//PrintMatrix(bindLocalTxList[j], boneNames[j] + " - Local Tx");
			DXMAT lMatXM = bindLocalTxList[j].XMMatrix();
			DXMAT mMatXM = DirectX::XMMatrixMultiply(lMatXM, pMatXM);
			bindModelTxList[j] = Mat4(mMatXM);
			//DirectX::XMStoreFloat4x4(&bindModelTxList[j], mMatXM);
			//PrintMatrix(bindModelTxList[j], boneNames[j] + " - Model Tx");

			DXMAT ibMatXM = DirectX::XMMatrixInverse(nullptr, mMatXM);
			inverseBindTxList[j] = Mat4(ibMatXM);
			//DirectX::XMStoreFloat4x4(&inverseBindTxList[j], ibMatXM);
			//PrintMatrix(inverseBindTxList[j], boneNames[j] + " - Inverse Draw Tx");
			//PrintMatrix(ConvertToDirectX(&newMesh->mBones[j]->mOffsetMatrix), boneNames[j] + " - Imported Inverse Draw Tx");
		}

		// Create Bones  TODO: fix for submeshes
		for (int j = 0; j < numBones; j++)
		{
			Bone nBone;
			nBone.name = boneNames[j];
			nBone.parentName = parentNames[j];
			nBone.parentIndex = parentIndicies[j];
			nBone.inverseBindMatrix = inverseBindTxList[j];
			outMesh->armature.bones.Add(nBone);
		}

		// Get Weights  TODO: fix for submeshes
		for (int j = 0; j < newMesh->mNumBones; j++)
		{
			VertexGroup newGroup;
			newGroup.name = boneList[j]->mName.C_Str();
			newGroup.index = j;
			int numWeights = newMesh->mBones[j]->mNumWeights;
			for (size_t k = 0; k < newMesh->mBones[j]->mNumWeights; k++)
			{
				VertexWeightPair newVertexWeight;
				newVertexWeight.vertex = newMesh->mBones[j]->mWeights[k].mVertexId;
				newVertexWeight.weight = newMesh->mBones[j]->mWeights[k].mWeight;
				newGroup.verticies.Add(newVertexWeight);
			}
			outMesh->vertexGroups.Add(newGroup);
		}



		// Get mesh, TODO: fix for submeshes

		// Add material name to the material name list.
		outMesh->materialNames.Add(std::string(scene->mMaterials[newMesh->mMaterialIndex]->GetName().C_Str()));
		int startIndex = numIndexCounter;
		outMesh->startIndicies.Add(startIndex);

		for (unsigned int j = 0; j < newMesh->mNumVertices; j++)
		{
			aiVector3D currentVert = newMesh->mVertices[j];
			aiVector3D currentNorm = newMesh->mNormals[j];
			aiVector3D currentTan = newMesh->mTangents[j];
			aiVector3D currentBiTan = newMesh->mBitangents[j];
			aiVector3D currentUV = newMesh->mTextureCoords[0][j];
			//aiVector3D currentUV = currentUVSet[0];

			VertexRigged newVertex(
				Vec3(currentVert.x, currentVert.y, currentVert.z), // Position
				Vec3(currentNorm.x, currentNorm.y, currentNorm.z), // Normal
				Vec3(currentTan.x, currentTan.y, currentTan.z), // Tangent
				Vec3(currentBiTan.x, currentBiTan.y, currentBiTan.z), // Bitangent
				Vec2(currentUV.x, currentUV.y), // UV
				Vec2(0, 0), // Padding
				Point4(0, 0, 0, 0), // Bones (needs assigning later)
				Vec4(0, 0,0, 0)); // Weights (needs assigning later)

			// do existing vertex checks.
			int existingVertIndex = -1;
			for (unsigned int k = 0; k < outMesh->verticies.Size(); k++)
			{
				if (outMesh->verticies[k] == newVertex)
				{
					existingVertIndex = k;
					break;
				}
			}

			// If it's a new vertex, add it and the index.
			if (existingVertIndex == -1)
			{
				outMesh->verticies.Add(newVertex);
				outMesh->indicies.Add(indexCounter);
				indexCounter++;
			}
			else
			{
				outMesh->indicies.Add(existingVertIndex);
			}
			numIndexCounter++;
		}
		outMesh->indexCounts.Add(numIndexCounter - startIndex);
		subMeshCounter++;

		outMesh->armature.ResolveBoneParentIndicies();
	}
	return true;
}

bool FBXImporter::ImportFBXAnimations(std::string filePath, AnimationResource* const outAnimations)
{
	return false;
}
