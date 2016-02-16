#pragma once
#ifndef _CONSTRUCT_H
#define _CONSTRUCT_H

#include <new> // for placement new
#include "mini_type_traits.h"

namespace miniSTL
{
	namespace
	{
		template <typename ForwardIterator>
		inline void _destroy(ForwardIterator first, ForwardIterator last, _true_type)
		{
			;
		}

		template <typename ForwardIterator>
		inline void _destroy(ForwardIterator first, ForwardIterator last, _false_type)
		{
			for (;first != last;++first)
			{
				destroy(&*first);
			}
		}
	}

	namespace globals
	{
		template <typename T1, typename T2>
		inline void construct(T1 *ptr, const T2 &value)
		{
			new(ptr)T1(value);
		}

		// 以下是 destroy() 第一个版本，接受一个指针
		template <typename T>
		inline void destroy(T *ptr)
		{
			if (ptr)
				ptr->~T();
		}

		// 以下是 destroy() 第二版本，接受两个迭代器。此函数设法找出元素的数值类型，
		// 进而利用 __type_traits<> 求取最适当方式
		template <typename ForwardIterator>
		inline void destroy(ForwardIterator first, ForwardIterator last)
		{
			typedef typename _type_traits<typename iterator_traits<ForwardIterator>::value_type>::is_POD_type is_POD_type;
			_destroy(first, last, is_POD_type());
		}
	}
	
}

#endif