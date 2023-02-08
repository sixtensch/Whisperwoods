#pragma once
#include <DirectXCollision.h>
#include "Core.h"

#include <vector>
#include <set>


namespace dx = DirectX;

#define MAX_DEPTH 8
#define LEAF_MAX_ELEMENTS 10


/// TODO:
//		Add parent node data functionality
//		Add reconstruction feature
//		implement indexed culling
//

template <typename T>
class QuadTree
{
private: // Misc
	struct NodeData
	{
		T* element;
		dx::BoundingBox boundedVolume;
		int id;
	};
	struct Node
	{
		std::vector< NodeData > data;

		dx::BoundingBox nodeBox;
		Node* children[4];
	};
public: // Methods
	QuadTree();
	~QuadTree();
	void Init( float maxHeight, float minHeight, float top, float left);
	void Reconstruct( float top, float left );


	void AddElement( T* elementAddress, const dx::BoundingBox& boundingBox );
	void AddIndexed( T* elementAddress, const dx::BoundingBox& boundingBox, int index );

	std::vector<const T>& CullTree( const dx::BoundingFrustum& frustum ) const;
	void CullIndexed( const dx::BoundingFrustum& frustum, cs::List<T*>& out_culledObjects ) const;
private:
	void AddToNode( T* elementAddress, const dx::BoundingBox& boundingBox, Node* nodeToProcess, int depth, int index = -1 );
	void CheckNode( const dx::BoundingFrustum& frustum, Node* node, std::set<const T*>& out_validElements ) const;
	void CheckIndexed( const dx::BoundingFrustum& frustum, Node* node, int index, std::vector<const T*>& out_validElements );
	

	void FreeNode( Node* node );

	bool IsLeaf( Node* nodeToProcess ) const;
	bool IsFull(Node * nodeToProcess, int& out_index) const;
	bool IsParentData(Node* nodeToProcess, int& out_childNr) const;
	void SplitNode( Node* nodeToProcess );

public: // Variables
private:
	Node* m_root;
	float m_maxHeight;
	float m_minHeight;
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
inline void QuadTree<T>::Init( float maxHeight, float minHeight, float top, float left)
{
	m_maxHeight = maxHeight;
	m_minHeight = minHeight;

	// Creates the root bounding box
	dx::XMVECTOR topleft = { left, m_minHeight, top };
	dx::XMVECTOR bottomRigh = { -left, m_maxHeight, -top };

	dx::BoundingBox rootBox;
	dx::BoundingBox::CreateFromPoints( rootBox, topleft, bottomRigh );


	m_root = (Node*)malloc( sizeof( Node ) );
	if ( !m_root )
	{
		EXC("Failed mallocing the root for quadtree.");
	}

	for ( int i = 0; i < 4; i++ )
	{
		m_root->children[i] = nullptr;
	}
	for ( int i = 0; i < LEAF_MAX_ELEMENTS; ++i )
	{
		m_root->data[i].element = nullptr;
		m_root->data[i].boundedVolume = {};
	}
	m_root->nodeBox = rootBox;
}


/// <summary>
/// Calls Free() on all the nodes in the tree and destroys the tree.
/// After the tree is destroyed it is recreated with the new height and width parameters.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="Height"></param>
/// <param name="Width"></param>
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


	m_root = (Node*)malloc(sizeof(Node));
	if ( !m_root )
	{
		EXC("Failed mallocing the root for quadtree in Reconstruction function call.");
	}

	for ( int i = 0; i < 4; i++ )
	{
		m_root->children[i] = nullptr;
	}
	for ( int i = 0; i < LEAF_MAX_ELEMENTS; ++i )
	{
		m_root->data[i].element = nullptr;
		m_root->data[i].boundedVolume = {};
	}
	m_root->nodeBox = rootBox;
}


template<typename T>
inline void QuadTree<T>::FreeNode( Node* node )
{
	if ( node == nullptr )
	{
		return;
	}
	if ( IsLeaf( node ) )
	{
		free( node );
		node = nullptr;
		return;
	}
	// This is an initialized parent
	for ( int i = 0; i < 4; ++i )
	{
		FreeNode( node->children[i] );
	}
	free( node );
}


/// <summary>
/// Add a non indexed item to the tree, if this is used the non indexed CullTree function should be used
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="ElementAddress"></param>
/// <param name="BoundingBox"></param>
template<typename T>
inline void QuadTree<T>::AddElement( T* elementAddress, const dx::BoundingBox& boundingBox )
{
	AddToNode( elementAddress, boundingBox, m_root, 0, -1 );
}
template<typename T>
inline void QuadTree<T>::AddIndexed( T* elementAddress, const dx::BoundingBox& boundingBox, int index )
{
	AddToNode( elementAddress, boundingBox,m_root, 0, index );
}

template<typename T>
inline void QuadTree<T>::AddToNode( T* elementAddress, const dx::BoundingBox& boundingBox, Node* nodeToProcess, int depth, int index )
{
	bool collision = nodeToProcess->nodeBox.Intersects( boundingBox );
	if ( !collision )
		return;

	int newDepth = ++depth;
	if ( IsLeaf( nodeToProcess ) )
	{
		int nodeElements = 0;
		if ( (depth >= MAX_DEPTH) )
		{
			//extend node
			nodeToProcess->data.push_back( { elementAddress, boundingBox, index } );
			return;
		}
		else if ( !IsFull( nodeToProcess, nodeElements ) )
		{
			// Add element to this node
			nodeToProcess->data[nodeElements].element = elementAddress;
			nodeToProcess->data[nodeElements].boundedVolume = boundingBox;
			nodeToProcess->data[nodeElements].id = index;
			return;
		}
		else
		{
			// Make this node a parent
			SplitNode( nodeToProcess );

			// Add the current nodes element to a child node
			for ( int elementIndex = 0; elementIndex < LEAF_MAX_ELEMENTS; elementIndex++ )
			{
				for ( int childIndex = 0; childIndex < 4; ++childIndex )
				{
					AddToNode( nodeToProcess->data[elementIndex].element, nodeToProcess->data[elementIndex].boundedVolume, nodeToProcess->children[childIndex], newDepth );
				}
			}
			// Clear node
			for ( int i = 0; i < LEAF_MAX_ELEMENTS; ++i )
			{
				nodeToProcess->data[i].element = nullptr;
				nodeToProcess->data[i].boundedVolume = {};
				nodeToProcess->data[i].id = index;
			}
		}
	}

	// Node is either parent or was made into one, pass along to next nodes
	AddToNode( elementAddress, boundingBox, nodeToProcess->children[0], newDepth, index );
	AddToNode( elementAddress, boundingBox, nodeToProcess->children[1], newDepth, index );
	AddToNode( elementAddress, boundingBox, nodeToProcess->children[2], newDepth, index );
	AddToNode( elementAddress, boundingBox, nodeToProcess->children[3], newDepth, index );
	return;
}


/// <summary>
/// Searches the spatialtree with the use of the frustum.
/// No duplicate objects are returned in the vector.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="Frustum"></param>
/// <returns></returns>
template<typename T>
inline std::vector<const T>& QuadTree<T>::CullTree( const dx::BoundingFrustum& frustum ) const
{
	std::vector<const T> toReturn;
	std::set<const T*> validElements;

	// Treverse the tree from root
	CheckNode( frustum, m_root, validElements );

	for ( auto& element : validElements )
	{
		toReturn.push_back( *element );
	}
	return toReturn;
}
template<typename T>
inline void QuadTree<T>::CullIndexed(const dx::BoundingFrustum& frustum, cs::List<T*>& out_culledObjects) const
{

}

template<typename T>
inline void QuadTree<T>::CheckNode( const dx::BoundingFrustum& frustum, Node* node, std::set<const T*>& out_validElements ) const
{
	//Check if node bounding volume is in the frustum
	bool collision = frustum.Contains( node->nodeBox );
	if ( !collision )
	{
		return;
	}

	if ( IsLeaf( node ) )
	{
		for ( int i = 0; i < LEAF_MAX_ELEMENTS; ++i )
		{
			if ( node->data[i].element == nullptr )
			{
				break;
			}

			// Check if element bounding volume is in the frustum
			collision = frustum.Contains( node->data[i].boundedVolume );
			if ( !collision )
			{
				continue;
			}

			out_validElements.insert( node->data[i].element );
		}
	}
	else
	{
		for ( int i = 0; i < 4; ++i )
		{
			CheckNode( frustum, node->children[i], out_validElements );
		}
	}

}

template<typename T>
inline void QuadTree<T>::CheckIndexed(const dx::BoundingFrustum& frustum, Node* node, int index, std::vector<const T*>& out_validElements)
{

}



// Helper functions

template<typename T>
inline bool QuadTree<T>::IsLeaf( Node* nodeToProcess ) const
{
	return nodeToProcess->children[0] == nullptr;
}

template<typename T>
inline bool QuadTree<T>::IsFull( Node* nodeToProcess, int& out_index ) const
{
	out_index = 0;
	for ( int i = 0; i < LEAF_MAX_ELEMENTS; ++i )
	{
		if ( nodeToProcess->data[i].element != nullptr )
		{
			++out_index;
		}
		else
		{
			break;
		}
	}
	return out_index == (LEAF_MAX_ELEMENTS);
}

template<typename T>
inline bool QuadTree<T>::IsParentData(Node* nodeToProcess, int& out_childNr) const
{
	for ( int i = 0; i < 4; ++i )
	{
		// check if there is a collision between element and child bounding volume
	}

	return false;
}

template<typename T>
inline void QuadTree<T>::SplitNode( Node* nodeToProcess )
{
	dx::BoundingBox tempBox;
	dx::XMFLOAT3 CenterPoint = nodeToProcess->nodeBox.Center;
	dx::XMFLOAT3 radiusExtend = nodeToProcess->nodeBox.Extents;


	// boundingbox topleft
	nodeToProcess->children[0] = (Node*)malloc( sizeof( Node ) );
	if ( nodeToProcess->children[0] )
	{
		tempBox.CreateFromPoints( nodeToProcess->children[0]->nodeBox,
								  { CenterPoint.x - radiusExtend.x, m_minHeight, CenterPoint.z + radiusExtend.z },
								  { CenterPoint.x,                  m_maxHeight, CenterPoint.z } );
	}
	// boundingbox topright
	nodeToProcess->children[1] = (Node*)malloc( sizeof( Node ) );
	if ( nodeToProcess->children[1] )
	{
		tempBox.CreateFromPoints( nodeToProcess->children[1]->nodeBox,
								  { CenterPoint.x,                  m_minHeight, CenterPoint.z + radiusExtend.z },
								  { CenterPoint.x + radiusExtend.x, m_maxHeight, CenterPoint.z } );
	}
	// boundingbox bottomleft
	nodeToProcess->children[2] = (Node*)malloc( sizeof( Node ) );
	if ( nodeToProcess->children[2] )
	{
		tempBox.CreateFromPoints( nodeToProcess->children[2]->nodeBox,
								  { CenterPoint.x - radiusExtend.x, m_minHeight, CenterPoint.z },
								  { CenterPoint.x,                  m_maxHeight, CenterPoint.z - radiusExtend.z } );
	}
	// boundingbox bottomright
	nodeToProcess->children[3] = (Node*)malloc( sizeof( Node ) );
	if ( nodeToProcess->children[3] )
	{
		tempBox.CreateFromPoints( nodeToProcess->children[3]->nodeBox,
								  { CenterPoint.x,                  m_minHeight, CenterPoint.z },
								  { CenterPoint.x + radiusExtend.x, m_maxHeight, CenterPoint.z - radiusExtend.z } );
	}



	// Created children for the node, sanitise the new nodes for future use
	for ( int i = 0; i < 4; ++i )
	{
		nodeToProcess->children[i]->children[0] = nullptr;
		nodeToProcess->children[i]->children[1] = nullptr;
		nodeToProcess->children[i]->children[2] = nullptr;
		nodeToProcess->children[i]->children[3] = nullptr;
		
		nodeToProcess->children[i]->data;
	}
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < LEAF_MAX_ELEMENTS; ++j )
		{
			nodeToProcess->children[i]->data[j].element = nullptr;
			nodeToProcess->children[i]->data[j].boundedVolume = {};
		}
	}
}