#ifndef _LIST_H
#define _LIST_H

#include "mini_algorithm.h"
#include "mini_iterator.h"
#include "mini_reverse_iterator.h"
#include "mini_uninitialized_functions.h"
#include "mini_type_traits.h"


namespace miniSTL
{
	namespace
	{
		// list �Ľڵ� (node) �ṹ,˫������
		template <typename T>
		struct _list_node final
		{
			typedef _list_node<T>* node_pointer;
			node_pointer prev;
			node_pointer next;
			T data;
		};

		//list �ĵ�����
		template <typename T, typename Ref, typename Ptr>
		struct _list_iterator
		{
			typedef _list_iterator<T, T&, T*>		iterator;
			typedef _list_iterator<T, Ref, Ptr>		self;

			typedef bidirectional_iterator_tag		iterator_category;
			typedef T								value_type;
			typedef Ptr								pointer;
			typedef Ref								reference;
			typedef _list_node<T>*					link_type;
			typedef size_t							size_type;
			typedef ptrdiff_t						difference_type;

			link_type node; // �������ڲ�����ָͨ��

			// ���캯��
			_list_iterator(link_type link):node(link) {}
			_list_iterator() {}
			_list_iterator(const iterator &other):node(other.node) {}

			bool operator==(const self &other) const { return node == other.node; }
			bool operator!=(const self &other) const { return node != other.node; }

			reference operator*() const { return (*node).data; }
			pointer operator->() const { return &(operator*()); }
			// �Ե�������1������ǰ��һ���ڵ�
			// ǰ׺
			self &operator++()
			{
				node = static_cast<link_type>((*node).next);
				return *this;
			}
			self &operator++(int)
			{
				self tmp = *this;
				++*this;
				return tmp;
			}

			self &operator--()
			{
				node = static_cast<link_type>((*node).prev);
				return *this;
			}

			self &operator--(int)
			{
				self tmp = *this;
				--*this;
				return tmp;
			}
		};
	}

	// list ��һ����״˫������
	template <typename T,typename Alloc = allocator<_list_node<T>>>
	class list final
	{
	public:
		typedef _list_node<T>*								link_type;
		typedef _list_iterator<T, T&, T*>					iterator;
		typedef T											value_type;
		typedef _list_iterator<const T, const T&, const T*>	const_iterator;
		typedef	reverse_iterator_t<iterator>				reverse_iterator;
		typedef	T&											reference;
		typedef size_t										size_type;

	private:
		typedef _list_node<T>	list_node;
		typedef Alloc			list_node_allocator;
		link_type				node;// ֻҪһ��ָ�룬��ɱ�ʾ��������˫������
	
	private:
		// ����һ���ڵ㲢����
		link_type get_node() { return list_node_allocator::allocate(); }

		// �ͷ�һ���ڵ�
		link_type put_node(link_type p) { list_node_allocator::deallocate(p); }

		// ����һ���ڵ㣬����Ԫ��ֵ
		link_type create_node(const T &value)
		{
			link_type p = get_node();
			globals::construct(&p->data, value);
			return p;
		}

		// ����һ���ڵ�
		void destroy_node(link_type p)
		{
			globals::destroy(&p->data);
			put_node(p);
		}

	public:
		// list �Ĺ������ڴ����
		list() { empty_initialize(); } // ����һ��������
		iterator begin() { return static_cast<link_type>((*node).next); }
		iterator end() { return node;}
		const_iterator cbegin() { return (*node).next; }
		const_iterator cend() { return node; }
		~list()
		{
			clear();
			globals::destroy(&(*node).data);
		}

		bool empty() const { return node->next == node; }
		size_type size() const
		{
			size_type result = 0;
			distance(begin(), end(), result);
			return result;
		}
		
		// ȡͷ�ڵ������
		reference front() { return *begin(); }
		// ȡβ�ڵ������
		reference back() { return *(--end()); }

		// �ڵ����� position ��ָλ�ò���һ���ڵ�
		iterator insert(iterator position, const T &value)
		{
			link_type tmp = create_node(value);
			// ����˫��ָ�룬ʹ tmp �����ȥ
			tmp->next = position.node;
			tmp->prev = position.node->prev;
			position.node->prev->next = tmp;
			position.node->prev = tmp;
			return tmp;
		}

	public:
		// list ��Ԫ�ز���
		void push_front(const T &value) { insert(begin(), value); }
		void push_back(const T &value) { insert(end(), value); }

		// �Ƴ������� position ��ָ�ڵ�
		iterator erase(iterator position)
		{
			link_type next_node = position.node->next;
			link_type prev_node = position.node->prev;
			prev_node->next = next_node;
			next_node->prev = prev_node;
			destroy_node(position.node);
			return static_cast<iterator>(next_node);
		}

		// �Ƴ�ͷ�ڵ�
		void pop_front() { erase(begin()); }

		// �Ƴ�β�ڵ�
		void pop_back()
		{
			iterator tmp = end();
			erase(--tmp);
		}

		void remove(const T &value);
		void unique();

		// �� x �Ӻ��� position ��ָλ��֮ǰ��x ���벻ͬ�� *this
		void splice(iterator position, list &x)
		{
			if (x.empty())
				transfer(position, x.begin(), x.end());
		}

		// �� i ��ָԪ�ؽӺ��� position ��ָλ��֮ǰ��position �� i ��ָ��ͬһ�� list
		void splice(iterator position, list&, iterator i)
		{
			iterator j = i;
			++j;
			if (position == i || position == j)
				return;
			transfer(position, i, j);
		}

		void splice(iterator position, list&, iterator first, iterator last)
		{
			if (first != last)
				transfer(position, first, last);
		}

		list &swap(list &other)
		{
			auto tmp = node;
			node = other.node;
			other.node = tmp;

			return *this;
		}

		void merge(list &other);
		void reverse();
		void sort();
		void clear();

	private:
		void empty_initialize()
		{
			_node = get_node();
			_node->next = _node;
			_node->prev = _node;
			// ����һ���ڵ�ռ䣬�� _node ָ����
			// ��_node ͷβ��ָ���Լ�������Ԫ��ֵ
		}

		// �� [first,last) �ڵ�Ԫ���ƶ��� position ֮ǰ
		void transfer(iterator position, iterator first, iterator last)
		{
			if (position != last)
			{
				(*((*last.node).prev)).next = position.node;
				(*((*first.node).prev)).next = last.node;
				(*((*position.node).prev)).next = first.node;
				link_type tmp = (*position.node).prev;
				(*position.node).prev = (*last.node).prev;
				(*last.node).prev = (*first.node).prev;
				(*first.node).prev = tmp;
			}
		}
	};

	template <typename T,typename Alloc>
	void list<T, Alloc>::clear()
	{
		link_type cur = node->next;
		while (cur != node)
		{
			link_type tmp = cur;
			cur = cur->next;
			destroy_node(tmp);
		}

		// �ָ� node ԭʼ״̬
		node->next = node;
		node->prev = node;
	}

	//����ֵΪ value ֮����Ԫ���Ƴ�
	template <typename T,typename Alloc>
	void list<T, Alloc>::remove(const T &value)
	{
		iterator first = begin();
		iterator last = end();
		while (first != last)
		{
			iterator next = first;
			++next;
			if (*first == value)
				erase(first);
			first = next;
		}
	}

	template <typename T,typename Alloc>
	void list<T, Alloc>::unique()
	{
		iterator first = begin();
		iterator last = end();
		// ������ֱ�ӷ���
		if (first == last)
			return;
		iterator next = first;
		while (++next != last)
		{
			// ����ÿһ���ڵ�
			if (*first == *next) // �������ͬ��Ԫ��
				erase(next);		// ɾ��
			else
				first = next;		//����ָ��
			next = first;			// �������η�Χ
		}
	}

	// merge() �� other �ϲ��� *this ���ϡ����� lists �����ݶ������Ⱦ�����������
	template <typename T,typename Alloc>
	void list<T, Alloc>::merge(list<T, Alloc> &other)
	{
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = other.begin();
		iterator last2 = other.end();

		// ǰ���ǣ����� lists ���Ѿ�������������
		while (first1 != last1 && first2 != last2)
		{
			if (*first2 < *first1)
			{
				iterator next = first2;
				transfer(first1, first2, ++next);
				first2 = next;
			}
			else
				++first1;
		}
		if (first2 != last2)
			transfer(last1, first2, last2);
	}

	template <typename T,typename Alloc>
	void list<T, Alloc>::reverse()
	{
		if (node->next == node || node->next->next == node)
			return;

		iterator first = begin();
		++first;
		while (first != end())
		{
			iterator old = first;
			++first;
			transfer(begin(), old, first);
		}
	}

	// quick sort
	template <typename T,typename Alloc>
	void list<T, Alloc>::sort()
	{
		if (node->next == node ||
			node->next->next == node)
			return;

		list<T, Alloc> carry;
		list<T, Alloc> counter[64];
		int fill = 0;
		while (!empty())
		{
			carry.splice(carry.begin(), *this, begin());
			int i = 0;
			while (i < fill && !counter[i].empty())
			{
				counter[i].merge(carry);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill)
				++fill;
		}

		for (int i = 1;i < fill;++i)
		{
			counter[i].merge(counter[i - 1]);
		}
		list<T,Alloc>::swap(counter[fill - 1]);
	}
}

#endif
