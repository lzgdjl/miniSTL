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

		// ������ destroy() ��һ���汾������һ��ָ��
		template <typename T>
		inline void destroy(T *ptr)
		{
			if (ptr)
				ptr->~T();
		}

		// ������ destroy() �ڶ��汾�������������������˺����跨�ҳ�Ԫ�ص���ֵ���ͣ�
		// �������� __type_traits<> ��ȡ���ʵ���ʽ
		template <typename ForwardIterator>
		inline void destroy(ForwardIterator first, ForwardIterator last)
		{
			typedef typename _type_traits<typename iterator_traits<ForwardIterator>::value_type>::is_POD_type is_POD_type;
			_destroy(first, last, is_POD_type());
		}
	}
	
}

#endif