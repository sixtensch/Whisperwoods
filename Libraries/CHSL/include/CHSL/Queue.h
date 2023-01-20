#pragma once

/* CHSL

	|	Queue<T>
	|
	|	- Growing FIFO cyclical container.

*/









#include "Debug.h"
#include "Math.h"

namespace cs
{

	template<typename T>
	class Queue
	{
	public:
		Queue();
		~Queue();
		Queue(const Queue&);
		Queue(Queue&&);

		Queue<T>& operator=(const Queue<T>&);
		Queue<T>& operator=(Queue<T>&&);

		int Size() const;
		int Capacity() const;
		bool Empty() const;
		T& At(int index) const;
		T& Last() const;
		T& Peek() const;

		void Clear();
		void Push(const T& value);
		T Pop();

	private:
		void Expand();

	private:
		static constexpr int c_dCapacity = 16;

		T* m_elements;
		int m_size;
		int m_capacity;
		int m_first;
		int m_last;
	};





	// Implementation
	
	// Construction

	template<typename T>
	inline Queue<T>::Queue()
		:
		m_elements(new T[c_dCapacity]),
		m_size(0),
		m_capacity(c_dCapacity),
		m_first(0),
		m_last(-1)
	{
	}

	template<typename T>
	inline Queue<T>::~Queue()
	{
		delete[] m_elements;
	}

	template<typename T>
	inline Queue<T>::Queue(const Queue<T>& lVal)
		:
		m_elements(nullptr)
	{
		*this = lVal;
	}

	template<typename T>
	inline Queue<T>::Queue(Queue<T>&& rVal)
		:
		m_elements(nullptr)
	{
		*this = static_cast<Queue<T>&&>(rVal);
	}

	template<typename T>
	inline Queue<T>& Queue<T>::operator=(const Queue<T>& lVal)
	{
		if (this == &lVal)
		{
			return *this;
		}

		m_size = lVal.m_size;
		m_capacity = lVal.m_capacity;
		m_first = lVal.m_first;
		m_last = lVal.m_last;

		delete[] m_elements;
		m_elements = new T[m_capacity];
		for (int i = 0; i < m_size; ++i)
		{
			int index = (m_first + i) % m_size;
			m_elements[index] = lVal.m_elements[index];
		}

		return *this;
	}

	template<typename T>
	inline Queue<T>& Queue<T>::operator=(Queue<T>&& rVal)
	{
		if (this == &rVal)
		{
			return *this;
		}

		m_size = rVal.m_size;
		m_capacity = rVal.m_capacity;
		m_first = rVal.m_first;
		m_last = rVal.m_last;

		delete[] m_elements;
		m_elements = rVal.m_elements;

		rVal.m_elements = nullptr;
		rVal.Clear();

		return *this;
	}

	template<typename T>
	inline int Queue<T>::Size() const
	{
		return m_size;
	}

	template<typename T>
	inline int Queue<T>::Capacity() const
	{
		return m_capacity;
	}

	template<typename T>
	inline bool Queue<T>::Empty() const
	{
		return m_size == 0;
	}

	template<typename T>
	inline T& Queue<T>::At(int index) const
	{
		if (index < 0 || index >= m_size)
		{
			throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Queue index out of bounds.");
		}

		return m_elements[index];
	}

	template<typename T>
	inline T& Queue<T>::Last() const
	{
		if (m_size == 0)
		{
			throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Cannot reference last member of an empty queue.");
		}

		return m_elements[m_last];
	}

	template<typename T>
	inline T& Queue<T>::Peek() const
	{
		if (m_size == 0)
		{
			throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Cannot peek member of an empty queue.");
		}

		return m_elements[m_first];
	}



	// Non-const

	template<typename T>
	inline void Queue<T>::Clear()
	{
		m_size = 0;
		m_first = 0;
		m_last = -1;
	}

	template<typename T>
	inline void Queue<T>::Push(const T& value)
	{
		Expand();

		m_size++;

		m_last = cs::imod(m_last + 1, m_capacity);
		m_elements[m_last] = value;
	}

	template<typename T>
	inline T Queue<T>::Pop()
	{
		if (m_size == 0)
		{
			throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Cannot pop member off empty queue.");
		}

		m_size--;

		int member = m_first;
		m_first = cs::imod(m_first + 1, m_capacity);
		return m_elements[member];
	}

	template<typename T>
	void Queue<T>::Expand()
	{
		if (m_size < m_capacity)
		{
			return;
		}

		T* newElements = new T[m_capacity * 2];

		for (int i = m_first; i < m_capacity; i++)
		{
			newElements[i - m_first] = m_elements[i];
		}

		for (int i = 0; i < m_first; i++)
		{
			newElements[i + m_capacity - m_first] = m_elements[i];
		}

		m_first = 0;
		m_last = m_capacity - 1;
		m_capacity *= 2;

		delete[] m_elements;
		m_elements = newElements;
	}

}