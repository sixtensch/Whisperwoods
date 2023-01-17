#pragma once

#include "Vector.h"
#include "Box.h"

namespace cs
{

	template <typename T_data, typename T_key, typename T_box>
	class OctreeIterator;

	template<typename T_data, typename T_key = cs::Box, typename T_box = cs::Box>
	class Octree
	{
	public:
		Octree(T_box bounds, int octantCapacity = 16);
		Octree(Octree&&);
		Octree(const Octree&) = delete;
		virtual ~Octree();

		// Returns true if added
		bool Add(const T_data& data, const T_key& key);

		// Sets up internal search. Retrieve results by iterating over the Octree.
		// Returns amount of hits.
		int Search(void* data, bool kSelector(const T_key& key, void* data), bool bSelector(const T_box& box, void* data));
		// Sets up internal search. Retrieve results by iterating over the Octree. 
		// Only narrows down to hit regions, does not cull by individual keys.
		// Returns amount of hits.
		int SearchFuzzy(void* data, bool bSelector(const T_box& box, void* data));
		// Sets up internal search. Retrieve results by iterating over the Octree.
		// Returns amount of hits.
		int SearchFast(void* data, bool kSelector(const T_key& key, void* data), bool bContainedSelector(const T_box& box, void* data), bool bTouchedSelector(const T_box& box, void* data));

		// Searches for the corresponding box(s) and removes their members from the tree.
		// Returns amount of removed hits.
		int Remove(void* data, bool kSelector(const T_key& key, void* data), bool bSelector(const T_box& box, void* data));

	public:
		// Premade search functions

		// data -> Rectangle<R>
		static bool SearchEquals(const T_box& box, void* data);




		// Privates

	private:
		struct Data
		{
			T_data data;
			T_key key;
			bool accounted;

			Data* nextGlobal;
			Data* nextSearch;
		};

		struct Node
		{
			Node* children[8] = { nullptr };
			T_box box;

			// -1 -> not a leaf
			int count;
			int capacity;
			Data** data = nullptr;
		};

	private:
		void RecursiveDelete(Node* node);
		bool RecursiveAdd(Node* node, Data* data);
		void RecursiveSearch(
			Node* node, 
			void* data, 
			bool kSelector(const T_key& key, void* data), 
			bool bSelector(const T_box& box, void* data), 
			bool cullByKey);
		void RecursiveSearch(
			Node* node, 
			void* data, 
			bool kSelector(const T_key& key, void* data), 
			bool bContainedSelector(const T_box& box, void* data), 
			bool bTouchedSelector(const T_box& box, void* data));
		void RecursiveAddToSearch(Node* node);


		// Variables

	private:
		Node* m_root;
		Data* m_data;

		Data* m_searchStart;
		int m_searchCount;

		int m_size;
		int m_height;
		int m_octantCapacity;



		// Iterators

	public:
		//template <typename T, typename R>
		class OctreeIterator
		{
		public:
			OctreeIterator(Octree<T_data, T_key, T_box>::Data* current, int count);

			T_data& operator*();
			OctreeIterator& operator++();
			bool operator!=(const OctreeIterator& other);

		private:
			Octree<T_data, T_key, T_box>::Data* m_current;
			int m_count;
		};

		OctreeIterator begin();
		OctreeIterator end();
	};



	// Implementation

	template<typename T_data, typename T_key, typename T_box>
	inline Octree<T_data, T_key, T_box>::Octree(T_box bounds, int octCapacity)
		:
		m_root(new Node{ { nullptr }, bounds, 0, octCapacity, new Data * [octCapacity] }),
		m_data(nullptr),
		m_searchStart(nullptr),
		m_searchCount(0),
		m_size(0),
		m_height(0),
		m_octantCapacity(octCapacity)
	{
	}

	template<typename T_data, typename T_key, typename T_box>
	inline Octree<T_data, T_key, T_box>::Octree(Octree&& rVal)
		:
		m_root(rVal.m_root),
		m_data(rVal.m_data),
		m_searchStart(rVal.m_searchStart),
		m_searchCount(rVal.m_searchCount),
		m_size(rVal.m_size),
		m_height(rVal.m_height),
		m_octantCapacity(rVal.m_octantCapacity)
	{
		rVal.m_data = nullptr;
		rVal.m_root = nullptr;
	}

	template<typename T_data, typename T_key, typename T_box>
	inline Octree<T_data, T_key, T_box>::~Octree()
	{
		RecursiveDelete(m_root);

		Data* current = m_data;
		while (current != nullptr)
		{
			Data* next = current->nextGlobal;
			delete current;
			current = next;
		}
	}

	template<typename T_data, typename T_key, typename T_box>
	bool Octree<T_data, T_key, T_box>::Add(const T_data& data, const T_key& key)
	{
		if (!m_root->box.Touches(key))
		{
			return false;
		}

		Data* dptr = new Data{ data, key, false, nullptr, nullptr };

		bool added = RecursiveAdd(m_root, dptr);

		if (added)
		{
			dptr->nextGlobal = m_data;
			m_data = dptr;
			m_size++;
		}

		return added;
	}

	template<typename T_data, typename T_key, typename T_box>
	bool Octree<T_data, T_key, T_box>::SearchEquals(const T_box& box, void* data)
	{
		return false;
	}

	template<typename T_data, typename T_key, typename T_box>
	int Octree<T_data, T_key, T_box>::Search(
		void* data,
		bool kSelector(const T_key& key, void* data),
		bool bSelector(const T_box& box, void* data))
	{
		m_searchStart = nullptr;
		m_searchCount = 0;

		RecursiveSearch(m_root, data, kSelector, bSelector, true);

		Data* current = m_searchStart;
		for (int i = 0; i < m_searchCount; i++)
		{
			current->accounted = false;
			current = current->nextSearch;
		}

		return m_searchCount;
	}

	template<typename T_data, typename T_key, typename T_box>
	inline int Octree<T_data, T_key, T_box>::SearchFuzzy(void* data, bool bSelector(const T_box& box, void* data))
	{
		m_searchStart = nullptr;
		m_searchCount = 0;

		RecursiveSearch(m_root, data, nullptr, bSelector, false);

		Data* current = m_searchStart;
		for (int i = 0; i < m_searchCount; i++)
		{
			current->accounted = false;
			current = current->nextSearch;
		}

		return m_searchCount;
	}

	template<typename T_data, typename T_key, typename T_box>
	inline int Octree<T_data, T_key, T_box>::SearchFast(void* data, bool kSelector(const T_key& key, void* data), bool bContainedSelector(const T_box& box, void* data), bool bTouchedSelector(const T_box& box, void* data))
	{
		m_searchStart = nullptr;
		m_searchCount = 0;

		RecursiveSearch(m_root, data, kSelector, bContainedSelector, bTouchedSelector);

		Data* current = m_searchStart;
		for (int i = 0; i < m_searchCount; i++)
		{
			current->accounted = false;
			current = current->nextSearch;
		}

		return m_searchCount;
	}

	template<typename T_data, typename T_key, typename T_box>
	void Octree<T_data, T_key, T_box>::RecursiveDelete(Node* node)
	{
		if (node->count == -1)
		{
			for (int i = 0; i < 8; i++)
			{
				RecursiveDelete(node->children[i]);
			}
		}
		else
		{
			delete[] node->data;
		}

		delete node;
	}

	template<typename T_data, typename T_key, typename T_box>
	bool Octree<T_data, T_key, T_box>::RecursiveAdd(Node* node, Data* data)
	{
		if (!node->box.Touches(data->key))
		{
			return false;
		}

		// Add to leaf
		if (node->count != -1 && node->count < node->capacity)
		{
			node->data[node->count++] = data;
			return true;
		}

		// Bifurcate
		if (node->count == node->capacity)
		{
			auto size = node->box.size / 2;
			T_box r[8];
			for (int x = 0; x < 2; x++)
				for (int y = 0; y < 2; y++)
					for (int z = 0; z < 2; z++)
						r[x + 2 * y + 4 * z] = T_box(
							node->box.position.x + size.x * x,
							node->box.position.y + size.y * y,
							node->box.position.z + size.z * z,
							size.x,
							size.y,
							size.z
						);
			
			for (int i = 0; i < 8; i++)
			{
				node->children[i] = new Node{ { nullptr }, r[i], 0, node->capacity, new Data*[node->capacity] };

				int adds = 0;

				for (int j = 0; j < node->count; j++)
				{
					adds += RecursiveAdd(node->children[i], node->data[j]);
				}

				if (node->children[i]->count > node->capacity - 2 && adds >= node->capacity * 2)
				{
					int newCapacity = node->capacity + m_octantCapacity;
					Data** replacement = new Data * [newCapacity];
					for (int j = 0; j < node->capacity; j++)
					{
						replacement[j] = node->children[i]->data[j];
					}
					delete[] node->children[i]->data;
					node->children[i]->data = replacement;
					node->children[i]->capacity = newCapacity;
				}
			}

			node->count = -1;
			delete[] node->data;
		}

		// Add to children
		bool added = false;
		for (int i = 0; i < 8; i++)
		{
			added |= RecursiveAdd(node->children[i], data);
		}

		return added;
	}

	template<typename T_data, typename T_key, typename T_box>
	void Octree<T_data, T_key, T_box>::RecursiveSearch(
		Node* node,
		void* data,
		bool kSelector(const T_key& key, void* data),
		bool bSelector(const T_box& box, void* data),
		bool cullByKey)
	{
		if (node->count == 0 || !bSelector(node->box, data))
		{
			return;
		}

		if (node->count == -1)
		{
			for (int i = 0; i < 8; i++)
			{
				RecursiveSearch(node->children[i], data, kSelector, bSelector, cullByKey);
			}

			return;
		}

		for (int i = 0; i < node->count; i++)
		{
			if (!node->data[i]->accounted && (!cullByKey || kSelector(node->data[i]->key, data)))
			{
				node->data[i]->accounted = true;
				node->data[i]->nextSearch = m_searchStart;
				m_searchStart = node->data[i];
				m_searchCount++;
			}
		}
	}

	template<typename T_data, typename T_key, typename T_box>
	inline void Octree<T_data, T_key, T_box>::RecursiveSearch(
		Node* node, 
		void* data, 
		bool kSelector(const T_key& key, void* data), 
		bool bContainedSelector(const T_box& box, void* data), 
		bool bTouchedSelector(const T_box& box, void* data))
	{
		if (bContainedSelector(node->box, data))
		{
			RecursiveAddToSearch(node);
			return;
		}

		if (node->count == 0 || !bTouchedSelector(node->box, data))
		{
			return;
		}

		if (node->count == -1)
		{
			for (int i = 0; i < 8; i++)
			{
				RecursiveSearch(node->children[i], data, kSelector, bContainedSelector, bTouchedSelector);
			}

			return;
		}

		for (int i = 0; i < node->count; i++)
		{
			if (!node->data[i]->accounted && kSelector(node->data[i]->key, data))
			{
				node->data[i]->accounted = true;
				node->data[i]->nextSearch = m_searchStart;
				m_searchStart = node->data[i];
				m_searchCount++;
			}
		}
	}

	template<typename T_data, typename T_key, typename T_box>
	inline void Octree<T_data, T_key, T_box>::RecursiveAddToSearch(Node* node)
	{
		if (node->count == -1)
		{
			for (int i = 0; i < 8; i++)
			{
				RecursiveAddToSearch(node->children[i]);
			}

			return;
		}

		for (int i = 0; i < node->count; i++)
		{
			if (!node->data[i]->accounted)
			{
				node->data[i]->accounted = true;
				node->data[i]->nextSearch = m_searchStart;
				m_searchStart = node->data[i];
				m_searchCount++;
			}
		}
	}

	template<typename T_data, typename T_key, typename T_box>
	inline typename Octree<T_data, T_key, T_box>::OctreeIterator Octree<T_data, T_key, T_box>::begin()
	{
		return OctreeIterator(m_searchStart, 0);
	}

	template<typename T_data, typename T_key, typename T_box>
	inline typename Octree<T_data, T_key, T_box>::OctreeIterator Octree<T_data, T_key, T_box>::end()
	{
		return OctreeIterator(nullptr, m_searchCount);
	}



	// Iterator

	template<typename T_data, typename T_key, typename T_box>
	inline Octree<T_data, T_key, T_box>::OctreeIterator::OctreeIterator(Octree<T_data, T_key, T_box>::Data* current, int count)
		:
		m_current(current),
		m_count(count)
	{
	}

	template<typename T_data, typename T_key, typename T_box>
	inline T_data& Octree<T_data, T_key, T_box>::OctreeIterator::OctreeIterator::operator*()
	{
		return m_current->data;
	}

	template<typename T_data, typename T_key, typename T_box>
	inline typename Octree<T_data, T_key, T_box>::OctreeIterator& Octree<T_data, T_key, T_box>::OctreeIterator::operator++()
	{
		m_count++;
		m_current = m_current->nextSearch;
		return *this;
	}

	template<typename T_data, typename T_key, typename T_box>
	inline bool Octree<T_data, T_key, T_box>::OctreeIterator::operator!=(const OctreeIterator& other)
	{
		return m_count != other.m_count;
	}

}
