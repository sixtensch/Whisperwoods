#include "core.h"
// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

#include "FBXImporter.h"
#include "Vertex.h"
#include "ModelResource.h"
#include "Armature.h"
#include "WhisperWoodsModelHead.h"
#include <fstream>
#include <filesystem>
#include <direct.h>

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

bool IsSame(VertexTextured a, VertexTextured b)
{
	if (a.pos == b.pos && a.nor == b.nor) return true;
	return false;
}

struct FixedSizeCharArray 
{
	char data[128] = { '\0' };
};

//void print_state(const std::ios& stream) {
//	std::cout << " good()=" << stream.good();
//	std::cout << " eof()=" << stream.eof();
//	std::cout << " fail()=" << stream.fail();
//	std::cout << " bad()=" << stream.bad();
//}

std::string FBXImporter::SaveWMM(ModelStaticResource* inMesh, std::string subDir)
{
	// Use model name as base for out name
	std::string outName = subDir + inMesh->name + ".wwm";
	//namespace fs = std::filesystem;
	_mkdir(subDir.c_str()); // TODO: Maybe remove
	// Open the writer
	std::fstream writer;
	writer.open(outName, std::fstream::out | std::fstream::binary);

	if (!writer)
	{
		LOG_WARN("WMM Writer failed to open file: %s", outName.c_str());
		return "fail";
	}

	// Gather the head data
	WhisperWoodsModelHead headData;
	headData.rigged = false;
	headData.numVerticies = inMesh->verticies.Size();
	headData.numIndicies = inMesh->indicies.Size();
	headData.numSubMeshes = inMesh->startIndicies.Size();

	// Write the head information
	writer.write((char*)&headData, sizeof(WhisperWoodsModelHead));

	// Write startIndicies
	writer.write((char*)inMesh->startIndicies.Data(), sizeof(int) * headData.numSubMeshes);

	// Write indexCounts
	writer.write((char*)inMesh->indexCounts.Data(), sizeof(int) * headData.numSubMeshes);
	
	// Convert the material names to a fixed buffersize
	cs::List<FixedSizeCharArray> nameList;
	for (int i = 0; i < headData.numSubMeshes; i++)
	{
		FixedSizeCharArray buffer;
		int length = (inMesh->materialNames[i].length() <= 128) ? inMesh->materialNames[i].length() : 128;
		for (int j = 0; j < length; j++)
		{
			buffer.data[j] = inMesh->materialNames[i][j];
		}
		nameList.Add(buffer);
	}

	// Write materialNames
	writer.write((char*)nameList.Data(), sizeof(FixedSizeCharArray) * headData.numSubMeshes);

	// Write verticies
	writer.write((char*)inMesh->verticies.Data(), sizeof(VertexTextured) * headData.numVerticies);

	// Write indicies
	writer.write((char*)inMesh->indicies.Data(), sizeof(int) * headData.numIndicies);

	// Close the writer
	writer.close();

	// If an error occured
	if (!writer.good()) {
		LOG_WARN("WMM Static Writer error occurred at writing time! , EOF: %d, FAILBIT: %d, BADBIT: %d", writer.eof(), writer.fail(), writer.bad());
		return "fail";
	}

	// Return the path
	return outName;
}

std::string FBXImporter::SaveWMM(ModelRiggedResource* inMesh, std::string subDir)
{
	// Use model name as base for out name
	std::string outName = subDir + inMesh->name + ".wwm";
	_mkdir(subDir.c_str());
	// Open the writer
	std::ofstream writer(outName, std::ios::out | std::ios::binary);

	// Gather the head data
	WhisperWoodsModelHead headData;
	headData.rigged = true;
	headData.numVerticies = inMesh->verticies.Size();
	headData.numIndicies = inMesh->indicies.Size();
	headData.numSubMeshes = inMesh->startIndicies.Size();

	// Write the head information
	writer.write((char*)&headData, sizeof(WhisperWoodsModelHead));

	// Write startIndicies
	writer.write((char*)inMesh->startIndicies.Data(), sizeof(int) * headData.numSubMeshes);
	// Write indexCounts
	writer.write((char*)inMesh->indexCounts.Data(), sizeof(int) * headData.numSubMeshes);

	// Convert the material names to a fixed buffersize
	cs::List<FixedSizeCharArray> nameList;
	for (int i = 0; i < headData.numSubMeshes; i++)
	{
		FixedSizeCharArray buffer;
		int length = (inMesh->materialNames[i].length() <= 128) ? inMesh->materialNames[i].length() : 128;
		for (int j = 0; j < length; j++)
		{
			buffer.data[j] = inMesh->materialNames[i][j];
		}
		nameList.Add(buffer);
	}

	// Write materialNames
	writer.write((char*)nameList.Data(), sizeof(FixedSizeCharArray) * headData.numSubMeshes);

	// Write verticies
	writer.write((char*)inMesh->verticies.Data(), sizeof(VertexRigged) * headData.numVerticies);

	// Write indicies
	writer.write((char*)inMesh->indicies.Data(), sizeof(int) * headData.numIndicies);

	// Close the writer
	writer.close();

	// If an error occured
	if (!writer.good()) {
		LOG_WARN("WMM Rigged Writer error occurred at writing time! , EOF: %d, FAILBIT: %d, BADBIT: %d", writer.eof(), writer.fail(), writer.bad());
		return "fail";
	}

	// Return the path
	return outName;
}

bool FBXImporter::LoadWWMStatic(std::string filePath, ModelStaticResource* const outMesh)
{
	LOG_TRACE("Loading Static WMM: %s ...", filePath.c_str());
	std::ifstream reader(filePath, std::ifstream::out | std::ifstream::binary);
	if (!reader)
	{
		LOG_WARN("WMM Reader failed to open file: %s", filePath.c_str());
		return false;
	}

	// Read the head data.
	WhisperWoodsModelHead headData;
	reader.read((char*)&headData, sizeof(WhisperWoodsModelHead));

	if (headData.rigged)
	{
		LOG_WARN("WMM Reader Static read file: %s which was saved as rigged, aborted.", filePath.c_str());
		return false;
	}

	//Read the startindicies
	reader.read((char*)outMesh->startIndicies.MassAdd(headData.numSubMeshes), sizeof(int) * headData.numSubMeshes);

	//Read the indexCounts
	reader.read((char*)outMesh->indexCounts.MassAdd(headData.numSubMeshes), sizeof(int) * headData.numSubMeshes);

	// Temp store
	cs::List<FixedSizeCharArray> readNames;

	//Read the names
	reader.read((char*)readNames.MassAdd(headData.numSubMeshes), sizeof(FixedSizeCharArray) * headData.numSubMeshes);

	// Convert and add to the outmesh
	for (int i = 0; i < headData.numSubMeshes; i++)
	{
		outMesh->materialNames.Add(std::string(readNames[i].data));
		outMesh->materialIndicies.Add(i); // Add the indicies.
		outMesh->materials.Add(i); // Add the indicies. TODO: FIX FOR BLANK REFERNCES TO THE RESOURCE STUFF
	}

	//Read the verticies
	reader.read((char*)outMesh->verticies.MassAdd(headData.numVerticies), sizeof(VertexTextured) * headData.numVerticies);

	//Read the indicies
	reader.read((char*)outMesh->indicies.MassAdd(headData.numIndicies), sizeof(int) * headData.numIndicies);

	// close the reader
	reader.close();

	if (!reader.good())
	{
		LOG_WARN("WMM Static reader, error found at close, data might be garbled.");
		return false;
	}
	LOG_TRACE("DONE");
	return true;
}

bool FBXImporter::LoadWWMRigged(std::string filePath, ModelRiggedResource* const outMesh)
{
	LOG_TRACE("Loadiing Rigged WMM: %s ...", filePath.c_str());
	std::ifstream reader(filePath, std::ios::out | std::ios::binary);
	if (!reader)
	{
		LOG_WARN("WMM Reader failed to open file: %s", filePath.c_str());
		return false;
	}

	// Read the head data.
	WhisperWoodsModelHead headData;
	reader.read((char*)&headData, sizeof(WhisperWoodsModelHead));

	if (!headData.rigged)
	{
		LOG_WARN("WMM Reader Rigged read file: %s which was saved as rigged, aborted.", filePath.c_str());
		return false;
	}

	//Read the startindicies
	reader.read((char*)outMesh->startIndicies.MassAdd(headData.numSubMeshes), sizeof(int) * headData.numSubMeshes);

	//Read the indexCounts
	reader.read((char*)outMesh->indexCounts.MassAdd(headData.numSubMeshes), sizeof(int) * headData.numSubMeshes);

	// Temp store
	cs::List<FixedSizeCharArray> readNames;

	//Read the names
	reader.read((char*)readNames.MassAdd(headData.numSubMeshes), sizeof(FixedSizeCharArray) * headData.numSubMeshes);

	// Convert and add to the outmesh
	for (int i = 0; i < headData.numSubMeshes; i++)
	{
		outMesh->materialNames.Add(std::string(readNames[i].data));
		outMesh->materialIndicies.Add(i); // Add the indicies.
		outMesh->materials.Add(i); // Add the indicies. TODO: FIX FOR BLANK REFERNCES TO THE RESOURCE STUFF
	}

	//Read the verticies
	reader.read((char*)outMesh->verticies.MassAdd(headData.numVerticies), sizeof(VertexRigged) * headData.numVerticies);

	//Read the indicies
	reader.read((char*)outMesh->indicies.MassAdd(headData.numIndicies), sizeof(int) * headData.numIndicies);

	reader.close();

	if (!reader.good())
	{
		LOG_WARN("WMM Rigged reader, error found at close, data might be garbled.");
		return false;
	}
	LOG_TRACE("DONE");
	return true;
}

bool FBXImporter::ImportFBXStatic(std::string filePath, ModelStaticResource* const outMesh)
{
	Assimp::Importer importer;
	LOG_TRACE("\nStarting static FBX Import for file: %s", filePath.c_str());
	const aiScene* scene = importer.ReadFile(filePath, /*aiProcessPreset_TargetRealtime_MaxQuality*/
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_GenUVCoords | 
		aiProcess_JoinIdenticalVertices | 
		aiProcess_CalcTangentSpace | 
		aiProcess_Triangulate | 
		aiProcess_SortByPType
	); // chonky preset
	//aiProcessPreset_TargetRealtime_MaxQuality
	//const aiScene* scene = importer.ReadFile(filePath,
	//	/*aiProcess_MakeLeftHanded |
	//	aiProcess_CalcTangentSpace |
	//	aiProcess_Triangulate |
	//	aiProcess_JoinIdenticalVertices |
	//	aiProcess_SortByPType*/ /*aiProcessPreset_TargetRealtime_MaxQuality );*/ // chonky preset

	if (scene == nullptr) 
	{
		LOG_ERROR("THERE WAS AN FBX IMPORT ERROR:");
		LOG_TRACE(importer.GetErrorString());
		return false;
	}

	std::string answerString = (scene->HasMeshes()) ? "True" : "False";
	LOG_TRACE( "ASSIMP: opened file: %s, has meshes? - %s", filePath.c_str(), answerString.c_str() );
	LOG_TRACE( "Number of meshes: %d\n", scene->mNumMeshes );

	if (!scene->HasMeshes())
	{
		LOG_WARN("proposed .fbx does not contain any meshes.");
		return false;
	}

	int indexCounter = 0;
	int numIndexCounter = 0;
	int subMeshCounter = 0;
	int vertexCounter = 0;

	LOG_TRACE( "==STATIC IMPORT PROCESS START==\n", scene->mNumMeshes );

	// Get name from the input path
	outMesh->name = remove_extension(base_name(filePath));

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* newMesh = scene->mMeshes[i];
		// In case of empty, continue
		if (!newMesh->HasFaces())
			continue;
		
		LOG_TRACE( "	Processing submesh %s - material: %s - Num incomming verts: %d",
			newMesh->mName.C_Str(),
			scene->mMaterials[newMesh->mMaterialIndex]->GetName().C_Str(),
			newMesh->mNumVertices );

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
				Vec4(currentUV.x, currentUV.y, subMeshCounter, 0)); // UB, submeshIndex and Padding, submesh index is important for differentiating identical verticies of different materials.

			//int existingIndex = -1;
			//for (unsigned int k = 0; k < outMesh->verticies.Size(); k++)
			//{
			//	if (IsSame(outMesh->verticies[k], newVertex))
			//	{	
			//		//LOG_TRACE("%d is same as %d", j, k);
			//		existingIndex = k;
			//		break;
			//	}
			//}

			//if (existingIndex == -1)
			//{
			//	//LOG_TRACE("Added Vertex [%d] P(%f,%f,%f)", outMesh->verticies.Size(), newVertex.pos.x, newVertex.pos.y, newVertex.pos.z);
			//	outMesh->verticies.Add(newVertex); // Base verticies
			//	outMesh->indicies.Add(indexCounter);
			//	indexCounter++;
			//}
			//else
			//{
			//	outMesh->indicies.Add(existingIndex);
			//}
			//numIndexCounter++;
			// Add to the output vertex list
			outMesh->verticies.Add(newVertex);
		}

		// Process faces - indicies
		for (unsigned int j = 0; j < newMesh->mNumFaces; j++)
		{
			for (unsigned int k = 0; k < newMesh->mFaces[j].mNumIndices; k++)
			{
				outMesh->indicies.Add(vertexCounter+newMesh->mFaces[j].mIndices[k]);
				numIndexCounter++;
			}
			if (newMesh->mFaces[j].mNumIndices != 3)
			{
				LOG_TRACE("Face %d had wierd number of indicies: %d", j, newMesh->mFaces[j].mNumIndices);
			}
		}

		LOG_TRACE( "	Num indicies: %d, Num verts: ", numIndexCounter - startIndex, outMesh->verticies.Size() );
		outMesh->indexCounts.Add(numIndexCounter - startIndex);
		vertexCounter = outMesh->verticies.Size();
		subMeshCounter++;
	}
	LOG_TRACE( "Done\n" );
	LOG_TRACE( "Static Import Completed! Num verts: %d\n", outMesh->verticies.Size() );

	return true;
}

void RecursiveNodeFetch(aiNode* node, std::vector<aiNode*>& outputVector)
{
	for (int i = 0; i < node->mNumChildren; i++)
	{
		RecursiveNodeFetch(node->mChildren[i], outputVector);
	}
	//LOG_TRACE( "Node: %s", node->mName.C_Str() );
	outputVector.push_back(node);
}

std::vector<aiNode*> GetNodeVector(const aiScene* scene)
{
	std::vector<aiNode*> output;
	RecursiveNodeFetch(scene->mRootNode, output);
	return output;
}

bool Compare( BoneWeightPair i, BoneWeightPair j )
{
	return (i > j);
}

// Quite heavy function.
bool FBXImporter::ImportFBXRigged(std::string filePath, ModelRiggedResource* const outMesh)
{
	Assimp::Importer importer;
	LOG_TRACE("\nStarting Rigged FBX Import for file: %s", filePath.c_str());
	const aiScene* scene = importer.ReadFile(filePath,
		/*aiProcess_MakeLeftHanded |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType*/aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData);
	if (scene == nullptr)
	{
		LOG_ERROR("THERE WAS AN FBX IMPORT ERROR:");
		LOG_TRACE(importer.GetErrorString());
		return false;
	}
	std::string answerString = (scene->HasMeshes()) ? "True" : "False";
	LOG_TRACE( "ASSIMP: opened file: %s, has meshes? - %s", filePath.c_str(), answerString.c_str() );
	LOG_TRACE( "Number of meshes: %d\n", scene->mNumMeshes );

	if (!scene->HasMeshes())
	{
		LOG_WARN("proposed .fbx does not contain any meshes.");
		return false;
	}

	LOG_TRACE( "==RIGGED IMPORT PROCESS START==\n");

	// Get name from the input path
	outMesh->name = remove_extension(base_name(filePath));

	outMesh->armature.globalInverseTransform = ConvertToMat4(&scene->mRootNode->mTransformation.Inverse().Transpose());

	// Armature and vertex groups
	// Setup some lists for the import.
	std::vector<aiNode*> nodes = GetNodeVector( scene );
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
		0, 0, 0, 1 );
	Mat4 identityDx = ConvertToMat4( &identity );
	int numBones = 0;

	LOG_TRACE( "Retrieving bones from all submeshes ..." );
	// Step 1: Initalize all the vectors with base data, gathering all the bones from all submeshes.
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* newMesh = scene->mMeshes[i];

		if (!newMesh->HasBones())
			continue;
		
		// Initialize vectors.
		for (int j = 0; j < newMesh->mNumBones; j++)
		{
			bool boneAlreadyExists = false;
			aiString parentName = (newMesh->mBones[j]->mNode == nullptr) ? aiString("No node") : (newMesh->mBones[j]->mNode->mParent == nullptr) ? aiString("No parent") : newMesh->mBones[j]->mNode->mParent->mName;
			
			for (int k = 0; k < boneNames.size(); k++)
			{
				if (boneNames[k] == std::string(newMesh->mBones[j]->mName.C_Str()))
				{
					boneAlreadyExists = true;
					break;
				}
			}
			if (!boneAlreadyExists)
			{
				outMesh->armature.boneMatricies.Add(Mat4()); // Init an empty matrix, might be redundant.
				boneList.push_back( newMesh->mBones[j] );
				boneNames.push_back( std::string( newMesh->mBones[j]->mName.C_Str() ) );
				parentNames.push_back( std::string( newMesh->mBones[j]->mNode->mParent->mName.C_Str() ) );
				parentIndicies.push_back( -1 );
				bindLocalTxList.push_back( identityDx );
				bindModelTxList.push_back( identityDx );
				inverseBindTxList.push_back( identityDx );
				numBones++;
				//LOG_TRACE( "%s - Bone [%d] Name: %s, Parent: %s Added", newMesh->mName.C_Str(), j, newMesh->mBones[j]->mName.C_Str(), parentName.C_Str() );
			}
		}
	}
	LOG_TRACE( "Done\n" );
	LOG_TRACE( "Processing local Matricies ..." );
	// Step 2: Fill local matricies
	for (int i = 0; i < numBones; i++)
	{
		aiMatrix4x4 localMat = boneList[i]->mNode->mTransformation.Transpose();
		bindLocalTxList[i] = ConvertToMat4( &localMat );
	}
	LOG_TRACE( "Done\n" );
	LOG_TRACE( "Resolving parent indicies ... " );
	// Step 3: Resolve parent indicies
	for (int i = 0; i < numBones; i++)
	{
		for (int j = 0; j < numBones; j++)
		{
			if (parentNames[i] == boneNames[j])
			{
				parentIndicies[i] = j;
				break;
			}
		}
		//LOG_TRACE( "Bone[%d]: %s, Parent: %s, Parent index: %d", i, boneNames[i].c_str(), parentNames[i].c_str(), parentIndicies[i]);
	}
	LOG_TRACE( "Done\n" );
	LOG_TRACE( "Calculating inverse bind matricies ... " );
	// Step 4: Calculate inverse bind matricies
	// 4.1 Special case for root(s) (no parent index)
	for (int i = 0; i < numBones; i++)
	{
		if (parentIndicies[i] == -1)
		{
			// Store the bind matrix as Mat4
			bindModelTxList[i] = bindLocalTxList[i];
			// Calculate the inverse
			DXMAT ibMatXM = DirectX::XMMatrixInverse(nullptr, bindModelTxList[i].XMMatrix());
			// Store the inverse bind matrix as Mat4
			inverseBindTxList[i] = Mat4(ibMatXM); // Convert to Mat4 and store as inverse
		}
	}

	// 4.2 Resolve rest of bones
	for (int j = 1; j < numBones; j++)
	{
		// Get parent matrix as XMMATRIX
		DXMAT pMatXM = bindModelTxList[parentIndicies[j]].XMMatrix();
		// Get local matrix as XMMATRIX
		DXMAT lMatXM = bindLocalTxList[j].XMMatrix();
		// Multiply local and parent matrix.
		DXMAT mMatXM = DirectX::XMMatrixMultiply( lMatXM, pMatXM );
		// Store model bind matrix as Mat4
		bindModelTxList[j] = Mat4(mMatXM);
		// Use DirectX to invert the local matrix
		DXMAT ibMatXM = DirectX::XMMatrixInverse( nullptr, mMatXM );
		// Store the inverse as Mat4
		inverseBindTxList[j] = Mat4( ibMatXM );
	}
	LOG_TRACE( "Done\n" );
	LOG_TRACE( "Adding output bones to armature ..." );
	// Step 5: Create and add output bones to the output armature
	for (int i = 0; i < numBones; i++)
	{
		Bone nBone;
		nBone.name = boneNames[i]; // Name
		nBone.parentName = parentNames[i]; // Parent Name
		nBone.parentIndex = parentIndicies[i]; // Parent Index
		nBone.inverseBindMatrix = inverseBindTxList[i]; // Inverse bind matrix TODO: This shows that this is the only matrix that needs to be saved (maybe clean up above).
		outMesh->armature.bones.Add( nBone );
	}
	LOG_TRACE( "Done\n" );

	// Reset vertex/index counters
	unsigned int indexCounter = 0;
	unsigned int numIndexCounter = 0;
	unsigned int subMeshCounter = 0;
	int vertexCounter = 0;

	LOG_TRACE( "Processing mesh and weights...\n" );
	// Step 6: process the verticies and indicies as with the static import, but using the rigged verticies.
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* newMesh = scene->mMeshes[i];
		if (!newMesh->HasFaces())
			continue;

		if (!newMesh->HasBones())
			continue;

		// Add material name to the material name list.
		outMesh->materialNames.Add(std::string( scene->mMaterials[newMesh->mMaterialIndex]->GetName().C_Str()));
		int startIndex = numIndexCounter;
		outMesh->startIndicies.Add(startIndex);

		LOG_TRACE( "	Processing: %s\n",
			scene->mMaterials[newMesh->mMaterialIndex]->GetName().C_Str(),
			newMesh->mNumVertices );
		LOG_TRACE( "		Processing vertex weights ... ");
		int weightCounter = 0;
		struct VertexBoneWeights
		{
			std::vector<BoneWeightPair> bones;
		};
		std::vector<VertexBoneWeights> vertexWeights;
		for (unsigned int j = 0; j < newMesh->mNumVertices; j++)
		{
			VertexBoneWeights newWeightVector;
			for (unsigned int k = 0; k < newMesh->mNumBones; k++)
			{
				for (unsigned int l = 0; l < newMesh->mBones[k]->mNumWeights; l++)
				{
					if (newMesh->mBones[k]->mWeights[l].mVertexId != j)
					{
						continue;
					}
					else
					{
						BoneWeightPair newPair;
						newPair.bone = k;
						newPair.weight = newMesh->mBones[k]->mWeights[l].mWeight;
						newWeightVector.bones.push_back( newPair );
						weightCounter++;
					}
				}
			}
			std::sort(newWeightVector.bones.begin(), newWeightVector.bones.end(), Compare);
			while (newWeightVector.bones.size() < 4) // TODO: change to max bones later maybe.
			{
				BoneWeightPair newPair;
				newPair.bone = 0;
				newPair.weight = 0;
				newWeightVector.bones.push_back( newPair );
				weightCounter++;
			}
			vertexWeights.push_back(newWeightVector);
		}
		LOG_TRACE( "		Done - N:%d", weightCounter );
		LOG_TRACE( "		Processing verticies ... ");
		for (unsigned int j = 0; j < newMesh->mNumVertices; j++)
		{
			aiVector3D currentVert = newMesh->mVertices[j];
			aiVector3D currentNorm = newMesh->mNormals[j];
			aiVector3D currentTan = newMesh->mTangents[j];
			aiVector3D currentBiTan = newMesh->mBitangents[j];
			aiVector3D currentUV = newMesh->mTextureCoords[0][j];
			//aiVector3D currentUV = currentUVSet[0];

			VertexRigged newVertex(
				Vec3( currentVert.x, currentVert.y, currentVert.z ), // Position
				Vec3( currentNorm.x, currentNorm.y, currentNorm.z ), // Normal
				Vec3( currentTan.x, currentTan.y, currentTan.z ), // Tangent
				Vec3( currentBiTan.x, currentBiTan.y, currentBiTan.z ), // Bitangent
				Vec4( currentUV.x, currentUV.y, subMeshCounter, 0), // UV Padding and submesh index
				Point4 ( 
					vertexWeights[j].bones[0].bone,
					vertexWeights[j].bones[1].bone,
					vertexWeights[j].bones[2].bone,
					vertexWeights[j].bones[3].bone
				), // Bone indicies 
				Vec4 ( 
					vertexWeights[j].bones[0].weight,
					vertexWeights[j].bones[1].weight,
					vertexWeights[j].bones[2].weight,
					vertexWeights[j].bones[3].weight
				)); // Weights 

			// Add the vertex to the output list
			outMesh->verticies.Add( newVertex );
		}
		LOG_TRACE( "		Done - SubMesh Vertex count: %d, Total: %d", newMesh->mNumVertices, outMesh->verticies.Size() );
		LOG_TRACE( "		Processing submesh indicies ... ");

		for (unsigned int j = 0; j < newMesh->mNumFaces; j++)
		{
			for (unsigned int k = 0; k < newMesh->mFaces[j].mNumIndices; k++)
			{
				outMesh->indicies.Add(vertexCounter + newMesh->mFaces[j].mIndices[k] );
				numIndexCounter++;
			}
		}
		LOG_TRACE( "		Done - Submesh index count: %d - Total: %d\n", numIndexCounter - startIndex, outMesh->indicies.Size());
		outMesh->indexCounts.Add(numIndexCounter - startIndex);
		vertexCounter = outMesh->verticies.Size();
		subMeshCounter++;
	}
	LOG_TRACE( "Done\n" );
	LOG_TRACE( "Resolving armature parent indicies (Possibly redundant) ... " );
	// Maybe redundant?
	outMesh->armature.ResolveBoneParentIndicies();

	// Printout for checking
	//LOG_TRACE( "Import process 8: Debug print of verticies: " );
	//for (int i = 0; i < outMesh->verticies.Size(); i++)
	//{
	//	LOG_TRACE( "V[%d] Bones: %s, %s, %s, %s - Weights: %f, %f, %f, %f",
	//		i,
	//		boneNames[outMesh->verticies[i].bones[0]].c_str(),
	//		boneNames[outMesh->verticies[i].bones[1]].c_str(),
	//		boneNames[outMesh->verticies[i].bones[2]].c_str(),
	//		boneNames[outMesh->verticies[i].bones[3]].c_str(),
	//		outMesh->verticies[i].weights[0],
	//		outMesh->verticies[i].weights[1],
	//		outMesh->verticies[i].weights[2],
	//		outMesh->verticies[i].weights[3] );
	//	//LOG_TRACE( "Group[%d] - %s - Num vert-weights: %d", i, outMesh->vertexGroups[i].name.c_str(), outMesh->vertexGroups[i].verticies.Size());
	//}

	LOG_TRACE( "Done\n" );
	LOG_TRACE( "Rigged Import Completed! Num verts: %d\n", outMesh->verticies.Size());
	return true;
}

bool FBXImporter::ImportFBXAnimations(std::string filePath, AnimationResource* const outAnimations)
{
	LOG_TRACE( "\nStarting Animation FBX Import for file: %s", filePath.c_str() );
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( filePath,
		/*aiProcess_CalcTangentSpace | aiProcess_Triangulate |*/ aiProcess_PopulateArmatureData );
	if (scene == nullptr) {
		LOG_ERROR( "THERE WAS AN FBX IMPORT ERROR:" );
		LOG_TRACE( importer.GetErrorString() );
		return false;
	}
	std::string answerString = (scene->HasAnimations()) ? "True" : "False";
	LOG_TRACE( "ASSIMP: opened file: %s, has meshes? - %s", filePath.c_str(), answerString.c_str() );
	LOG_TRACE( "Number of animations: %d\n", scene->mNumAnimations );

	if (!scene->HasAnimations())
	{
		std::cout << "proposed .fbx does not contain any animations." << std::endl;
		return false;
	}

	LOG_TRACE( "==ANIMATION IMPORT PROCESS START==\n" );

	LOG_TRACE( "Processing animations ... \n" );
	//if (VERBOSEINFO) std::cout << "Found: " << scene->mNumAnimations << " animations:" << std::endl;
	for (size_t i = 0; i < scene->mNumAnimations; i++)
	{
		Animation newAnimation;
		aiAnimation* animation = scene->mAnimations[i];

		newAnimation.name = std::string(animation->mName.C_Str());
		newAnimation.duration = animation->mDuration;

		LOG_TRACE( "	Processing: %s\n", animation->mName.C_Str());
		LOG_TRACE( "		Processing channels ...");
		for (size_t j = 0; j < animation->mNumChannels; j++)
		{
			AnimationChannel newChannel;
			aiNodeAnim* channel = animation->mChannels[j];
			newChannel.channelName = channel->mNodeName.C_Str();
			//LOG_TRACE( "		Channel: %s", channel->mNodeName.C_Str());
			//LOG_TRACE( "			Processing position keys ... " );
			for (size_t k = 0; k < channel->mNumPositionKeys; k++)
			{
				Vec3KeyFrame newKey;
				aiVectorKey key = channel->mPositionKeys[k];
				newKey.time = key.mTime;
				newKey.value = { key.mValue.x, key.mValue.y, key.mValue.z };
				//if (VERBOSEINFO) std::cout << "Pos Key: " << newKey.value.ToString() << " Time: " << newKey.time << std::endl;
				newChannel.positionKeyFrames.Add( newKey );
			}
			//LOG_TRACE( "			Done! - Keys: %d", channel->mNumPositionKeys );
			//LOG_TRACE( "			Processing rotation keys ... " );
			for (size_t k = 0; k < channel->mNumRotationKeys; k++)
			{
				QuatKeyFrame newKey;
				aiQuatKey key = channel->mRotationKeys[k];
				newKey.time = key.mTime;
				newKey.value = Quaternion( { key.mValue.x, key.mValue.y, key.mValue.z }, key.mValue.w );
				//if (VERBOSEINFO) std::cout << "Rot Key: " << newKey.value.ToString() << " Time: " << newKey.time << std::endl;
				newChannel.rotationKeyFrames.Add( newKey );
			}
			//LOG_TRACE( "			Done! - Keys: %d", channel->mNumRotationKeys );
			//LOG_TRACE( "			Processing scaling keys ... " );
			for (size_t k = 0; k < channel->mNumScalingKeys; k++)
			{
				Vec3KeyFrame newKey;
				aiVectorKey key = channel->mScalingKeys[k];
				newKey.time = key.mTime;
				newKey.value = { key.mValue.x, key.mValue.y, key.mValue.z };
				//if (VERBOSEINFO) std::cout << "Scale Key: " << newKey.value.ToString() << " Time: " << newKey.time << std::endl;
				newChannel.scaleKeyFrames.Add( newKey );
			}
			//LOG_TRACE( "			Done! - Keys: %d", channel->mNumScalingKeys );
			//LOG_TRACE( "		Done! - PKeys: %d - RKeys: %d - SKeys: %d", channel->mNumPositionKeys, channel->mNumRotationKeys, channel->mNumScalingKeys );
			newAnimation.channels.Add( newChannel );
		}
		LOG_TRACE( "		Done! - Channels: %d - Duration: %f\n", animation->mNumChannels, animation->mDuration);
		outAnimations->animations.Add( newAnimation );
	}
	LOG_TRACE( "	Done! \n", scene->mNumAnimations );
	LOG_TRACE( "Animations Import Completed! Animations: %d\n", scene->mNumAnimations );
	return true;
}
