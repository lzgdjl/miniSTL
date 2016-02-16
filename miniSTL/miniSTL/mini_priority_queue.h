#ifndef _PRIORITY_QUEUE
#define _PRIORITY_QUEUE

#include "mini_functional.h"
#include "mini_vector.h"

namespace miniSTL
{
	template <typename T,typename Container = miniSTL::vector<T>,
		typename Compare = miniSTL::less<typename Container::value_type>>
	class priority_queue final
	{
	public:
		typedef T										value_type;
		typedef Container								container_type;
		typedef typename Container::reference			reference;
		typedef typename Container::const_reference		const_reference;
		typedef typename Container::size_type			size_type;

	private:
		container_type container;
		Compare compare;

	public:
		explicit priority_queue(const Compare &comp = Compare(),
			const Container &ctnr = Container())
			:container(ctnr),compare(comp) {}
		template <typename InputIterator>
		priority_queue(InputIterator first, InputIterator last,
			const Compare &comp = Compare(),
			const Container &ctnr = Container())
			: container(ctnr), compare(comp)
		{
			container.insert(container.end(), first, last);
			miniSTL::make_heap(container.begin(), container.end());
		}

		bool empty() const
		{
			return container.empty();
		}

		size_type size() const { return container.size(); }
		reference top() { return container.front(); }

		void push(const value_type &val)
		{
			container.push_back(val);
			miniSTL::push_heap(container.begin(), container.end(), compare);
		}

		void pop()
		{
			miniSTL::pop_heap(container.begin(), container.end(), compare);
			container.pop_back();
		}

		void swap(priority_queue &other)
		{
			miniSTL::swap(container, other.container);
			miniSTL::swap(compare, other.compare);
		}

	public:
		template <typename T,typename Container,typename Compare>
		friend void swap(priority_queue<T, Container, Compare> &x, priority_queue<T, Container, Compare> &y);
	};

	template <typename T,typename Container,typename Compare>
	void swap(priority_queue<T, Container, Compare> &x, priority_queue<T, Container, Compare> &y)
	{
		x.swap(y);
	}
}


#endif