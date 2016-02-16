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
	* �ռ�������Ĭ�ϲ��� allocator<T>
	*/
	template <typename T,typename Alloc = allocator<T>>
	class vector final
	{
	private:
		T *_start; // ��ʾĿǰʹ�ÿռ��ͷ
		T *_finish; // ��ʾĿǰʹ�ÿռ��β
		T *_end_of_storage; // ��ʾĿǰ���ÿռ��β

		typedef Alloc data_allocator;

	public:
		// vector ��Ƕ�����Ͷ���
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
		// ���죬���ƣ�������غ���
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

		// �Ƚϲ�����غ���
		bool operator==(const vector &v) const;
		bool operator!=(const vector &v) const;

		// ���������
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

		// ���������
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

		// ����Ԫ�����
		reference operator[] (const size_type i) { return *(begin() + i); }
		const_reference operator[] (const size_type i) const { return *(cbegin() + i); }
		reference front() { return *(begin()); }
		reference back() { return *(end() - 1); }
		pointer data() { return _start; }

		// �޸�������صĲ�������
		// ������������������е����ж���ʹ������ size Ϊ0�����������������еĿռ�
		void clear()
		{
			erase(begin(), end());
		}
		void swap(vector &v);
		void push_back(const value_type &value)
		{
			// ��Ԫ�ز�������β��
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

		// �����Ŀռ���������غ���
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


// ʵ��vector
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

	// �߼��Ƚ���غ���
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
			// ���б��ÿռ�
			// �ڱ��ÿռ俪ʼ������һ��Ԫ�أ����� vector ���һ��Ԫ��ֵΪ���ֵ
			globals::construct(_finish, *(_finish - 1));
			// ����
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
			// ���ޱ��ÿռ�
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			// ���Ϸ���ԭ�����ԭ��СΪ0�������1.
			// ���ԭ��С��Ϊ0�������ԭ��С��������
			// ǰ�����������ԭ���ݣ�����׼����������������

			iterator new_start = data_allocator::allocate(len);// ʵ�ʷ���
			iterator new_finish = new_start;
			try
			{
				// ��ԭ vector �����ݿ������� vector
				new_finish = globals::uninitialized_copy(_start, position, new_start);
				// Ϊ��Ԫ���趨��ֵ val
				globals::construct(new_finish, val);
				// ����
				++new_finish;
				// ��������ԭ����Ҳ��������
				new_finish = globals::uninitialized_copy(position, _finish, new_finish);
			}
			catch (...)
			{
				// �ع�
				globals::destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			// �������ͷ�ԭ vector
			globals::destroy(begin(), end());
			deallocate();

			// ������������ָ���� vector
			_start = new_start;
			_finish = new_finish;
			_end_of_storage = new_start + len;
		}
	}

	// ��� [first,last) �е�����Ԫ��
	template <typename T,typename Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
	{
		iterator i = copy(last, _finish, first);
		globals::destroy(i, _finish);
		_finish = _finish - (last - first);
		return first;
	}

	// ���ĳ��λ���ϵ�Ԫ��
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
			// �� n != 0 �Ž����������в���
			if (static_cast<size_type>(_end_of_storage - _finish) >= n)
			{
				// ���ÿռ���ڵ��ڡ�����Ԫ�ظ�����
				value_type val_copy = x;
				// ���¼�������֮�������Ԫ�ظ���
				const size_type elems_after = _finish - position;
				iterator old_finish = _finish;
				if (elems_after > n)
				{
					// �����֮�������Ԫ�ظ���  > ����Ԫ�ظ���
					globals::uninitialized_copy(_finish - n, _finish, _finish);
					_finish += n;// �� vector β�˱�Ǻ���
					for (auto index = old_finish-1;index - position != n - 1;index--)
					{
						*index = *(index - n);
					}

					// �Ӳ���㿪ʼ������ֵ
					fill(position, position + n, val_copy);
				}
				else
				{
					// �����֮�������Ԫ�ظ��� <= ����Ԫ�ظ���
					globals::uninitialized_fill_n(_finish, n - elems_after, val_copy);
					_finish += n - elems_after;
					globals::uninitialized_copy(position, old_finish, _finish);
					_finish += elems_after;
					fill(position, old_finish, val_copy);
				}
			}
			else
			{
				// ���ÿռ�С�� ����Ԫ�ظ�����������������ڴ�
				// ���Ⱦ����³��ȣ��ɳ��ȵ���������ɳ���+����Ԫ�ظ���
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);

				// ���������µ� vector �ռ�
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try
				{
					// ���Ƚ��� vector �Ĳ����֮ǰ��Ԫ�ظ��Ƶ��¿ռ�
					new_finish = globals::uninitialized_copy(_start, position, new_start);
					// �ٽ�����Ԫ�������¿ռ�
					new_finish = globals::uninitialized_fill_n(new_finish, n, val);
					// �ٽ��� vector �Ĳ����֮���Ԫ�ظ��Ƶ��¿ռ�
					new_finish = globals::uninitialized_copy(position, _finish, new_finish);
				}
				catch (...)
				{
					globals::destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}

				// ������ͷžɵ� vector
				globals::destroy(_start, _finish);
				deallocate();

				// ����
				_start = new_start;
				_finish = new_finish;
				_end_of_storage = new_start + len;
			}
		}
	}

}

#endif