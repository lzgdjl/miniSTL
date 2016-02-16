#ifndef _STACK_H
#define _STACK_H

#include "mini_vector.h"

namespace miniSTL
{
	template <typename T,typename Container = miniSTL::vector<T>>
	class stack final
	{
	public:
		typedef typename Container::value_type	value_type;
		typedef typename Container::reference	reference;
		typedef typename Container::size_type	size_type;
		typedef Container						container_type;

	private:
		container_type container;

	public:
		explicit stack(const container_type &ctnr = container_type()):container(ctnr) {}

		bool empty() const { return container.empty(); }
		size_type size() const { return container.size(); }
		value_type &top() { return container.back(); }
		const value_type &top() const { return container.back(); }
		void push(const value_type &val) { container.push_back(val); }
		void pop() { container.pop_back(); }
		void swap(stack &other) { miniSTL::swap(container, other.container); }
	public:
		template <typename T,typename Container>
		friend bool operator==(const stack<T, Container> &lhs, const stack<T, Container> &rhs);
		template <typename T,typename Container>
		friend bool operator!=(const stack<T, Container> &lhs, const stack<T, Container> &rhs);

		template <typename T,typename Container>
		friend void swap(stack<T, Container> &x, stack<T, Container> &y);
	};

	template <typename T,typename Container>
	bool operator==(const stack<T, Container> &lhs, const stack<T, Container> &rhs)
	{
		return lhs.container == rhs.container;
	}

	template <typename T,typename Container>
	bool operator(const stack<T, Container> &lhs, const stack<T, Container> &rhs)
	{
		return !(lhs == rhs);
	}

	template <typename T,typename Container>
	void swap(stack<T, Container> &x, stack<T, Container> &y)
	{
		x.swap(y);
	}
}

#endif
