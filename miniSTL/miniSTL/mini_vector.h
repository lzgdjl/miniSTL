#ifndef _VECTOR_H
#define _VECTOR_H

#include "mini_allocator.h"
#include "mini_algorithm.h"
#include "mini_iterator.h"
#include "mini_reverse_iterator.h"
#include "mini_uninitialized_functions.h"
#include "mini_type_traits.h"

namespace miniSTL
{
	/*
	* 空间配置器默认采用 allocator<T>
	*/
	template <typename T,typename Alloc = allocator<T>>
	class vector final
	{
	private:
		T *_start; // 表示目前使用空间的头
		T *_finish; // 表示目前使用空间的尾
		T *_end_of_storage; // 表示目前可用空间的尾

		typedef Alloc data_allocator;

	public:
		// vector 的嵌套类型定义
		typedef T										value_type;
		typedef value_type*								pointer;
		typedef value_type*								iterator;
		typedef const value_type*						const_iterator;
		typedef reverse_iterator_t<value_type*>			reverse_iterator;
		typedef reverse_iterator_t<const value_type*>	const_reverse_iterator;
		typedef value_type&								reference;
		typedef const value_type&						const_reference;
		typedef size_t									size_type;
		typedef ptrdiff_t								difference_type;

	private:
		void insert_aux(iterator position, const value_type &value);
		void deallocate()
		{
			if (_start)
				data_allocator::deallocate(_start, _end_of_storage - _start);
			_start = _finish = _end_of_storage = nullptr;
		}

		void fill_initialize(size_type n, const value_type &value)
		{
			_start = allocate_and_fill_n(n, value);
			_finish = _start + n;
			_end_of_storage = _finish;
		}

	public:
		// 构造，复制，析构相关函数
		vector():_start(nullptr),_finish(nullptr),_end_of_storage(nullptr){}
		explicit vector(const size_type n) { fill_initialize(n, T()); }
		vector(const size_type n, const value_type &value) { fill_initialize(n, value); }
		vector(int n, const value_type &value) { fill_initialize(n, value); }
		vector(long n, const value_type &value) { fill_initialize(n, value); }

		template <typename InputIterator>
		vector(InputIterator first, InputIterator last);
		vector(const vector &v);
		vector(vector &&v);
		vector &operator=(const vector &v);
		vector &operator=(vector &&v);
		~vector()
		{
			globals::destroy(_start, _finish);
			deallocate();
		}

		// 比较操作相关函数
		bool operator==(const vector &v) const;
		bool operator!=(const vector &v) const;

		// 迭代器相关
		iterator begin() { return _start; }
		const_iterator begin() const { return _start; }
		const_iterator cbegin() const { return _start; }
		iterator end() { return _finish; }
		const_iterator end() const { return _finish; }
		const_iterator cend() const { return _finish; }
		reverse_iterator rbegin() { return reverse_iterator(_finish); }
		const_reverse_iterator crbegin() const { return const_reverse_iterator(_finish); }
		reverse_iterator rend() { return reverse_iterator(_start); }
		const_reverse_iterator crend() const { return const_reverse_iterator(_start); }

		// 与容量相关
		size_type size() const { return static_cast<size_type>(end() - begin()); }
		size_type capacity() const { return _end_of_storage - _start; }
		bool empty() const { return _start == _finish; }
		void resize(size_type new_size, const value_type &val = value_type())
		{
			if (new_size < size())
				erase(begin() + new_size, end());
			else
				insert(end(), new_size - size(), val);
		}
		void shrink_to_fit();

		// 访问元素相关
		reference operator[] (const size_type i) { return *(begin() + i); }
		const_reference operator[] (const size_type i) const { return *(cbegin() + i); }
		reference front() { return *(begin()); }
		reference back() { return *(end() - 1); }
		pointer data() { return _start; }

		// 修改容器相关的操作函数
		// 清空容器，销毁容器中的所有对象并使容器的 size 为0，但不回收容器已有的空间
		void clear()
		{
			erase(begin(), end());
		}
		void swap(vector &v);
		void push_back(const value_type &value)
		{
			// 将元素插入至最尾端
			if (_finish != _end_of_storage)
			{
				globals::construct(_finish, value);
				++_finish;
			}
			else
				insert_aux(end(), value);
		}
		void pop_back()
		{
			--_finish;
			globals::destroy(_finish);
		}

		void insert(iterator position, const size_type n, const value_type &val);

		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);
		void reserve(size_type n);

		// 容器的空间配置器相关函数
		Alloc get_allocator() const { return data_allocator; }

	private:
		void destroy_and_deallocate_all();
		iterator allocate_and_fill_n(const size_type n, const value_type &value)
		{
			iterator result = data_allocator::allocate(n);
			globals::uninitialized_fill_n(result, n, value);
			return result;
		}

	public:
		template <typename T,typename Alloc>
		friend bool operator==(const vector<T, Alloc> &v1, const vector<T, Alloc> &v2);
		
		template <typename T,typename Alloc>
		friend bool operator!=(const vector<T, Alloc> &v1, const vector<T, Alloc> &v2);

	};
}


// 实现vector
namespace miniSTL
{

	template <typename T,typename Alloc>
	vector<T, Alloc>::vector(vector &&v)
	{
		_start = v._start;
		_finish = v._finish;
		_end_of_storage = v._end_of_storage;
		v._start = v._finish = v._end_of_storage = nullptr;
	}

	template <typename T,typename Alloc>
	vector<T, Alloc> &vector<T, Alloc>::operator=(vector &&v)
	{
		if (this != &v)
		{
			destroy_and_deallocate_all();
			_start = v._start;
			_finish = v._finish;
			_end_of_storage = v._end_of_storage;
			v._start = v._finish = v._end_of_storage = nullptr;
		}
		return *this;
	}

	template <typename T,typename Alloc>
	void vector<T, Alloc>::reserve(size_type n)
	{
		if (n <= capacity())
			return;

		T *new_start = data_allocator::allocate(n);
		T *new_finish = miniSTL::globals::uninitialized_copy(begin(), end(), new_start);
		destroy_and_deallocate_all();

		_start = new_start;
		_finish = new_finish;
		_end_of_storage = _start + n;
	}

	// 逻辑比较相关函数
	template <typename T,typename Alloc>
	bool vector<T, Alloc>::operator==(const vector &v) const
	{
		if (size() != v.size())
		{
			return false;
		}
		else
		{
			auto ptr1 = _start;
			auto ptr2 = v._start;
			for (;ptr1 != _finish && ptr2 != v._finish;++ptr1, ++ptr2)
			{
				if (*ptr1 != *ptr2)
					return false;
			}
			return true;
		}
	}

	template <typename T,typename Alloc>
	bool vector<T, Alloc>::operator!=(const vector &v) const
	{
		return !(*this == v);
	}

	template <typename T,typename Alloc>
	bool operator==(const vector<T, Alloc> &v1, const vector<T, Alloc> &v2)
	{
		return v1.operator==(v2);
	}

	template <typename T,typename Alloc>
	bool operator!=(const vector<T, Alloc> &v1, const vector<T, Alloc> &v2)
	{
		return !(v1 == v2);
	}

	template <typename T,typename Alloc>
	void vector<T, Alloc>::shrink_to_fit()
	{
		data_allocator::deallocate(_finish, _end_of_storage - _finish);
		_end_of_storage = _finish;
	}

	template <typename T,typename Alloc>
	void vector<T, Alloc>::swap(vector &v)
	{
		if (this != &v)
		{
			miniSTL::swap(_start, v._start);
			miniSTL::swap(_finish, v._finish);
			miniSTL::swap(_end_of_storage, v._end_of_storage);
		}
	}

	template <typename T,typename Alloc>
	void vector<T, Alloc>::destroy_and_deallocate_all()
	{
		if (capacity() != 0)
		{
			data_allocator::destroy(_start, _finish);
			data_allocator::deallocate(_start, capacity());
		}

		_start = _finish = _end_of_storage = nullptr;
	}

	template <typename T,typename Alloc>
	void vector<T, Alloc>::insert_aux(iterator position, const value_type &val)
	{
		if (_finish != _end_of_storage)
		{
			// 还有备用空间
			// 在备用空间开始处构造一个元素，并以 vector 最后一个元素值为其初值
			globals::construct(_finish, *(_finish - 1));
			// 调整
			++_finish;
			value_type val_copy = val;
			for (auto index = _finish - 2;index != position;index--)
			{
				*index = *(index - 1);
			}
			*position = val_copy;
		}
		else
		{
			// 已无备用空间
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			// 以上分配原则：如果原大小为0，则分配1.
			// 如果原大小不为0，则分配原大小的两倍，
			// 前半段用来放置原数据，后半段准备用来放置新数据

			iterator new_start = data_allocator::allocate(len);// 实际分配
			iterator new_finish = new_start;
			try
			{
				// 将原 vector 的内容拷贝到新 vector
				new_finish = globals::uninitialized_copy(_start, position, new_start);
				// 为新元素设定初值 val
				globals::construct(new_finish, val);
				// 调整
				++new_finish;
				// 将安插点的原内容也拷贝过来
				new_finish = globals::uninitialized_copy(position, _finish, new_finish);
			}
			catch (...)
			{
				// 回滚
				globals::destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			// 析构并释放原 vector
			globals::destroy(begin(), end());
			deallocate();

			// 调整迭代器，指向新 vector
			_start = new_start;
			_finish = new_finish;
			_end_of_storage = new_start + len;
		}
	}

	// 清除 [first,last) 中的所有元素
	template <typename T,typename Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
	{
		iterator i = copy(last, _finish, first);
		globals::destroy(i, _finish);
		_finish = _finish - (last - first);
		return first;
	}

	// 清除某个位置上的元素
	template <typename T,typename Alloc>
	typename vector<T,Alloc>::iterator
		vector<T, Alloc>::erase(iterator position)
	{
		if (position + 1 != end())
			copy(position + 1, _finish, position);
		--_finish;
		globals::destroy(_finish);
		return position;
	}

	template <typename T,typename Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const value_type &val)
	{
		if (n != 0)
		{
			// 当 n != 0 才进行以下所有操作
			if (static_cast<size_type>(_end_of_storage - _finish) >= n)
			{
				// 备用空间大于等于“新增元素个数”
				value_type val_copy = x;
				// 以下计算插入点之后的现有元素个数
				const size_type elems_after = _finish - position;
				iterator old_finish = _finish;
				if (elems_after > n)
				{
					// 插入点之后的现有元素个数  > 新增元素个数
					globals::uninitialized_copy(_finish - n, _finish, _finish);
					_finish += n;// 将 vector 尾端标记后移
					for (auto index = old_finish-1;index - position != n - 1;index--)
					{
						*index = *(index - n);
					}

					// 从插入点开始填入新值
					fill(position, position + n, val_copy);
				}
				else
				{
					// 插入点之后的现有元素个数 <= 新增元素个数
					globals::uninitialized_fill_n(_finish, n - elems_after, val_copy);
					_finish += n - elems_after;
					globals::uninitialized_copy(position, old_finish, _finish);
					_finish += elems_after;
					fill(position, old_finish, val_copy);
				}
			}
			else
			{
				// 备用空间小于 新增元素个数（必须分配额外的内存
				// 首先决定新长度：旧长度的两倍，或旧长度+新增元素个数
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);

				// 以下配置新的 vector 空间
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try
				{
					// 首先将旧 vector 的插入点之前的元素复制到新空间
					new_finish = globals::uninitialized_copy(_start, position, new_start);
					// 再将新增元素填入新空间
					new_finish = globals::uninitialized_fill_n(new_finish, n, val);
					// 再将旧 vector 的插入点之后的元素复制到新空间
					new_finish = globals::uninitialized_copy(position, _finish, new_finish);
				}
				catch (...)
				{
					globals::destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}

				// 清除并释放旧的 vector
				globals::destroy(_start, _finish);
				deallocate();

				// 调整
				_start = new_start;
				_finish = new_finish;
				_end_of_storage = new_start + len;
			}
		}
	}

}

#endif