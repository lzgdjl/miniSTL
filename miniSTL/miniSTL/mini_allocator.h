#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

#include "mini_alloc.h"
#include "mini_construct.h"

namespace miniSTL
{
	/*
	* ø’º‰≈‰÷√∆˜
	*/
	template <typename T>
	class allocator
	{
	public:
		typedef T			value_type;
		typedef T*			pointer;
		typedef const T*	const_pointer;
		typedef T&			reference;
		typedef const T&	const_reference;
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

	public:
		allocator() noexcept {}
		allocator(const allocator &a) noexcept {}
		template <typename Tp>
		allocator(const allocator<Tp> &) noexcept() {}

		static T *allocate();
		static T *allocate(size_t n);
		static void deallocate(T *ptr);
		static void deallocate(T *ptr, size_t n);

		static void construct(T *ptr);
		static void construct(T *ptr, const T& value);
		static void destroy(T *ptr);
		static void destroy(T *first, T *last);
	};

	template <typename T>
	T *allocator<T>::allocate()
	{
		return static_cast<T*>(_alloc::allocate(sizeof(T)));
	}

	template <typename T>
	T *allocator<T>::allocate(size_t n)
	{
		if (n == 0)
			return nullptr;
		return static_cast<T*>(_alloc::allocate(sizeof(T) * n));
	}

	template <typename T>
	void allocator<T>::deallocate(T *ptr)
	{
		_alloc::deallocate(static_cast<void *>(ptr), sizeof(T));
	}

	template <typename T>
	void allocator<T>::deallocate(T *ptr, size_t n)
	{
		if (n == 0)
			return;
		_alloc::deallocate(static_cast<void*>(ptr), sizeof(T)*n);
	}

	template <typename T>
	void allocator<T>::construct(T *ptr)
	{
		globals::construct(ptr£¨T());
	}

	template <typename T>
	void allocator<T>::construct(T *ptr, const T &value)
	{
		globals::construct(ptr, value);
	}

	template <class T>
	void allocator<T>::destroy(T *ptr)
	{
		globals::destroy(ptr);
	}
	
	template <typename T>
	void allocator<T>::destroy(T *first, T *last)
	{
		globals::destroy(first, last);
	}
}

#endif