#pragma once

#include <string>
#include "ShuffleMap.h"

//namespace cs
//{
//
//	template <typename T_char>
//	class _Archive
//	{
//	private:
//		typedef std::basic_string<T_char, std::char_traits<T_char>, std::allocator<T_char>> T_string;
//
//	public:
//		_Archive(unsigned int shelfCapacity = 2048);
//		_Archive(const _Archive& lval);
//		_Archive(_Archive&& rval);
//
//		int GetContentSize() const;
//
//	private:
//		struct Shelf
//		{
//			T_char* content;
//			unsigned char* index;
//		};
//
//		int m_shelfCapacity;
//		int m_contentSize;
//
//		ShuffleMap<int, 
//	};
//
//	typedef _Archive<char> Archive;
//	typedef _Archive<wchar_t> WArchive;
//
//}
