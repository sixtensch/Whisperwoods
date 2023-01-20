#pragma once

/* CHSL

	|	Quadtree
	|
	|	- Quadtree implementation, effectively scaled down from the Octree.
	|	- Less fully featured than Octree.

*/









#include "Vector.h"
#include "Box.h"

namespace cs
{

	template <typename T_data, typename T_key, typename T_rect>
	class QuadtreeIterator;

	template<typename T_data, typename T_key = cs::Box2, typename T_rect = cs::Box2>
	class Quadtree
	{
	public:
		Quadtree(T_rect bounds, int quadrantCapacity = 16);
		Quadtree(Quadtree&&);
		Quadtree(const Quadtree&) = delete;
		virtual ~Quadtree();

		// Returns true if added
		bool Add(const T_data& data, const T_key& key);
		// Sets up internal search. Retrieve results by iterating over the Quadtree.
		// Returns amount of hits.
		int Search(void* data, bool kSelector(const T_key& key, void* data), bool rSelector(const T_rect& rectangle, void* data));
		// Searches for the corresponding rectangle(s) and removes their members from the tree.
		// Returns amount of removed hits.
		int Remove(void* data, bool kSelector(const T_key& key, void* data), bool rSelector(const T_rect& rectangle, void* data));

	public:
		// Premade search functions

		// data -> Rectangle<R>
		static bool SearchEquals(const T_rect& rectangle, void* data);




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
			Node* children[4] = { nullptr };
			T_rect rect;

			// -1 -> not a leaf
			int count;
			int capacity;
			Data** data = nullptr;
		};

	private:
		void RecursiveDelete(Node* node);
		bool RecursiveAdd(Node* node, Data* data);
		void RecursiveSearch(Node* node, void* data, bool kSelector(const T_key& key, void* data), bool rSelector(const T_rect& rectangle, void* data));



		// Variables

	private:
		Node* m_root;
		Data* m_data;

		Data* m_searchStart;
		int m_searchCount;

		int m_size;
		int m_height;
		int m_quadCapacity;



		// Iterators

	public:
		//template <typename T, typename R>
		class QuadtreeIterator
		{
		public:
			QuadtreeIterator(Quadtree<T_data, T_key, T_rect>::Data* current, int count);

			T_data& operator*();
			QuadtreeIterator& operator++();
			bool operator!=(const QuadtreeIterator& other);

		private:
			Quadtree<T_data, T_key, T_rect>::Data* m_current;
			int m_count;
		};

		QuadtreeIterator begin();
		QuadtreeIterator end();
	};



	// Implementation

	template<typename T_data, typename T_key, typename T_rect>
	inline Quadtree<T_data, T_key, T_rect>::Quadtree(T_rect bounds, int quadCapacity)
		:
		m_root( new Node { { nullptr }, bounds, 0, quadCapacity, new Data*[quadCapacity] }),
		m_data(nullptr),
		m_searchStart(nullptr),
		m_searchCount(0),
		m_size(0),
		m_height(0),
		m_quadCapacity(quadCapacity)
	{
	}

	template<typename T_data, typename T_key, typename T_rect>
	inline Quadtree<T_data, T_key, T_rect>::Quadtree(Quadtree&& rVal)
		:
		m_root(rVal.m_root),
		m_data(rVal.m_data),
		m_searchStart(rVal.m_searchStart),
		m_searchCount(rVal.m_searchCount),
		m_size(rVal.m_size),
		m_height(rVal.m_height),
		m_quadCapacity(rVal.m_quadCapacity)
	{
		rVal.m_data = nullptr;
		rVal.m_root = nullptr;
	}

	template<typename T_data, typename T_key, typename T_rect>
	inline Quadtree<T_data, T_key, T_rect>::~Quadtree()
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

	template<typename T_data, typename T_key, typename T_rect>
	bool Quadtree<T_data, T_key, T_rect>::Add(const T_data& data, const T_key& key)
	{
		if (!m_root->rect.Touches(key))
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

	template<typename T_data, typename T_key, typename T_rect>
	bool Quadtree<T_data, T_key, T_rect>::SearchEquals(const T_rect& rectangle, void* data)
	{
		return false;
	}

	template<typename T_data, typename T_key, typename T_rect>
	int Quadtree<T_data, T_key, T_rect>::Search(
		void* data,
		bool kSelector(const T_key& key, void* data),
		bool rSelector(const T_rect& rectangle, void* data))
	{
		m_searchStart = nullptr;
		m_searchCount = 0;

		RecursiveSearch(m_root, data, kSelector, rSelector);

		Data* current = m_searchStart;
		for (int i = 0; i < m_searchCount; i++)
		{
			current->accounted = false;
			current = current->nextSearch;
		}

		return m_searchCount;
	}

	template<typename T_data, typename T_key, typename T_rect>
	void Quadtree<T_data, T_key, T_rect>::RecursiveDelete(Node* node)
	{
		if (node->count == -1)
		{
			for (int i = 0; i < 4; i++)
			{
				RecursiveDelete(node->children[i]);
			}
		}

		delete[] node->data;
		delete node;
	}

	template<typename T_data, typename T_key, typename T_rect>
	bool Quadtree<T_data, T_key, T_rect>::RecursiveAdd(Node* node, Data* data)
	{
		if (!node->rect.Touches(data->key))
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
			auto size = node->rect.size / 2;
			T_rect r[] = {
				T_rect(node->rect.position, size),
				T_rect(node->rect.position.x + size.x, node->rect.position.y, size.x, size.y),
				T_rect(node->rect.position.x, node->rect.position.y + size.y, size.x, size.y),
				T_rect(node->rect.position + size, size),
			};

			for (int i = 0; i < 4; i++)
			{
				node->children[i] = new Node{ { nullptr }, r[i], 0, node->capacity, new Data*[node->capacity] };

				for (int j = 0; j < node->count; j++)
				{
					RecursiveAdd(node->children[i], node->data[j]);
				}

				if (node->children[i]->count > node->capacity - 2)
				{
					int newCapacity = node->capacity + m_quadCapacity;
					Data** replacement = new Data*[newCapacity];
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
		for (int i = 0; i < 4; i++)
		{
			added |= RecursiveAdd(node->children[i], data);
		}

		return added;
	}

	template<typename T_data, typename T_key, typename T_rect>
	void Quadtree<T_data, T_key, T_rect>::RecursiveSearch(
		Node* node,
		void* data,
		bool kSelector(const T_key& key, void* data),
		bool rSelector(const T_rect& rectangle, void* data))
	{
		if (node->count == 0 || !rSelector(node->rect, data))
		{
			return;
		}

		if (node->count == -1)
		{
			for (int i = 0; i < 4; i++)
			{
				RecursiveSearch(node->children[i], data, kSelector, rSelector);
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

	template<typename T_data, typename T_key, typename T_rect>
	inline typename Quadtree<T_data, T_key, T_rect>::QuadtreeIterator Quadtree<T_data, T_key, T_rect>::begin()
	{
		return QuadtreeIterator(m_searchStart, 0);
	}

	template<typename T_data, typename T_key, typename T_rect>
	inline typename Quadtree<T_data, T_key, T_rect>::QuadtreeIterator Quadtree<T_data, T_key, T_rect>::end()
	{
		return QuadtreeIterator(nullptr, m_searchCount);
	}



	// Iterator

	template<typename T_data, typename T_key, typename T_rect>
	inline Quadtree<T_data, T_key, T_rect>::QuadtreeIterator::QuadtreeIterator(Quadtree<T_data, T_key, T_rect>::Data* current, int count)
		:
		m_current(current),
		m_count(count)
	{
	}

	template<typename T_data, typename T_key, typename T_rect>
	inline T_data& Quadtree<T_data, T_key, T_rect>::QuadtreeIterator::QuadtreeIterator::operator*()
	{
		return m_current->data;
	}

	template<typename T_data, typename T_key, typename T_rect>
	inline typename Quadtree<T_data, T_key, T_rect>::QuadtreeIterator& Quadtree<T_data, T_key, T_rect>::QuadtreeIterator::operator++()
	{
		m_count++;
		m_current = m_current->nextSearch;
		return *this;
	}

	template<typename T_data, typename T_key, typename T_rect>
	inline bool Quadtree<T_data, T_key, T_rect>::QuadtreeIterator::operator!=(const QuadtreeIterator& other)
	{
		return m_count != other.m_count;
	}

}
