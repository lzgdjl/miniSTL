#ifndef _ALGORITHM_H
#define _ALGORITHM_H

#include <cstring> // 为了使用 memset()
#include <utility>
#include <algorithm>
#include <cassert>

#include "mini_allocator.h"
#include "mini_functional.h"
#include "mini_iterator.h"
#include "mini_type_traits.h"
#include "mini_utility.h"

namespace miniSTL
{
	// fill
	// 时间复杂度O(N)
	template <typename ForwardIterator,typename T>
	inline void fill(ForwardIterator first, ForwardIterator last, const T &value)
	{
		for (;first != last;++first) // 迭代整个区间
			*first = value;          // 设定新值
	}

	template <>
	inline void fill(char *first, char *last, const char &value)
	{
		memset(first, static_cast<unsigned char>(value), last - first);
	}

	template <>
	inline void fill(wchar_t *first, wchar_t *last, const char &value)
	{
		memset(first, static_cast<unsigned char>(value), (last - first) * sizeof(wchar_t));
	}

	/*
	* 函数 fill_n
	* 时间复杂度：O(N)
	*/
	template <typename OutputIterator,typename Size,typename T>
	OutputIterator fill_n(OutputIterator first, Size n, const T &value)
	{
		for (;n > 0;--n, ++first)
			*first = value;
		return first;
	}

	template <typename Size>
	char *fill_n(char *first, Size n, const char &value)
	{
		memset(first, static_cast<unsigned char>(value), n);
		return first + n;
	}

	template <typename Size>
	wchar_t *fill_n(wchar_t *first, Size n, const wchar_t &value)
	{
		memset(first, static_cast<unsigned char>(value), n * sizeof(wchar_t));
	}

	/*
	* 函数 min
	* 函数复杂度:O(1)
	*/
	template <typename T>
	const T &min(const T &a, const T &b)
	{
		return !(b < a) ? a : b;
	}

	template <typename T,typename Compare>
	const T &min(const T &a, const T &b, Compare comp)
	{
		return !comp(b, a) ? a : b;
	}

	/*
	* 函数 max
	* 时间复杂度：O(1)
	*/
	template <typename T>
	const T &max(const T &a, const T &b)
	{
		return (a < b) ? b : a;
	}

	template <typename T,typename Compare>
	const T &max(const T &a, const T &b, Compare comp)
	{
		return comp(a, b) ? b : a;
	}

	/*
	* 函数：make_heap
	* 时间复杂度:O(N)
	*/

	// heap上溯算法
	template <typename RandomAccessIterator,typename Compare>
	static void up(RandomAccessIterator first, RandomAccessIterator last,
		RandomAccessIterator head, Compare comp)
	{
		//范围:[first,last]
		if (first != last)
		{
			auto index = last - head;
			auto parent_index = (index - 1) / 2;
			for (auto cur = last;parent_index >= 0 && cur != head;parent_index = (index - 1) / 2)
			{
				auto parent = head + parent_index;// 父节点
				if (comp(*parent, *cur))
					miniSTL::swap(*parent, *cur);
				cur = parent;
				index = cur - head;
			}
		}
	}

	template <typename RandomAccessIterator, typename Compare>
	static void down(RandomAccessIterator first, RandomAccessIterator last,
		RandomAccessIterator head, Compare comp)
	{
		if (first != last)
		{
			auto index = first - head;
			auto left_child_index = index * 2 + 1;
			for (auto cur = first;left_child_index < (last - head + 1) && cur < last;left_child_index = index * 2 + 1)
			{
				auto child = head + left_child_index;
				if ((child + 1) <= last && *(child + 1) > *child)
					child += 1;
				if (comp(*cur, *child))
					miniSTL::swap(*cur, *child);
				cur = child;
				index = cur - head;
			}
		}
	}

	template <typename RandomAccessIterator, typename Compare>
	void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		const auto range = last - first;
		for (auto cur = first + range / 2 - 1;cur >= first;--cur)
		{
			miniSTL::down(cur, last - 1, first, comp);
			if (cur == first)
				return;
		}
	}

	template <typename RandomAccessIterator>
	void make_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		make_heap(first, last,
			miniSTL::less< typename miniSTL::iterator_traits<RandomAccessIterator>::value_type>());
	}

	/*
	* push_heap
	* O(lgN)
	*/
	template <typename RandomAccessIterator,typename Compare>
	void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		miniSTL::up(first, last - 1, first, comp);
	}

	template <typename RandomAccessIterator>
	void push_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		miniSTL::push_heap(first, last,
			miniSTL::less<typename miniSTL::iterator_traits<RandomAccessIterator>::value_type>());
	}

	/*
	* pop_heap
	* O(lgN)
	*/
	template <typename RandomAccessIterator,typename Compare>
	void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		miniSTL::swap(*first, *(last - 1));
		if (last - first >= 2)
			miniSTL::down(first, last - 2, first, comp);
	}

	template <typename RandomAccessIterator>
	void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		miniSTL::pop_heap(first, last, miniSTL::less<typename miniSTL::iterator_traits<RandomAccessIterator>::value_type>());
	}

	/*
	* Sort_heap
	* O(N)
	*/
	template <typename RandomAccessIterator,typename Compare>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		for (auto cur = last;cur != first;--cur)
			miniSTL::pop_heap(first, cur, comp);
	}

	template <typename RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		miniSTL::sort_heap(first, last,
			miniSTL::less<typename miniSTL::iterator_traits<RandomAccessIterator>::value_type>());
	}

	/*
	* is_heap
	* O(N)
	*/
	template <typename RandomAccessIterator,typename Compare>
	bool is_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
	{
		const auto range = last - first;
		auto index = range / 2 - 1;
		for (auto cur = first + range / 2 - 1;cur >= first;--cur,--index)
		{
			if (*(first + (index * 2 + 1)) > *cur || // 左节点
				((first + (index * 2 + 2)) <= last && *(first + (index * 2 + 2)) > *cur))//右节点
			{
				return false;
			}
			if (cur == first)
				break;
		}
		return true;
	}

	template <typename RandomAccessIterator>
	bool is_heap(RandomAccessIterator first, RandomAccessIterator last)
	{
		return miniSTL::is_heap(first, last,
			miniSTL::less<typename miniSTL::iterator_traits<RandomAccessIterator>::value_type>());
	}

	/*
	* all_of()
	* O(N)
	*/
	template <typename InputIterator,typename UnaryPredicate>
	bool all_of(InputIterator first, InputIterator last, UnaryPredicate pred)
	{
		for (;first != last;++first)
		{
			if (!pred(*first))
				return false;
		}

		return true;
	}

	/*
	* any_of()
	* O(N)
	*/
	template <typename InputIterator,typename UnaryPredicate>
	bool any_of(InputIterator first, InputIterator last, UnaryPredicate pred)
	{
		for (;first != last;++first)
		{
			if (pred(*first))
				return true;
		}

		return false;
	}

	/*
	* none_of
	* O(N)
	*/
	template <typename InputIterator,typename UnaryPredicate>
	bool none_of(InputIterator first, InputIterator last, UnaryPredicate pred)
	{
		for (;first != last;++first)
		{
			if (pred(*first))
				return false;
		}
		return true;
	}

	/*
	* for_each()
	* O(N)
	*/
	template <typename InputIterator,typename Function>
	Function for_each(InputIterator first, InputIterator last, Function fn)
	{
		for (;first != last;++first)
			fn(*first);
		return fn;
	}

	/*
	* find()
	* O(N)
	*/
	template <typename InputIterator,typename T>
	InputIterator find(InputIterator first, InputIterator last, const T &val)
	{
		for (;first != last;++first)
		{
			if (*first == val)
				break;
		}
		return first;
	}

	/*
	* find_if
	* O(N)
	*/
	template <typename InputIterator,typename UnaryPredicate>
	InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate pred)
	{
		for (;first != last;++first)
		{
			if (pred(*first))
				break;
		}
		return first;
	}

	/*
	* find_end
	* O(N*N)
	*/
	template <typename ForwardIterator1,typename ForwardIterator2>
	ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2)
	{
		if (first2 == last2)
			return last1;
		auto ret = last1;
		while (first1 != last1)
		{
			auto it1 = first1;
			auto it2 = first2;
			while (*it1 == *it2)
			{
				++it1;
				++it2;
				if (it2 == last2)
				{
					ret = first1;
					break;
				}
				if (it1 == last1)
					return ret;
			}
			++first1;
		}
		return ret;
	}

	template <typename ForwardIterator1,typename ForwardIterator2,typename BinaryPredicate>
	ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2,
		BinaryPredicate pred)
	{
		if (first2 == last2)
			return last1;
		auto ret = last1;
		while (first1 != last1)
		{
			auto it1 = first1;
			auto it2 = first2;
			while (pred(*it1, *it2))
			{
				++it1;
				++it2;
				if (it2 == last2)
				{
					ret = first1;
					break;
				}
				if (it1 == last1)
					return ret;
			}
			++first1;
		}
		return ret;
	}

	/*
	* find_if_not
	* O(N)
	*/
	template <typename InputIterator,typename UnaryPredicate>
	InputIterator find_if_not(InputIterator first, InputIterator last, UnaryPredicate pred)
	{
		for (;first != last;++first)
		{
			if (!pred(*first))
				break;
		}
		return first;
	}

	/*
	* find_first_of()
	* O(N*N)
	*/
	template <typename ForwardIterator1,typename ForwardIterator2>
	ForwardIterator1 find_first_of(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2)
	{
		for (;first1 != last1;++first1)
		{
			for (auto it = first2;it != last2;++it)
			{
				if (*first1 == *it)
					return first1;
			}
		}
		return last1;
	}

	template <typename ForwardIterator1,typename ForwardIterator2,typename BinaryPredicate>
	ForwardIterator1 find_first_of(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2,
		BinaryPredicate pred)
	{
		for (;first1 != last1;++first1)
		{
			for (auto it = first2;it != last2;++it)
			{
				if (pred(*first1, *it))
					return first1;
			}
		}
		return last1;
	}


	/*
	* adjacent_find
	* O(N)
	*/
	template <typename ForwardIterator,typename BinaryPredicate>
	ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last,
		BinaryPredicate pred)
	{
		for (;first != last;++first)
		{
			if (first + 1 != last && pred(*first, *(first + 1)))
				break;
		}
		return first;
	}

	template <typename ForwardIterator>
	ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last)
	{
		return miniSTL::adjacent_find(first, last, equal_to<typename iterator_traits<ForwardIterator>::value_type>());
	}

	/*
	* Count
	* O(N)
	*/
	template <typename InputIterator,typename T>
	typename iterator_traits<InputIterator>::difference_type
		count(InputIterator first, InputIterator last, const T &val)
	{
		typename iterator_traits<InputIterator>::difference_type n = 0;
		for (;first != last;++first)
		{
			if (pred(*first))
				++n;
		}
		return n;
	}

	/*
	* mismatch
	* O(N)
	*/
	template <typename InputIterator1,typename InputIterator2>
	pair<InputIterator1,InputIterator2>
		mismatch(InputIterator1 first1, InputIterator1 last1,
			InputIterator2 first2)
	{
		for (;first1 != last1;++first1, ++first2)
		{
			if (*first1 != *first2)
				break;
		}
		return miniSTL::make_pair(first1, first2);
	}

	template <typename InputIterator1,typename InputIterator2,typename BinaryPredicate>
	pair<InputIterator1,InputIterator2>
		mismatch(InputIterator1 first1, InputIterator1 last1,
			InputIterator2 first2, BinaryPredicate pred)
	{
		for (;first1 != last1;++first1, ++first2)
		{
			if (!pred(*first1, *first2))
				break;
		}
		return miniSTL::make_pair(first1, first2);
	}

	/*
	* equal
	* O(N)
	*/
	template <typename InputIterator1,typename InputIterator2,typename BinaryPredicate>
	bool equal(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, BinaryPredicate pred)
	{
		for (;first1 != last1;++first1, ++first2)
		{
			if (!pred(*first1, *first2))
				return false;
		}
		return true;
	}


	template <typename InputIterator1,typename InputIterator2>
	bool equal(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2)
	{
		return miniSTL::equal(first1, last1, first2,
			miniSTL::equal_to<typename miniSTL::iterator_traits<InputIterator1>::value_type>());
	}

	/*
	* is_permutation
	* O(N*N)
	*/
	template <typename ForwardIterator1,typename ForwardIterator2,typename BinaryPredicate>
	bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, BinaryPredicate pred)
	{
		// 搜寻第一个不等的位置
		auto res = miniSTL::mismatch(first1, last1, first2, pred);
		first1 = res.first, first2 = res.senond;
		if (first1 == last1)
			return true;
		auto last2 = first2;
		std::advance(last2, std::distance(first1, last1));
		for (auto it1 = first1;it1 != last1;++it1)
		{
			if (miniSTL::find_if(first1, it1, [&](decltype(*first1) val) {return pred(val, *it1);}) == it1)
			{
				auto n = miniSTL::count(first2, last2, *it1);
				if (n == 0 || miniSTL::count(it1, last1, *it1))
					return false;
			}
		}
		return true;
	}

	/*
	* search
	* O(N*N)
	*/
	template <typename ForwardIterator1,typename ForwardIterator2,typename BinaryPredicate>
	ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2,
		BinaryPredicate pred)
	{
		while (first1 != last1)
		{
			auto it1 = first1;
			auto it2 = first2;
			while (it1 != last1 && it2 != last2)
			{
				if (pred(*it1, *it2))
				{
					++it1;
					++it2;
				}
				else
					break;
			}
			if (it2 == last2)
				return first1;
			if (it1 == last1)
				return last1;
			++first1;
		}
		return last1;
	}

	template <typename ForwardIterator1,typename ForwardIterator2>
	ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2)
	{
		return miniSTL::search(first1, first2, first2, last2,
			miniSTL::equal_to<typename miniSTL::iterator_traits<ForwardIterator1>::value_type>());
	}

	/*
	* advance
	* O(N)
	*/
	namespace
	{
		template <typename InputIterator,typename Distance>
		void _advance(InputIterator &it, Distance n, input_iterator_tag)
		{
			assert(n >= 0);
			while (n--)
			{
				++it;
			}
		}

		template <typename BidirectionalIterator,typename Distance>
		void _advance(BidirectionalIterator &it, Distance n, bidirectional_iterator_tag)
		{
			if (n < 0)
			{
				while (n++)
				{
					--it;
				}
			}
			else
			{
				while (n--)
				{
					++it;
				}
			}
		}

		template <typename RandomIterator,typename Distance>
		void _advance(RandomIterator &it, Distance n, random_access_iterator_tag)
		{
			if (n < 0)
			{
				it -= (-n);
			}
			else
			{
				it += n;
			}
		}
	}

	template <typename InputIterator,typename Distance>
	void advance(InputIterator &it, Distance n)
	{
		typedef iterator_traits<InputIterator>::iterator_category iterator_category;
		_advance(it, n, iterator_category());
	}

	/*
	* sort
	* O(NlogN)
	* 采用快速排序算法
	*/
	namespace
	{
		// [first,last]
		template <typename RandomIterator,typename BinaryPredicate>
		typename iterator_traits<RandomIterator>::value_type
			mid3(RandomIterator first, RandomIterator last, BinaryPredicate pred)
		{
			auto mid = first + (last + 1 - first) / 2;
			if (pred(*mid, *first))
			{
				swap(*mid, *first);
			}
			if (pred(*last, *mid))
			{
				swap(*last, *mid);
			}
			if (pred(*last, *first))
			{
				swap(*last, *first);
			}
			auto ret = *mid;
			swap(*mid, *(last - 1));
			return ret;
		}

		template <typename RandomIterator,typename BinaryPredicate>
		void bubble_sort(RandomIterator first, RandomIterator last, BinaryPredicate pred)
		{
			auto len = last - first;
			for (auto i = len;i != 0;--i)
			{
				bool swaped = false;
				for (auto p = first;p != (first + i - 1);++p)
				{
					if (pred(*(p + 1), *p))
					{
						swap(*(p + 1), *p);
						swaped = true;
					}
				}
				if (!swaped)
					break;
			}
		}
	}

	template <typename RandomIterator,typename BinaryPredicate>
	void sort(RandomIterator first, RandomIterator last, BinaryPredicate pred)
	{
		if (first >= last || first + 1 == last)
			return;
		if (last - first <= 20)//区间长度小于等于20采用冒泡排序
		{
			bubble_sort(first, last, pred);
			return;
		}

		auto mid = mid3(first, last - 1, pred);
		auto p1 = first, p2 = last - 2;
		while (p1 < p2)
		{
			while (pred(*p1, mid) && (p1 < p2))
				++p1;
			while (!pred(*p2, mid) && (p1 < p2))
				--p2;
			if (p1 < p2)
				swap(*p1, *p2);
		}
		swap(*p1, *(last - 2));
		sort(first, p1, pred);
		sort(p1 + 1, last, pred);
	}

	template <typename RandomIterator>
	void sort(RandomIterator first, RandomIterator last)
	{
		return sort(first, last, less<typename iteratr_traits<RandomIterator>::value_type>());
	}

	/*
	* generate
	* O(N)
	*/
	template <typename InputIterator,typename Function>
	void generate(InputIterator first, InputIterator last, Function fn)
	{
		for (;first != last;++first)
		{
			*first = fn();
		}
	}

	/*
	* generate_n
	* O(N)
	*/
	template <typename OutputIterator,typename Size,typename Generator>
	void generate_n(OutputIterator first, Size n, Generator gen)
	{
		while (n--)
		{
			*first = gen();
			++first;
		}
	}

	/*
	* Distance
	* O(N)
	*/
	template <typename InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		_distance(InputIterator first, InputIterator last, input_iterator_tag)
	{
		typename iterator_traits<InputIterator>::difference_type dist = 0;
		while (first++ != last)
		{
			++dist;
		}
		return dist;
	}

	template <typename RandomIterator>
	typename iterator_traits<RandomIterator>::difference_type
		_distance(RandomIterator first, RandomIterator last, random_access_iterator_tag)
	{
		auto dist = last - first;
		return dist;
	}

	template <typename Iterator>
	typename iterator_traits<Iterator>::difference_type
		distance(Iterator first, Iterator last)
	{
		typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
		return _distance(first, last, iterator_category());
	}

	/*
	* copy()
	* O(N)
	*/
	template <typename InputIterator,typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last,
		OutputIterator result);

	template <>
	inline char *copy(const char *first, const char *last, char *result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	template <>
	inline wchar_t *copy(const wchar_t *first, const wchar_t *last,
		wchar_t *result)
	{
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	template <typename InputIterator,typename OutputIterator>
	struct __copy_dispatch;

	template <typename RandomAccessIterator,typename OutputIterator,typename Distance>
	inline OutputIterator
		__copy_d(RandomAccessIterator first, RandomAccessIterator last,
			OutputIterator result, Distance*)
	{
		// 以 n 决定循环的执行次数.速度快
		for (Distance n = last - first;n > 0;--n, ++result, ++first)
		{
			*result = *first;
		}
		return result;
	}
	
	template <typename T>
	inline T *__copy_t(const T *first, const T *last, T *result,
		_true_type)
	{
		memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}

	template <typename T>
	inline T *__copy_t(const T *first, const T *last, T *result,
		_false_type)
	{
		return __copy_d(first, last, result, (ptrdiff_t*)(nullptr));
	}


	template <typename T>
	struct __copy_dispatch<T*, T*>
	{
		T *operator()(T *first, T *last, T *result)
		{
			typedef typename _type_traits<T>::is_POD_type is_pod;
			return __copy_t(first, last, result, is_pod());
		}
	};

	template <typename T>
	struct __copy_dispatch<const T*, T*>
	{
		T *operator()(const T *first, const T *last, T *result)
		{
			typedef typename _type_traits<T>::is_POD_type is_pod;
			return __copy_t(first, last, result, is_pod());
		}
	};

	// InputIterator 版本
	template <typename InputIterator,typename OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last,
		OutputIterator result, input_iterator_tag)
	{
		// 以迭代器等同与否，决定循环是否继续。速度慢
		for (;first != last;++result, ++first)
			*result = *first;
		return result;
	}

	// RandomAccessIterator 版本
	template <typename RandomAccessIterator,typename OutputIterator>
	inline OutputIterator
		__copy(RandomAccessIterator first, RandomAccessIterator last,
			OutputIterator result, random_access_iterator_tag)
	{
		return __copy_d(first, last, result, difference_type(first));
	}

	template <typename InputIterator,typename OutputIterator>
	struct __copy_dispatch
	{
		OutputIterator operator()(InputIterator first, InputIterator last,
			OutputIterator result)
		{
			return __copy(first, last, result, iterator_category(first));
		}
	};

	template <typename InputIterator,typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last,
		OutputIterator result)
	{
		return __copy_dispatch<InputIterator, OutputIterator>()
			(first, last, result);
	}
}


#endif