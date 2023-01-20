#pragma once

/* CHSL

	|	Indexer
	|
	|	- Semi-complex cyclical list type for storing and retrieving items using unique ID:s.
	|	- Not extensively tested.

*/









#include <iostream>

#include "CHSLTypedef.h"
#include "Math.h"
#include "Quadtree.h"

namespace cs
{
	// Fixed-size cyclic list of members with fixed indices
	template <typename T, size_t C, byte CBits = 16>
	class Indexer
	{
	public:
		Indexer()
		{
			for (int i = 0; i < C; ++i)
			{
				m_array[i].used = false;
				m_array[i].fIndex = (i + 1) % C;
				m_array[i].bIndex = (i - 1 + C) % C;
			}

			m_count = 0;
			m_head = 0;

			m_max = 0;
		}

		///// <summary>
		///// Init with default constructor values for each member before usage. 
		///// </summary>
		//template <typename... P>
		//Indexer(P&&... args)
		//{
		//	m_array = (Member*)malloc(sizeof(Member) * C);

		//	for (size_t i = 0; i < C; ++i)
		//	{
		//		m_array[i] = { T(std::forward(args)), false, (i + 1) % C, (i - 1 + C) % C };
		//	}

		//	m_count = 0;
		//	m_head = 0;
		//	m_first = 0;
		//	m_last = 0;
		//}

		Indexer(const Indexer& lVal)
		{
			delete[] m_array;
			m_array = new Member[C];

			for (int i = 0; i < C; ++i)
			{
				m_array[i] = lVal.m_array[i];
			}

			m_count = lVal.m_count;
			m_head = lVal.m_head;
		}

		Indexer(Indexer&& rVal)
		{
			delete[] m_array;
			m_array = rVal.m_array;
			rVal.m_array = nullptr;

			m_count = rVal.m_count;
			m_head = rVal.m_head;
		}

		~Indexer()
		{
			// delete[] m_array;
		}



		// Add member and return index

		int Add(const T& value)
		{
			if (m_count >= C)
			{
				throw std::length_error("Tried to exceed Indexer capacity.");
			}

			m_count++;

			Member& current = m_array[m_head];

			if (m_head >= m_max)
			{
				m_max = m_head;
			}

			// Maximizing array special case
			if (m_count == C)
			{
				// Will create an unstable state, which is resolved in Remove
				current.value = value;
				current.used = true;
				return (int)m_head;
			}

			current.value = value;
			current.used = true;
			m_array[current.fIndex].bIndex = current.bIndex;
			m_array[current.bIndex].fIndex = current.fIndex;

			size_t usedIndex = m_head;
			m_head = current.fIndex;

			return (int)usedIndex;
		}



		// Remove member by index
		void Remove(int index)
		{
			if (m_count == 0 || index < 0 || index >= C || !m_array[index].used)
			{
				return;
			}

			m_count--;

			Member& current = m_array[index];

			// Special case if array full
			if (m_count == C - 1)
			{
				m_head = index;

				current.used = false;
				Optimize();

				return;
			}

			// Normal procedure

			if (m_array[current.bIndex].used)
			{
				size_t pUnused;
				for (size_t i = cs::imod(current.bIndex - 1 + C, C); m_array[cs::imod(i + 1, C)].used; i = cs::imod(i - 1 + C, C))
				{
					pUnused = i;
				}

				if (pUnused < current.bIndex)
				{
					for (size_t i = index; i > pUnused; --i)
					{
						m_array[i].bIndex = pUnused;
					}
				}
				else
				{
					for (size_t i = index; i != pUnused; i = cs::imod(i - 1 + C, C))
					{
						m_array[i].bIndex = pUnused;
					}
				}
			}
			else
			{
				m_array[current.bIndex].fIndex = index;
			}

			for (size_t i = cs::imod(index + 1, C); m_array[cs::imod(i - 1 + C, C)].used; i = cs::imod(i + 1, C))
			{
				current.fIndex = i;
				m_array[i].bIndex = index;
			}

			current.used = false;

			if (index < m_head)
			{
				m_head = index;
			}
		}



		// Check if index exists

		bool Exists(int index) const
		{
			if (index >= C || index < 0)
			{
				return false;
			}

			return m_array[index].used;
		}



		// Get member by index

		const T& Get(int index) const
		{
			if (index >= C || index < 0)
			{
				throw std::out_of_range("Tried to get Indexer member out of range.");
			}

			return m_array[index].value;
		}

		const T& operator[](int index) const
		{
			return Get(index);
		}



		// Get member by index non-const

		T& Get(int index)
		{
			if (index >= C || index < 0)
			{
				throw std::out_of_range("Tried to get Indexer member out of range.");
			}

			return m_array[index].value;
		}

		T& operator[](int index)
		{
			return Get(index);
		}



		// Misc

		void Clear()
		{
			for (size_t i = 0; i < C; ++i)
			{
				m_array[i].used = false;
				m_array[i].fIndex = imod(i + 1, C);
				m_array[i].bIndex = imod(i - 1 + C, C);
			}

			m_count = 0;
			m_head = 0;
			m_max = 0;
		}

		void Optimize()
		{
			if (m_count == C)
			{
				return;
			}
			else if (m_count == 0)
			{
				Clear();
				return;
			}

			size_t firstEmpty = 0;

			while (m_array[firstEmpty].used)
			{
				firstEmpty++;
			}

			size_t latestEmpty = firstEmpty;

			for (size_t i = firstEmpty + 1; i < C; ++i)
			{
				Member& current = m_array[i];

				if (current.used)
				{
					current.bIndex = latestEmpty;
					current.fIndex = imod(i + 1, C);
				}
				else
				{
					m_array[latestEmpty].fIndex = i;

					current.bIndex = latestEmpty;

					latestEmpty = i;
				}
			}

			for (size_t i = 0; i < firstEmpty; ++i)
			{
				Member& current = m_array[i];

				if (current.used)
				{
					current.bIndex = latestEmpty;
					current.fIndex = i + 1;
				}
				else
				{
					m_array[latestEmpty].fIndex = i;

					current.bIndex = latestEmpty;

					latestEmpty = i;
				}
			}

			m_array[firstEmpty].bIndex = latestEmpty;
		}


	public:
		// For iterating
		struct Iterator
		{
			Indexer* indexer;
			size_t current;

			Iterator& operator++()
			{
				current++;

				for (; current <= indexer->m_max; ++current)
				{
					if (indexer->m_array[current].used)
					{
						break;
					}
				}

				return *this;
			}

			T& operator*()
			{
				return indexer->m_array[current].value;
			}

			bool operator!=(const Iterator& lVal)
			{
				return current != lVal.current;
			}
		};

		Iterator begin()
		{
			if (m_count == 0)
			{
				return { this, m_max + 1 };
			}

			for (size_t i = 0; i <= m_max; ++i)
			{
				if (m_array[i].used)
				{
					return { this, i };
				}
			}

			return { this, m_max + 1 };
		}

		Iterator end()
		{
			return { this, m_max + 1 };
		}



	private:
		struct Member
		{
			T value;
			bool used : 1;
			size_t fIndex : CBits;
			size_t bIndex : CBits;
		};



		Member m_array[C];
		size_t m_count;
		size_t m_head;
		size_t m_max;
	};
}