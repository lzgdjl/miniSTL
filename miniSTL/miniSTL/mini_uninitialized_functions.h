#ifndef _UNINITIALIZED_FUNCTIONS_H
#define _UNINITIALIZED_FUNCTIONS_H

#include "mini_algorithm.h"
#include "mini_construct.h"
#include "mini_iterator.h"
#include "mini_type_traits.h"

namespace miniSTL
{

	
	/*
	* 实现三个“全局函数”，放在命名空间globals中
	* uninitialized_copy()
	* uninitialized_fill()
	* uninitialized_fill_n()
	*/
	namespace
	{
		template <typename InputIterator, typename ForwardIterator>
		ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result,
			_true_type);

		template <typename InputIterator, typename ForwardIterator>
		ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
			ForwardIterator result, _false_type);

		template <typename ForwardIterator, typename T>
		inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
			const T &value, _true_type);

		template <typename ForwardIterator, typename T>
		void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
			const T &value, _false_type);

		template <typename ForwardIterator, typename Size, typename T>
		ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, _true_type);

		template <typename ForwardIterator, typename Size, typename T>
		ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, _false_type);
	}

	namespace globals
	{
		/*
		* uninitialized_copy接受三个参数
		*   迭代器 first 指向输入端的起始位置
		*	迭代器 last 指向输入端的结束位置
		*	迭代器 result 指向输出端（欲初始化空间）的起始处。
		* _uninitialized_copy_aux()为辅助函数模板
		*/

		template <typename InputIterator,typename ForwardIterator>
		ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
		{
			typedef typename _type_traits<typename iterator_traits<InputIterator>::value_type>::is_POD_type is_pod;
			return _uninitialized_copy_aux(first, last, result, is_pod());
		}

		/*
		* 针对 char* 和 wchar_t* 两种类型，可以采用最具效率的做法 memmove
		*/
		template <>
		inline char *uninitialized_copy(const char *first, const char *last,
			char *result)
		{
			memmove(result, first, last - first);
			return result + (last - first);
		}

		template <>
		inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last,
			wchar_t *result)
		{
			memmove(result, first, sizeof(wchar_t) * (last - first));
			return result + (last - first);
		}

		/*
		* uninitialized_fill() 接受三个参数
		*	迭代器 first 指向输出端 (欲初始化空间）的起始处
		*	迭代器 last 指向输出端（欲初始化空间）的结束处（前闭后开空间）
		*	x 表示初值
		* _uninitialized_fill_aux 用作辅助函数模板
		*/

		
		template <typename ForwardIterator,typename T>
		inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value)
		{
			typedef typename _type_traits<T>::is_POD_Type is_pod;
			_uninitialized_fill_aux(first, last, value, is_pod());
		}
		
		/*
		* uninitialized_fill_n() 接受三个参数
		* 迭代器 first 指向欲初始化空间的开始处
		* n 表示欲初始化空间的大小
		* x 表示初值
		*/
		template <typename ForwardIterator,typename Size,typename T>
		inline ForwardIterator uninitialized_fill_n(ForwardIterator first,
			Size n, const T &value)
		{
			typedef typename _type_traits<T>::is_POD_type is_pod;
			return _uninitialized_fill_n_aux(first, n, value, is_pod());
		}
	}

	namespace
	{
		template <typename InputIterator, typename ForwardIterator>
		ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
			ForwardIterator result, _true_type)
		{
			// 是POD类型，可采用效率最高的复制手法
			return copy(first, last, result);
		}

		// 不是 POD 类型
		template <typename InputIterator, typename ForwardIterator>
		ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
			ForwardIterator result, _false_type)
		{
			auto cur = result;
			for (;first != last;++cur, ++first)
			{
				construct(&*cur, *first);
			}

			return cur;
		}

		// 如果是 POD 类型
		template <typename ForwardIterator, typename T>
		inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
			const T &value, _true_type)
		{
			fill(first, last, value);
		}

		// 如果不是 POD 类型
		template <typename ForwardIterator, typename T>
		void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
			const T &value, _false_type)
		{

			for (auto cur = first;cur != last;++cur)
			{
				construct(&*first, value);
			}
		}

		// 如果是 POD 类型
		template <typename ForwardIterator, typename Size, typename T>
		ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n,
			const T &value, _true_type)
		{
			return fill_n(first, n, value);
		}

		// 如果不是 POD 类型
		template <typename ForwardIterator, typename Size, typename T>
		ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n,
			const T &value, _false_type)
		{
			for (auto cur = first;n > 0;--n, ++cur)
			{
				construct(&*cur, value);
			}
			return cur;
		}
	}
	
}

#endif