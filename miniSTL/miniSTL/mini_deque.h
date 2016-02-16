#ifndef _DEQUE_H
#define _DEQUE_H

#include "mini_allocator.h"
#include "mini_iterator.h"
#include "mini_reverse_iterator.h"
#include "mini_utility.h"

namespace miniSTL
{
	namespace
	{
		template <typename T,typename Alloc = allocator<T>>
		class deque;

		// deque 的迭代器
		template <typename T>
		struct deque_iter : public iterator<bidirectional_iterator_tag, T>
		{
			typedef const miniSTL::deque<T> *cntr_ptr;
			size_t map_index;
			T *cur;
			cntr_ptr container;

			deque_iter():map_index(-1),cur(0),container(0) {}
			deque_iter(size_t index,T *ptr,cntr_ptr container)
				:map_index(index),cur(ptr),container(container) {}
			deque_iter(const deque_iter &it)
				:map_index(it.map_index),cur(it.cur),container(it.container) {}
			deque_iter &operator=(const deque_iter &it);
			void swap(deque_iter &it);

			reference operator*() { return *cur; }
			const reference operator*() const { return *cur; }

			pointer operator->() { return &(operator*()); }
			const pointer operator->() const { return &(operator*()); }

			deque_iter &operator++();
			deque_iter operator++(int);
			deque_iter &operator--();
			deque_iter operator--(int);
			
			bool operator==(const deque_iter &it) const;
			bool operator!=(const deque_iter &it) const;
		
			T *get_buck_tail(size_t map_index) const;
			T *get_buck_head(size_t map_index) const;
			size_t get_buck_size() const;
		};
		template <typename T>
		deque_iter<T> operator+(const deque_iter<T> &it, typename deque_iter<T>::difference_type n);

		template <typename T>
		deque_iter<T> operator+(typename deque_iter<T>::difference_type n, const deque_iter<T> &it);

		template <typename T>
		fdeque_iter<T> operator-(const deque_iter<T> &it, typename deque_iter<T>::difference_type n);

		template <typename T>
		deque_iter<T> operator-(typename deque_iter<T>::difference_type n, const deque_iter<T> &it);

		template <typename T>
		typename deque_iter<T>::difference_type operator-(const deque_iter<T> &it1, const deque_iter<T> &it2);

		template <typename T>
		void swap(deque_iter<T> &lhs, deque_iter<T> &rhs);
	}

	template <typename T,typename Alloc = allocator<T>>
	class deque
	{
	private:
		template <typename T>
		friend class miniSTL::deque_iter;

	public:
		typedef T					value_type;
		typedef deque_iter<T>		iterator;
		typedef deque_iter<const T>	const_iterator;
		typedef T&					reference;
		typedef const reference		const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;
		typedef Alloc				allocator_type;

	private:
		typedef Alloc				data_allocator;
		enum class EBuckSize{BUCKSIZE = 64};

	private:
		iterator start, finish;
		size_t map_size;
		T **map;

	public:
		deque();
		explicit deque(size_type n, const value_type &val = value_type());
		template <typename InputIterator>
		deque(InputIterator first, InputIterator last);
		deque(const deque &other);

		~deque();

		deque &operator=(const deque &other);
		deque &operator=(deque &&other);

		iterator begin();
		iterator end();
		iterator begin() const;
		iterator end() const;

	public:
		size_type size() const { return end() - begin(); }
		bool empty() const { return begin() == end(); }

		reference operator[](size_type n);
		reference front();
		reference back();
		const_reference operator[](size_type n) const;
		const_reference front() const;
		const_reference back() const;

		void push_back(const value_type &val);
		void push_front(const value_type &val);
		void pop_back();
		void pop_front();
		void swap(deque &other);
		void clear();

	private:
		T *get_a_new_buck();
		T **get_a_new_map(size_t size);
		size_t get_new_map_size(size_t size);
		size_t get_buck_size() const;
		void init();
		bool back_full() const;
		bool front_full() const;
		void deque_aux(size_t n, const value_type &val, std::true_type);
		template <typename Iterator>
		void deque_aux(Iterator first, Iterator last, std::false_type);
		void reallocate_and_copy();

	public:
		template <typename T,typename Alloc>
		friend bool operator==(const deque<T, Alloc> &lhs, const deque<T, Alloc> &rhs);
		template <typename T,typename Alloc>
		friend bool operator!=(const deque<T, Alloc> &lhs, const deque<T, Alloc> &rhs);
		template <typename T,typename Alloc>
		friend void swap(deque<T, Alloc> &x, deque<T, Alloc> &y);
	};

	namespace
	{
		template <typename T>
		deque_iter<T> &deque_iter<T>::operator++()
		{
			if (cur != get_buck_tail(map_index))
				++cur;// +1 后还在同一个桶里
			else if (map_index + 1 < container->map_size)
			{
				// +1 后还在同一个map里
				++map_index;
				cur = get_buck_head(map_index);
			}
			else
			{
				// +1 后跳出了map
				map_index = container->map_size;
				cur = container->map[map_index];
			}
			return *this;
		}

		template <typename T>
		deque_iter<T> deque_iter<T>::operator++(int)
		{
			auto result = *this;
			++*this;
			return result;
		}

		template <typename T>
		deque_iter<T> &deque_iter<T>::operator--()
		{
			if (cur != get_buck_head(map_index)) // 当前不指向桶头
				--cur;
			else if (map_index - 1 >= 0)
			{
				// -1 后还在 map 
				--map_index;
				cur = get_buck_tail(map_index);
			}
			else
			{
				map_idnex = 0;
				cur = container->map[map_index]; // 指向 map[0] 的头
			}

			return *this;
		}

		template <typename T>
		deque_iter<T> deque_iter<T>::operator--(int)
		{
			auto res = *this;
			--*this;
			return res;
		}

		template <typename T>
		deque_iter<T> deque_iter<T>::operator==(const deque_iter<T> &other) const
		{
			return map_index == other.map_index &&
				cur == other.cur && container == other.container;
		}

		template <typename T>
		bool deque_iter<T>::operator!=(const deque_iter<T> &other) const
		{
			return !(*this == other);
		}

		template <typename T>
		deque_iter<T> &deque_iter<T>::operator=(const deque_iter<T> &other)
		{
			if (this != &it)
			{
				map_index = other.map_index;
				cur = other.cur;
				container = other.container;
			}
			return *this;
		}

		template <typename T>
		void deque_iter<T>::swap(deque_iter<T> &other)
		{
			miniSTL::swap(map_index, other.map_index);
			miniSTL::swap(cur, other.cur);
			miniSTL::swap(container, other.container);
		}

		template <typename T>
		deque_iter<T> operator+(const deque_iter &it, typename deque_iter<T>::difference_type n)
		{
			deque_iter<T> result(it);
			auto m = result.get_buck_tail(result.map_index) - res.cur;
			if (n <= m)
				result.cur += n;
			else
			{
				n = n - m;
				result.map_index += n / it.get_buck_size() + 1;
				auto p = result.get_buck_head(result.map_index);
				auto x = n % it.get_buck_size() - 1;
				result.cur = p + x;
			}

			return result;
		}

		template <typename T>
		deque_iter<T> operator+(typename deque_iter<T>::difference_type n, const deque_iter<T> &it)
		{
			return it + n;
		}

		template <typename T>
		deque_iter<T> operator-(const deque_iter<T> &it, typename deque_iter<T>::difference_type n)
		{
			deque_iter<T> result(it);
			auto m = result.cur - result.get_buck_head(res.map_index);
			if (n <= m)
				result.cur -= n;
			else
			{
				n = n - m;
				result.map_index -= n / result.get_buck_size() + 1;
				result.cur = result.get_buck_tail(result.map_index) - (n % result.get_buck_size() - 1);
			}

			return result;
		}

		template <typename T>
		deque_iter<T> operator-(typename deque_iter<T>::difference_type n, const deque_iter<T> &it)
		{
			return it - n;
		}

		template <typename T>
		typename deque_iter<T>::difference_type operator-(const deque_iter<T> &it1, const deque_iter<T> &it2)
		{
			if (it1.container == it2.container && it1.container == 0)
				return 0;
			return typename deque_iter<T>::difference_type(it1.get_buck_size()) * (it1.map_index - it2.map_index - 1)
				+ (it1.cur - it1.get_buck_head(it1.map_index)) + (it2.get_buck_tail(it2.map_index) - it2.cur) + 1;
		}

		template <typename T>
		void swap(deque_iter<T> &lhs, deque_iter<T> &rhs)
		{
			lhs.swap(rhs);
		}

		template <typename T>
		T *deque_iter<T>::get_buck_tail(size_t index) const
		{
			return container->map[index] + (container->get_buck_size() - 1);
		}

		template <typename T>
		T *deque_iter<T>::get_buck_head(size_t index) const
		{
			return container->map[index];
		}

		template <typename T>
		size_t deque_iter<T>::get_buck_size() const
		{
			return container->get_buck_size();
		}
	}

	template <typename T,typename Alloc>
	bool deque<T, Alloc>::back_full() const
	{
		return map[map_size - 1] && map[map_size] == end().cur;
	}

	template <typename T,typename Alloc>
	bool deque<T, Alloc>::front_full() const
	{
		return map[0] && map[0] == begin().cur;
	}

	template <typename T,typename Alloc>
	void deque<T, Alloc>::deque_aux(size_t n, const value_type &val, std::true_type)
	{
		int i = 0;
		for (;i != n / 2;++i)
			(*this).push_front(val);
		for (;i != n;++i)
			(*this).push_back(val);
	}

	template <typename T,typename Alloc>
	template <typename Iterator>
	void deque<T, Alloc>::deque_aux(Iterator first, Iterator last, std::false_type)
	{
		difference_type mid = (last - first) / 2;
		for (auto it = first + mid;it != first - 1;--it)
			(*this).push_front(*it);
		for (auto it = first + mid + 1;it != last;++it)
			(*this).push_back(*it);
	}

	template <typename T,typename Alloc>
	void deque<T, Alloc>::init()
	{
		map_size = 2;
		map = get_a_new_map(map_size);
		start.container = finish.container = this;
		start.map_index = finish.map_index = map_size - 1;
		start.cur = finish.cur = map[map_size - 1];
	}

	template <typename T,typename Alloc>
	T *deque<T, Alloc>::get_a_new_buck()
	{
		return data_allocator::allocate(get_buck_size());
	}

	template <typename T,typename Alloc>
	T** deque<T, Alloc>::get_a_new_map(size_t size)
	{
		T **result = allocator<T*>::allocate(size);
		for (int i = 0;i != size;++i)
			map[i] = get_a_new_buck();
		return result;
	}

	template <typename T, typename Alloc>
	size_t deque<T, Alloc>::get_new_map_size(size_t size)
	{
		return size == 0 ? 2 : size * 2;
	}

	template <typename T,typename Alloc>
	size_t deque<T, Alloc>::get_buck_size() const
	{
		return (size_t)EBuckSize::BUCKSIZE;
	}

	template <typename T,typename Alloc>
	void deque<T, Alloc>::clear()
	{
		for (auto i = 0;i != map_size;++i)
		{
			for (auto p = map[i];!p && p != map[i] + get_buck_size();++p)
			{
				data_allocator::destroy(p);
			}
		}

		map_size = 0;
		start.map_index = finish.map_index = map_size / 2;
		start.cur = finish.cur = map[map_size / 2];
	}

	template <typename T,typename Alloc>
	typename deque<T, Alloc>::reference deque<T, Alloc>::operator[](size_type n)
	{
		return *(begin() + n);
	}

	template <typename T,typename Alloc>
	typename deque<T, Alloc>::reference deque<T, Alloc>::front()
	{
		return *begin();
	}

	template <typename T,typename Alloc>
	typename deque<T, Alloc>::reference deque<T, Alloc>::back()
	{
		return *(*end() - 1);
	}

	template <typename T,typename Alloc>
	typename deque<T, Alloc>::const_reference deque<T, Alloc>::operator[](size_type n) const
	{
		return *(begin() + n);
	}

	template <typename T,typename Alloc>
	typename deque<T, Alloc>::const_reference deque<T, Alloc>::front() const
	{
		return *begin();
	}

	template <typename T,typename Alloc>
	typename deque<T, Alloc>::const_reference deque<T, Alloc>::back() const
	{
		return *(end() - 1);
	}

	template <typename T,typename Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::begin() { return start; }
	template <typename T,typename Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::end() { return finish; }

	template <typename T,typename Alloc>
	typename deque<T, Alloc>::const_iterator deque<T, Alloc>::cbegin() const { return start; }
	template <typename T,typename Alloc>
	typename deque<T, Alloc>::const_iterator deque<T, Alloc>::cend() const { return finish; }

	template <typename T,typename Alloc>
	deque<T, Alloc>::~deque()
	{
		for (int i = 0;i != map_size;++i)
		{
			for (auto p = map[i];!p && p != map[i] + get_buck_size();++p)
			{
				data_allocator::destroy(p);
			}
			if (!map[i])
				data_allocator::deallocate(map[i], get_buck_size());
		}

		allocator<T*>::deallocate(map, map_size);
	}

	template <typename T,typename Alloc>
	deque<T,Alloc>::deque()
		:map_size(0),map(0){}

	template <typename T,typename Alloc>
	deque<T, Alloc>::deque(size_type n, const value_type &val = value_type())
	{
		deque();
		deque_aux(n, val, typename std::is_integral<size_type>::type());
	}

	template <typename T,typename Alloc>
	template <typename InputIterator>
	deque<T, Alloc>::deque(InputIterator first, InputIterator last)
	{
		deque();
		deque_aux(first, last, typename std::is_integral<InputIterator>::type());
	}

	template <typename T,typename Alloc>
	deque<T, Alloc>::deque(const deque &other)
	{
		map_size = other.map_size;
		map = get_a_new_map(map_size);
		for (int i = 0;i + other.start.map_index != other.map_size;++i)
		{
			for (int j = 0;j != get_buck_size();++j)
				map[other.start.map_index + i][j] = other.map[other.start.map_index + i][j];
		}

		start.map_index = other.start.map_index;
		finish.map_index = other.finish.map_index;
		start.cur = map[start.map_index] + (other.start.cur - other.map[other.start.map_index]);
		finish.cur = map[finish.map_index] + (other.finish.cur - other.map[other.finish.map_index]);
		start.container = finish.container = this;
	}

	template <typename T,typename Alloc>
	void deque<T, Alloc>::reallocate_and_copy()
	{
		auto new_map_size = get_new_map_size(map_size);
		T **new_map = get_a_new_map(new_map_size);
		size_t start_index = new_map_size / 4;

		for (int i = 0;i + start.map_index != map_size; = +i)
		{
			for (int j = 0;j != get_buck_size();++j)
				new_map[start_index + i][j] = map[start.map_index + i][j];
		}

		size_t n = start.cur - map[start.map_index];
		auto size = this->size();
		auto b = start, e = finish;
		clear();
		map_size = new_map_size;
		map = new_map;
		start = iterator(start_index, new_map[start_index] + n, this);
		finish = start + size;
	}

	template <typename T,typename Alloc>
	void deque<T, Alloc>::push_back(const value_type &val)
	{
		if (empty())
		{
			init();
		}
		else if (back_full())
		{
			reallocate_and_copy();
		}

		miniSTL::globals::construct(finish.cur, val);
		++finish;
	}

	template <typename T,typename Alloc>
	void deque<T, Alloc>::push_front(const value_type &val)
	{
		if (empty())
		{
			init();
		}
		else if (front_full())
		{
			reallocate_and_copy();
		}

		--start;

		globals::construct(start.cur, val);
	}

	template <typename T,typename Alloc>
	void deque<T, Alloc>::pop_front()
	{
		data_allocator::destroy(start.cur);
		++start;
	}

	template <typename T,typename Alloc>
	void deque<T, Alloc>::swap(deque<T, Alloc> &other)
	{
		miniSTL::swap(map_size, other.map_size);
		miniSTL::swap(map, other.map);
		start.swap(other.start);
		finish.swap(other.finish);
	}

	template <typename T,typename Alloc>
	bool operator==(const deque<T, Alloc> &lhs, const deque<T, Alloc> &rhs)
	{
		auto cit1 = lhs.begin(), cit2 = rhs.begin();
		for (;cit1 != lhs.end() && cit2 != rhs.end();++cit1, ++cit2)
		{
			if (*cit1 != *cit2)
				return false;
		}

		if (cit1 == lhs.end() && cit2 == rhs.end())
			return true;
		return false;
	}

	template <typename T,typename Alloc>
	bool operator!=(const deque<T, Alloc> &lhs, const deque<T, Alloc> &rhs)
	{
		return !(lhs == rhs);
	}

	template <typedef T,typename Alloc>
	void swap(deque<T, Alloc> &x, deque<T, Alloc> &y)
	{
		x.swap(y);
	}
}

#endif
