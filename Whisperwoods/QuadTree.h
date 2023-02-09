#pragma once
#include <DirectXCollision.h>
#include "Core.h"
#include <vector>

// TODO:
//		* Fix errors
//		* GraphViz print function for debugging
//		

template <typename T>
class QuadTree
{
private: // Misc
	struct NodeData
	{
		shared_ptr<const T*> element;
		dx::BoundingBox boundedVolume;
		int id;
	};
	struct Node
	{
		std::vector< NodeData > data;

		dx::BoundingBox rootPartition;
		shared_ptr<Node> child[4];
	};
public: // Methods
	/// <summary>The tree works like a binary tree with a constant of 4 children per node. 
	/// <para>The parent and-leaf nodes will have items added to them, therefore disallowing duplication.</para>
	/// <para>There are two core functionalities:</para>
	/// 
	///	<para>1. CullTree() - Culls the tree and returns a vector of all the found elements</para>
	///	<para>2. CullTreeIndexed() - Culls the tree and returns the values by their assigned index in the out_list passed as parameter. (no item is added if index is non positive)</para>
	/// 
	/// <para>!Beware that if the positions of the object pointed to by the tree are changed, UB is due</para>
	/// </summary>
	QuadTree();
	~QuadTree();

	void Init(float maxHeight, float minHeight, float top, float left);
	void Reconstruct(float top, float left);
#if WW_DEBUG
	const string PrintTree() const;
#endif

public: // Core functionality
	void AddElement(shared_ptr<const T*> elementAddress, const dx::BoundingBox& boundingBox);
	void AddElementIndexed(shared_ptr<const T*> elementAddress, const dx::BoundingBox& boundingBox, int index);

	std::vector<const T>& CullTree(const dx::BoundingFrustum& frustum) const;
	void CullTreeIndexed(const dx::BoundingFrustum& frustum, cs::List<cs::List<const T*>>& out_culledObjects) const;

private: // Recursive callers
	void AddToNode(shared_ptr<const T*> elementAddress, const dx::BoundingBox& boundingBox, const shared_ptr<Node>& currentNode, int depth, int index = -1);

	void CullNode(const dx::BoundingFrustum& frustum, const shared_ptr<Node>& currentNode, std::vector<const T*>& out_validElements) const;
	void CullNodeIndexed(const dx::BoundingFrustum& frustum, const shared_ptr<Node>& currentNode, cs::List<cs::List<const T*>>& out_indexedList);
	
	void FreeNode(shared_ptr<Node>& currentNode);

#if WW_DEBUG
	string PrintNode(const shared_ptr<Node>& currentNode) const;
#endif

private: // Helpers
	bool IsLeaf(const shared_ptr<Node>& currentNode) const;
	bool IsFull(const shared_ptr<Node>& currentNode, int& out_index) const;
	bool PartitionIntoChild(const dx::BoundingBox& elementVolume, const shared_ptr<Node>& currentNode, int& out_childNr) const;
	void SplitNode(const shared_ptr<Node>& currentNode);

private:
	shared_ptr<Node> m_root;
	//bool m_lock;
	float m_maxHeight;
	float m_minHeight;

private: // Settings
	static const UINT s_maxLeafElements = 10;
	static const UINT s_maxDepth = 8;
};

template<typename T>
inline QuadTree<T>::QuadTree() : m_minHeight( -20.0f ), m_maxHeight( 20.0f )
{
	m_root = nullptr;
}
template<typename T>
inline QuadTree<T>::~QuadTree()
{
	//Recursively go through the tree to free every node
	FreeNode( m_root );
}



// ################################### Definitions ###################################

template<typename T>
inline void QuadTree<T>::Init(float maxHeight, float minHeight, float top, float left)
{
	m_maxHeight = maxHeight;
	m_minHeight = minHeight;

	// Creates the root bounding box
	dx::XMVECTOR topleft = { left, m_minHeight, top };
	dx::XMVECTOR bottomRigh = { -left, m_maxHeight, -top };

	dx::BoundingBox rootBox;
	dx::BoundingBox::CreateFromPoints( rootBox, topleft, bottomRigh );


	m_root = make_shared<Node>();
	if ( !m_root )
	{
		EXC("Failed creating the root for quadtree.");
	}

	for ( int i = 0; i < 4; i++ )
	{
		m_root->child[i] = nullptr;
	}

	m_root->data.reserve(s_maxLeafElements);
	m_root->rootPartition = rootBox;
}
template<typename T>
inline void QuadTree<T>::Reconstruct(float top, float left)
{
	// Clear the entire tree
	FreeNode(m_root);

	// Create a new tree
	dx::XMVECTOR topleft = { left, m_minHeight, top };
	dx::XMVECTOR bottomRigh = { -left, m_maxHeight, -top };

	dx::BoundingBox rootBox;
	dx::BoundingBox::CreateFromPoints(rootBox, topleft, bottomRigh);


	m_root = make_shared<Node>();
	if ( !m_root )
	{
		EXC("Failed mallocing the root for quadtree in Reconstruction function call.");
	}

	for ( int i = 0; i < 4; i++ )
	{
		m_root->child[i] = nullptr;
	}
	m_root->data.clear();
	m_root->rootPartition = rootBox;
}

/// <summary>
/// Searches the spatialtree with the use of the frustum.
/// This does not use the index functionality
/// </summary>
template<typename T>
inline std::vector<const T>& QuadTree<T>::CullTree(const dx::BoundingFrustum& frustum) const
{
	std::vector<const T> toReturn;
	// Treverse the tree from root
	CullNode(frustum, m_root, toReturn);

	return toReturn;
}
/// <summary>
/// Searches the spatialtree with the use of the frustum.
/// This uses the index functionality,
/// <para>It is assumed that the indexes are valid and are not out of bounds</para>
/// </summary>
template<typename T>
inline void QuadTree<T>::CullTreeIndexed(const dx::BoundingFrustum& frustum, cs::List<cs::List<const T*>>& out_culledObjects) const
{
	CullNodeIndexed(frustum, m_root, out_culledObjects);
}

template<typename T>
inline void QuadTree<T>::AddElement(shared_ptr<const T*> elementAddress, const dx::BoundingBox& boundingBox)
{
	AddToNode(elementAddress, boundingBox, m_root, 0, -1);
}
template<typename T>
inline void QuadTree<T>::AddElementIndexed(shared_ptr<const T*> elementAddress, const dx::BoundingBox& boundingBox, int index)
{
	AddToNode(elementAddress, boundingBox,m_root, 0, index);
}




// ########################   RECURSIVE CALLS   ########################


template<typename T>
inline void QuadTree<T>::AddToNode(shared_ptr<const T*> elementAddress, const dx::BoundingBox& boundingBox, const shared_ptr<Node>& currentNode, int depth, int index)
{
	bool collision = currentNode->rootPartition.Contains(boundingBox);
	if ( !collision )
		return;

	// Max depth is reached
	if ((depth >= s_maxDepth))
	{
		currentNode->data.push_back({ elementAddress, boundingBox, index });
		return;
	}

	int newDepth = ++depth;
	if ( IsLeaf(currentNode) )
	{
		int nodeElements = 0;
		if ( !IsFull(currentNode, nodeElements ) )
		{
			currentNode->data.push_back( { elementAddress, boundingBox, index } );
			return;
		}
		else
		{
			// Make this node a parent
			SplitNode( currentNode );

			int partitionIndex = -1;
			for (int i = 0; i < currentNode->data.size();) 
			{
				if (PartitionIntoChild(currentNode->data[i].boundedVolume, currentNode, partitionIndex))
				{
					// Only one child fits this node, therefore send it into that node
					AddToNode(currentNode->data[i].element, currentNode->data[i].boundedVolume, currentNode->child[partitionIndex], newDepth, currentNode->data[i].id);
					
					// Clear node
					currentNode->data.erase(currentNode->data.begin() + i);
				}
				else
				{
					++i;
				}
			}

		}
	}

	// Node is either parent or was made into one, continue with the adding of a new element
	int partitionIndex = -1;
	if (PartitionIntoChild(boundingBox, currentNode, partitionIndex))
	{
		// Only one child fits this node, therefore send it into that node
		AddToNode(elementAddress, boundingBox, currentNode->child[partitionIndex], newDepth, index);
	}
	else
	{
		// It does not fit into any child partition, put it in this parent node
		currentNode->data.push_back({ elementAddress, boundingBox, index });
	}
	return;
}

template<typename T>
inline void QuadTree<T>::CullNode(const dx::BoundingFrustum& frustum, const shared_ptr<Node>& node, std::vector<const T*>& out_validElements) const
{
	//Check if node bounding volume is in the frustum
	bool collision = frustum.Contains(node->rootPartition);
	if (!collision)
		return;

	for (auto& nodeData : node->data)
	{
		collision = frustum.Contains(nodeData.boundedVolume);
		if (collision)
			out_validElements.push_back(*nodeData.element);
	}
	if (!IsLeaf(node))
	{
		for (int i = 0; i < 4; ++i)
		{
			CullNode(frustum, node->child[i], out_validElements);
		}
	}
}

template<typename T>
inline void QuadTree<T>::CullNodeIndexed(const dx::BoundingFrustum& frustum, const shared_ptr<Node>& node, cs::List<cs::List<const T*>>& out_indexedList)
{
	bool collision = frustum.Contains(node->rootPartition);
	if (!collision)
		return;

	for (auto& nodeData : node->data)
	{
		collision = frustum.Contains(nodeData.boundedVolume);
		if (collision)
			out_indexedList[nodeData.id].Add(*nodeData.element);
	}
	if (!IsLeaf(node))
	{
		for (int i = 0; i < 4; ++i)
		{
			CullNodeIndexed(frustum, node->child[i], out_indexedList);
		}
	}
}


template<typename T>
inline void QuadTree<T>::FreeNode(shared_ptr<Node>& node)
{
	if (node)
	{
		return;
	}
	if (!IsLeaf(node))
	{
		// This is an initialized parent
		for (int i = 0; i < 4; ++i)
		{
			FreeNode(node->child[i]);
		}
	}
	node.reset();
}

// ######################## RECURSIVE CALLS END ########################

// Helper functions

template<typename T>
inline bool QuadTree<T>::IsLeaf(const shared_ptr<Node>& currentNode) const
{
	return currentNode->child[0] == nullptr;
}

template<typename T>
inline bool QuadTree<T>::IsFull(const shared_ptr<Node>& currentNode, int& out_index) const
{
	out_index = 0;
	for ( int i = 0; i < s_maxLeafElements; ++i )
	{
		if (currentNode->data[i].element == nullptr)
		{
			out_index = i;
			break;
		}
	}
	return out_index == (s_maxLeafElements);
}

template<typename T>
inline bool QuadTree<T>::PartitionIntoChild(const dx::BoundingBox& elementVolume, const shared_ptr<Node>& currentNode, int& out_childNr) const
{
	out_childNr = -1;
	bool collision = {};
	for( int i = 0; i < 4; ++i )
	{
		// check if there is a collision between element and child bounding volume
		collision = currentNode->child[i]->rootPartition.Contains(elementVolume);
		if (collision && out_childNr < 0)
			out_childNr = i;
		else
			return false;
	}
	return true;
}

template<typename T>
inline void QuadTree<T>::SplitNode(const shared_ptr<Node>& currentNode)
{
	dx::BoundingBox tempBox;
	dx::XMFLOAT3 CenterPoint = currentNode->rootPartition.Center;
	dx::XMFLOAT3 radiusExtend = currentNode->rootPartition.Extents;


	// boundingbox topleft
	currentNode->child[0] = make_shared<Node>();
	if ( currentNode->child[0] )
	{
		tempBox.CreateFromPoints( currentNode->child[0]->rootPartition,
								  { CenterPoint.x - radiusExtend.x, m_minHeight, CenterPoint.z + radiusExtend.z },
								  { CenterPoint.x,                  m_maxHeight, CenterPoint.z } );
	}
	// boundingbox topright
	currentNode->child[1] = make_shared<Node>();
	if ( currentNode->child[1] )
	{
		tempBox.CreateFromPoints( currentNode->child[1]->rootPartition,
								  { CenterPoint.x,                  m_minHeight, CenterPoint.z + radiusExtend.z },
								  { CenterPoint.x + radiusExtend.x, m_maxHeight, CenterPoint.z } );
	}
	// boundingbox bottomleft
	currentNode->child[2] = make_shared<Node>();
	if ( currentNode->child[2] )
	{
		tempBox.CreateFromPoints( currentNode->child[2]->rootPartition,
								  { CenterPoint.x - radiusExtend.x, m_minHeight, CenterPoint.z },
								  { CenterPoint.x,                  m_maxHeight, CenterPoint.z - radiusExtend.z } );
	}
	// boundingbox bottomright
	currentNode->child[3] = make_shared<Node>();
	if ( currentNode->child[3] )
	{
		tempBox.CreateFromPoints( currentNode->child[3]->rootPartition,
								  { CenterPoint.x,                  m_minHeight, CenterPoint.z },
								  { CenterPoint.x + radiusExtend.x, m_maxHeight, CenterPoint.z - radiusExtend.z } );
	}



	// Created children for the node, sanitise the new nodes for future use
	for ( int i = 0; i < 4; ++i )
	{
		currentNode->child[i]->child[0] = nullptr;
		currentNode->child[i]->child[1] = nullptr;
		currentNode->child[i]->child[2] = nullptr;
		currentNode->child[i]->child[3] = nullptr;
		
		for (int j = 0; j < s_maxLeafElements; ++j)
		{
			currentNode->child[i]->data.reserve(s_maxLeafElements);
			currentNode->child[i]->data[j].element = nullptr;
			currentNode->child[i]->data[j].boundedVolume = {};
			currentNode->child[i]->data[j].id = -1;
		}
	}
}