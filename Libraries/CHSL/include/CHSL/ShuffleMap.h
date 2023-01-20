#pragma once

/* CHSL

	|	ShuffleMap<Key, Value>
	|
	|	- Red-Black Tree based associative container.
	|	- Optimized and built for items to change their key efficiently and quickly.

*/









#include <map>

#include "CHSLTypedef.h"
#include "RBTree.h"

namespace cs
{

	template <typename T_key, typename T_val>
	struct ShuffleMapStruct
	{
		T_key key;
		T_val val;
		uint identifier;

		bool operator==(const ShuffleMapStruct& lVal) const { return identifier == lVal.identifier; }
		bool operator!=(const ShuffleMapStruct& lVal) const { return key != lVal.key || identifier != lVal.identifier; }
		bool operator>(const ShuffleMapStruct& lVal) const { return key > lVal.key || (key == lVal.key && identifier > lVal.identifier); }
		bool operator<(const ShuffleMapStruct& lVal) const { return key < lVal.key || (key == lVal.key && identifier < lVal.identifier); }
	};



	template<typename T_key, typename T_val>
	class ShuffleMap
	{
	public:
		ShuffleMap();
		~ShuffleMap();

		ShuffleMap(ShuffleMap&& rVal);
		ShuffleMap& operator=(ShuffleMap&& rVal);

		// Copy deleted
		ShuffleMap(const ShuffleMap& lVal) = delete;
		ShuffleMap& operator=(const ShuffleMap& lVal) = delete;

		uint Add(T_key key, T_val val);

		void Clear();
		void Delete(T_key key, uint id);
		void Shuffle(T_key key, uint id, T_key newKey);

		std::vector<T_val> InOrder();

	private:
		void BranchInOrder(typename RBTree<ShuffleMapStruct<T_key, T_val>, false>::Node* node, std::vector<T_val>& order);

	private:
		RBTree<ShuffleMapStruct<T_key, T_val>, false> m_tree;
		uint m_idCounter;


	};



	// ---



	template<typename T_key, typename T_val>
	ShuffleMap<T_key, T_val>::ShuffleMap()
		:
		m_tree(),
		m_idCounter(1)
	{
	}

	template<typename T_key, typename T_val>
	ShuffleMap<T_key, T_val>::~ShuffleMap()
	{
	}

	template<typename T_key, typename T_val>
	ShuffleMap<T_key, T_val>::ShuffleMap(ShuffleMap&& rVal)
	{
		*this = std::move(rVal);
	}

	template<typename T_key, typename T_val>
	ShuffleMap<T_key, T_val>& ShuffleMap<T_key, T_val>::operator=(ShuffleMap&& rVal)
	{
		m_tree = rVal.m_tree;
	}

	template<typename T_key, typename T_val>
	uint ShuffleMap<T_key, T_val>::Add(T_key key, T_val val)
	{
		ShuffleMapStruct<T_key, T_val> newItem;
		newItem.key = key;
		newItem.val = val;
		newItem.identifier = m_idCounter;

		m_idCounter++;

		m_tree.Add(newItem);

		return newItem.identifier;
	}

	template<typename T_key, typename T_val>
	void ShuffleMap<T_key, T_val>::Delete(T_key key, uint id)
	{
		char bytes[sizeof(ShuffleMapStruct<T_key, T_val>)];
		ShuffleMapStruct<T_key, T_val>& byteRef = *(ShuffleMapStruct<T_key, T_val>*) & bytes;

		byteRef.key = key;
		byteRef.identifier = id;

		m_tree.Delete(byteRef);
	}

	template<typename T_key, typename T_val>
	void ShuffleMap<T_key, T_val>::Clear()
	{
		m_tree.Clear();
		m_idCounter = 1;
	}

	template<typename T_key, typename T_val>
	void cs::ShuffleMap<T_key, T_val>::Shuffle(T_key key, uint id, T_key newKey)
	{
		typedef typename RBTree<ShuffleMapStruct<T_key, T_val>, false>::Node* nptr;

		nptr current = m_tree.root;

		while (!(current == m_tree.nilNode) && current->element.identifier != id)
		{
			if (key > current->element.key)
			{
				current = current->rightChild;
			}
			else
			{
				current = current->leftChild;
			}
		}

		if (current == m_tree.nilNode)
		{
			// Couldn't find node
			return;
		}

		current->element.key = newKey;

		auto swap = [](nptr source, nptr target)
		{
			T_key tKey = target->element.key;
			T_val tVal = target->element.val;
			uint tId = target->element.identifier;

			target->element.key = source->element.key;
			target->element.val = source->element.val;
			target->element.identifier = source->element.identifier;

			source->element.key = tKey;
			source->element.val = tVal;
			source->element.identifier = tId;
		};

		bool switching = true;
		nptr target = nullptr;

		do
		{
			target = nullptr;

			if (current->parent->rightChild == current)
			{
				// Is right child

				if (current->rightChild != m_tree.nilNode && current->element > current->rightChild->element)
				{
					target = current->rightChild;
				}
				else if (current->parent != m_tree.nilNode && current->element < current->parent->element)
				{
					target = current->parent;
				}
				else if (current->leftChild != m_tree.nilNode && current->element < current->leftChild->element)
				{
					target = current->leftChild;
				}
			}
			else
			{
				// Is left child

				if (current->leftChild != m_tree.nilNode && current->element < current->leftChild->element)
				{
					target = current->leftChild;
				}
				else if (current->parent != m_tree.nilNode && current->element > current->parent->element)
				{
					target = current->parent;
				}
				else if (current->rightChild != m_tree.nilNode && current->element > current->rightChild->element)
				{
					target = current->rightChild;
				}
			}

			if (target == nullptr)
			{
				switching = false;
			}
			else
			{
				swap(current, target);
				current = target;
			}

		} 
		while (switching);
	}

	template<typename T_key, typename T_val>
	std::vector<T_val> cs::ShuffleMap<T_key, T_val>::InOrder()
	{
		std::vector<T_val> order;

		if (m_tree.root != m_tree.nilNode)
		{
			BranchInOrder(m_tree.root, order); // Recursively fill the vector
		}

		return order;
	}



	template<typename T_key, typename T_val>
	void ShuffleMap<T_key, T_val>::BranchInOrder(typename RBTree<ShuffleMapStruct<T_key, T_val>, false>::Node* node, std::vector<T_val>& order)
	{
		if (node->leftChild != m_tree.nilNode)
		{
			BranchInOrder(node->leftChild, order);
		}

		order.push_back(node->element.val);

		if (node->rightChild != m_tree.nilNode)
		{
			BranchInOrder(node->rightChild, order);
		}
	}

}