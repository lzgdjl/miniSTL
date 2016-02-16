#ifndef _UNINITIALIZED_FUNCTIONS_H
#define _UNINITIALIZED_FUNCTIONS_H

#include "mini_algorithm.h"
#include "mini_construct.h"
#include "mini_iterator.h"
#include "mini_type_traits.h"

namespace miniSTL
{

	
	/*
	* ʵ��������ȫ�ֺ����������������ռ�globals��
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
		* uninitialized_copy������������
		*   ������ first ָ������˵���ʼλ��
		*	������ last ָ������˵Ľ���λ��
		*	������ result ָ������ˣ�����ʼ���ռ䣩����ʼ����
		* _uninitialized_copy_aux()Ϊ��������ģ��
		*/

		template <typename InputIterator,typename ForwardIterator>
		ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
		{
			typedef typename _type_traits<typename iterator_traits<InputIterator>::value_type>::is_POD_type is_pod;
			return _uninitialized_copy_aux(first, last, result, is_pod());
		}

		/*
		* ��� char* �� wchar_t* �������ͣ����Բ������Ч�ʵ����� memmove
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
		* uninitialized_fill() ������������
		*	������ first ָ������� (����ʼ���ռ䣩����ʼ��
		*	������ last ָ������ˣ�����ʼ���ռ䣩�Ľ�������ǰ�պ󿪿ռ䣩
		*	x ��ʾ��ֵ
		* _uninitialized_fill_aux ������������ģ��
		*/

		
		template <typename ForwardIterator,typename T>
		inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value)
		{
			typedef typename _type_traits<T>::is_POD_Type is_pod;
			_uninitialized_fill_aux(first, last, value, is_pod());
		}
		
		/*
		* uninitialized_fill_n() ������������
		* ������ first ָ������ʼ���ռ�Ŀ�ʼ��
		* n ��ʾ����ʼ���ռ�Ĵ�С
		* x ��ʾ��ֵ
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
			// ��POD���ͣ��ɲ���Ч����ߵĸ����ַ�
			return copy(first, last, result);
		}

		// ���� POD ����
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

		// ����� POD ����
		template <typename ForwardIterator, typename T>
		inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
			const T &value, _true_type)
		{
			fill(first, last, value);
		}

		// ������� POD ����
		template <typename ForwardIterator, typename T>
		void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
			const T &value, _false_type)
		{

			for (auto cur = first;cur != last;++cur)
			{
				construct(&*first, value);
			}
		}

		// ����� POD ����
		template <typename ForwardIterator, typename Size, typename T>
		ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n,
			const T &value, _true_type)
		{
			return fill_n(first, n, value);
		}

		// ������� POD ����
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