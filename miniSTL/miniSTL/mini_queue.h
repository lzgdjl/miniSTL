#ifndef _QUEUE_H
#define _QUEUE_H

#include "mini_deque.h"
#include "mini_functional.h"
#include "mini_vector.h"

namespace miniSTL
{
	template <typename T,typename Container = miniSTL::deque<T>>
	class queue final
	{
	public:
		typedef T									value_type;
		typedef Container							container_type;
		typedef typename Container::reference		reference;
		typedef typename Container::const_reference	const_reference;
		typedef typename Container::size_type		size_type;

	private:
		Container container;
		
	public:
		explicit queue(const container_type &cntr = container_type()):container(cntr) {}

		bool empty() const { return container.empty(); }
		size_type size() const { return container.size(); }
		reference front() { return container.front(); }
		const_reference front() const { return container.front(); }

		reference back() { return container.back(); }
		const_reference back() const { return container.back(); }

		void push(const value_type &val) { container.push_back(val); }
		void pop() { container.pop_front(); }
		void swap(queue &other) { container.swap(other.container); }

	public:
		template <typename T,typename Container>
		friend bool operator==(const queue<T, Container> &lhs, const queue<T, Container> &rhs);
		template <typename T,typename Container>
		friend bool operator!=(const queue<T, Container> &lhs, const queue<T, Container> &rhs);
		
		template <typename T,typename Container>
		friend void swap(queue<T, Container> &x, queue<T, Container> &y);
	};

	template <typename T,typename Container>
	bool operator==(const queue<T, Container> &lhs, const queue<T, Container> &rhs)
	{
		return lhs.container == rhs.container;
	}

	template <typename T,typename Container>
	bool operator!=(const queue<T, Container> &lhs, const queue<T, Container> &rhs)
	{
		return !(lhs == rhs);
	}

	template <typename T,typename Container>
	void swap(queue<T, Container> &x, queue<T, Container> &y)
	{
		miniSTL::swap(x.container, y.container);
	}
}

#endif
