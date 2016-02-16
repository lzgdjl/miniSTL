#ifndef _RB_TREE_H
#define _RB_TREE_H

#include <utility>

#include "mini_algorithm.h"
#include "mini_allocator.h"
#include "mini_functional.h"
#include "mini_uninitialized_functions.h"
#include "mini_type_traits.h"
#include "mini_iterator.h"
#include "mini_reverse_iterator.h"
/*
* 本文件实现红黑树
* 插入与删除算法采用CLRS(算法导论)上面的算法。有两点例外
*  1：header 为了实现_begin() 与 _end() 的常数时间操作
*  2：当一个有两个子节点的节点被删除，被删除节点的下一个节点重新
*     连接上去，而不是复制。这样唯一失效的迭代器是被删除节点。（CLRS 第三版中的算法）
*/

namespace
{
	enum Rb_tree_color {red, black};

	struct Rb_tree_node_base
	{
		typedef Rb_tree_node_base*			Base_ptr;
		typedef const Rb_tree_node_base*	Const_Base_ptr;

		Rb_tree_color color;
		Base_ptr parent;
		Base_ptr left;
		Base_ptr right;

		static Base_ptr minimum(Base_ptr x)
		{
			while (x->left != nullptr)
				x = x->left;
			return x;
		}

		static Const_Base_ptr minimum(Const_Base_ptr x)
		{
			while (x->left != nullptr)
				x = x->left;
			return x;
		}

		static Base_ptr maximum(Base_ptr x)
		{
			while (x->right != nullptr)
				x = x->right;
			return x;
		}

		static Const_Base_ptr maximum(Const_Base_ptr x)
		{
			while (x->right != nullptr)
				x = x->right;
			return x;
		}
	};

	template <typename Val>
	struct Rb_tree_node : public Rb_tree_node_base
	{
		typedef Rb_tree_node<Val>* Link_type;
		Val value_field;

		template <typename... Args>
		Rb_tree_node(Args&&... args)
			:Rb_tree_node_base(),
			value_field(std::forward<Args>(args)...) {}
	};

	Rb_tree_node_base *Rb_tree_increment(Rb_tree_node_base *x) noexcept();
	const Rb_tree_node_base* Rb_tree_increment(const Rb_tree_node_base *x) noexcept();

	Rb_tree_node_base *Rb_tree_decrement(Rb_tree_node_base *x) noexcept();
	const Rb_tree_node_base *Rb_tree_decrement(const Rb_tree_node_base *x) noexcept();

	template <typename T>
	struct Rb_tree_iterator
	{
		typedef T		value_type;
		typedef T&		reference;
		typedef T*		pointer;

		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t					difference_type;

		typedef Rb_tree_iterator<T>			Self;
		typedef Rb_tree_node_base::Base_ptr Base_ptr;
		typedef Rb_tree_node<T>*			Link_type;

		Rb_tree_iterator()
			:node() {}

		explicit Rb_tree_iterator(Link_type x)
			:node(x) {}

		reference operator*() const
		{
			return static_cast<Link_type>(node)->value_field;
		}
		pointer operator->() const
		{
			return &(operator*());
		}

		Self &operator++()
		{
			node = Rb_tree_increment(node);
			return *this;
		}

		Self operator++(int)
		{
			Self tmp = *this;
			node = Rb_tree_increment(node);
			return tmp;
		}

		Self &operator--()
		{
			node = Rb_tree_decrement(node);
			return *this;
		}

		Self operator--(int)
		{
			Self tmp = *this;
			node = Rb_tree_decrement(node);
			return tmp;
		}

		bool operator==(const Self &x) const
		{
			return node == x.node;
		}

		bool operator!=(const Self &x) const
		{
			return node != x.node;
		}

		Base_ptr node;
	};

	template <typename T>
	struct Rb_tree_const_iterator
	{
		typedef T			value_type;
		typedef const T&	reference;
		typedef const T*	pointer;

		typedef Rb_tree_iterator<T> iterator;

		typedef bidirectional_iterator_tag	iterator_category;
		typedef ptrdff_t					difference_type;

		typedef Rb_tree_const_iterator<T>			Self;
		typedef Rb_tree_node_base::Const_Base_ptr	Base_ptr;
		typedef const Rb_tree_node<T>*				Link_type;

		Rb_tree_const_iterator()
			:node() {}

		explicit Rb_tree_const_iterator(Link_type x)
			:node(x) {}

		Rb_tree_const_iterator(const iterator &it)
			:node(it.node) {}

		iterator _const_cast() const
		{
			return iterator(static_cast<typename iterator::Link_type>
				(const_cast<typename iterator::Base_ptr>(node)));
		}

		reference operator*() const
		{
			return static_cast<Link_type>(node)->value_field;
		}

		pointer operator->() const
		{
			return &(operator*());
		}

		Self &operator++()
		{
			node = Rb_tree_increment(node);
			return *this;
		}

		Self operator++(int)
		{
			Self tmp = *this;
			node = Rb_tree_increment(node);
			return tmp;
		}

		Self &operator--()
		{
			node = Rb_tree_decrement(node);
			return *this;
		}

		Self operator--(int)
		{
			Self tmp = *this;
			node = Rb_tree_decrement(node);
			return tmp;
		}

		bool operator==(const Self &x) const
		{
			return node == x.node;
		}

		bool operator!=(const Self &x) const
		{
			return node != x.node;
		}

		Base_ptr node;
	};

	template <typename Val>
	inline bool operator==(
		const Rb_tree_iterator<Val> &x,
		const Rb_tree_const_iterator<Val> &y)
	{
		return x.node == y.node;
	}

	template <typename Val>
	inline bool operator!=(
		const Rb_tree_iterator<Val> &x,
		const Rb_tree_const_iterator<Val> &y)
	{
		return x.node != y.node;
	}

	void Rb_tree_insert_and_rebalance(const bool _insert_left,
		Rb_tree_node_base *x,
		Rb_tree_node_base *p,
		Rb_tree_node_base &header) noexcept;

	Rb_tree_node_base*
		Rb_tree_rebalance_for_erase(Rb_tree_node_base* const z,
			Rb_tree_node_base &header) noexcept;
}

namespace miniSTL
{
	template <typename Key,typename Val,typename KeyOfValue,
		typename Compare, typename Alloc = allocator < Val >>
	class Rb_tree final
	{
	private:
		typedef allocator<Rb_tree_node<Val>> Node_allocator;

		typedef Rb_tree_node_base*			Base_ptr;
		typedef const Rb_tree_node_base*	Const_Base_ptr;

	public:
		typedef Key							key_type;
		typedef Val							value_type;
		typedef value_type*					pointer;
		typedef const value_type*			const_pointer;
		typedef value_type&					reference;
		typedef const value_type&			const_reference;
		typedef Rb_tree_node<Val>*			Link_type;
		typedef const Rb_tree_node<Val>*	Const_Link_type;
		typedef size_t						size_type;
		typedef ptrdiff_t					difference_type;
		typedef Alloc						allocator_type;

		Node_allocator &get_node_allocator() noexcept
		{
			return *static_cast<Node_allocator*>(&this->impl);
		}

		const Node_allocator &get_node_allocator() const noexcept
		{
			return *static_cast<const Node_allocator*>(&this->impl);
		}

		allocator_type get_allocator() const noexcept
		{
			return allocator_type(get_node_allocator());
		}

	private:
		Link_type get_node()
		{
			return impl.Node_allocator::allocate(1);
		}

		void put_node(Link_type p)
		{
			impl.Node_allocator::deallocate(p, 1);
		}

		Link_type create_node(const value_type &x)
		{
			Link_type tmp = get_node();
			try
			{
				globals::construct(&(tmp->value_field), x);
			}
			catch (...)
			{
				put_node(tmp);
				throw;
			}
			return tmp;
		}

		void destroy_node(Link_type p)
		{
			globals::destroy(&(p->value_field));
			put_node(p);
		}

		Link_type clone_node(Const_Link_type x)
		{
			Link_type tmp = create_node(x->value_field);
			tmp->color = x->color;
			tmp->left = nullptr;
			tmp->right = nullptr;
			return tmp;
		}

	private:
		template <typename Key_compare>
		struct Rb_tree_impl :public Node_allocator
		{
			Key_compare key_compare;
			Rb_tree_node_base header;
			size_type node_count;

			Rb_tree_impl()
				:Node_allocator(),key_compare(),header(),
				node_count(0)
			{
				initialize();
			}

			Rb_tree_impl(const Key_compare &comp, const Node_allocator &a)
				: Node_allocator(a), key_compare(comp), header(),
				node_count(0)
			{
				initialize();
			}

		private:
			void initialize()
			{
				this->header.color = Rb_tree_color::red;
				this->header.parent = nullptr;
				this->header.left = &this->header;
				this->header.right = &this->header;
			}
		};

		Rb_tree_impl<Compare> impl;

		private:
			Base_ptr &root()
			{
				return this->impl.header.parent;
			}

			Const_Base_ptr root() const
			{
				return this->impl.header.parent;
			}

			Base_ptr &leftmost()
			{
				return this->impl.header.left;
			}

			Const_Base_ptr leftmost() const
			{
				return this->impl.header.left;
			}

			Base_ptr &rightmost()
			{
				this->impl.header.right;
			}

			Const_Base_ptr right_most() const
			{
				return this->impl.header.right;
			}

			Link_type _begin()
			{
				return static_cast<Link_type>(this->impl.header.parent);
			}

			Const_Link_type _begin() const
			{
				return static_cast<Const_Link_type>(this->impl.header.parent);
			}

			Link_type _end()
			{
				return static_cast<Link_type>(&this->impl.header);
			}

			Const_Link_type _end() const
			{
				return static_cast<Const_Link_type>(&this->impl.header);
			}

			static const_reference value(Const_Link_type x)
			{
				return x->value_field;
			}

			static const Key& key(Const_Link_type x)
			{
				return KeyOfValue()(value(x));
			}

			static Link_type left(Base_ptr x)
			{
				return static_cast<Link_type>(x->left);
			}

			static Const_Link_type left(Const_Base_ptr x)
			{
				return static_cast<Const_Link_type>(x->left);
			}

			static Link_type right(Base_ptr x)
			{
				return static_cast<Link_type>(x->right);
			}

			static Const_Link_type right(Const_Base_ptr x)
			{
				return static_cast<Const_Link_type>(x->right);
			}

			static const_reference value(Const_Base_ptr x)
			{
				return static_cast<Const_Link_type>(x)->value_field;
			}

			static const Key& key(Const_Base_ptr x)
			{
				return KeyOfValue()(value(x));
			}

			static Base_ptr minimum(Base_ptr x)
			{
				return Rb_tree_node_base::minimum(x);
			}

			static Const_Base_ptr minimum(Const_Base_ptr x)
			{
				return Rb_tree_node_base::minimum(x);
			}

			static Base_ptr maximum(Base_ptr x)
			{
				return Rb_tree_node_base::maximum(x);
			}

			static Const_Base_ptr maximum(Const_Base_ptr x)
			{
				return Rb_tree_node_base::maximum(x);
			}

		public:
			typedef Rb_tree_iterator<value_type>		iterator;
			typedef Rb_tree_const_iterator<value_type>	const_iterator;

			typedef reverse_iterator_t<iterator>		reverse_iterator;
			typedef reverse_iterator_t<const_iterator>	const_reverse_iterator;

		private:
			template <typename Arg>
			iterator _insert(Const_Base_ptr x, Const_Base_ptr y, Arg&& v);

			template <typename Arg>
			iterator _insert_lower(Base_ptr x, Base_ptr y, Arg&& v);

			template <typename Arg>
			iterator _insert_equal_lower(Arg&& x);

			Link_type _copy(Const_Link_type x, Link_type p);

			void _erase(Link_type x);

			iterator _lower_bound(Link_type x, Link_type y,
				const Key &k);
			const_iterator _lower_bound(Const_Link_type x, Const_Link_type y,
				const Key &k) const;

			iterator _upper_bound(Link_type x, Link_type y,
				const Key &k);
			const_iterator _upper_bound(Const_Link_type x, Const_Link_type y,
				const Key &k) const;

		public:
			Rb_tree() {}

			Rb_tree(const Compare &comp,const allocator_type &a = allocator_type())
				:impl(comp,Node_allocator(a)) {}
			Rb_tree(const Rb_tree &x)
				:impl(x.impl.key_compare, x.get_node_allocator())
			{
				if (x.root() != nullptr)
				{
					root() = _copy(x._begin(), _end());
					leftmost() = minimum(root());
					rightmost() = maximum(root());
					impl.node_count = x.impl.node_count;
				}
			}
			~Rb_tree() { _erase(_begin()); }

			Rb_tree &operator=(const Rb_tree &x);
			Compare key_comp() const
			{
				return impl.key_compare;
			}

			iterator begin()
			{
				return iterator(static_cast<Link_type>(this->impl.header.left));
			}
			const_iterator begin() const
			{
				return const_iterator(static_cast<Const_Link_type>
					(this->impl.header.left));
			}

			iterator end()
			{
				return iterator(static_cast<Link_type>(&this->impl.header));
			}

			const_iterator end() const
			{
				return const_iterator(static_cast<Const_Link_type>
					(&this->impl.header));
			}

			reverse_iterator rbegin()
			{
				return reverse_iterator(_end());
			}

			const_reverse_iterator rbegin() const
			{
				return const_reverse_iterator(_end());
			}

			reverse_iterator rend()
			{
				return reverse_iterator(begin());
			}

			const_reverse_iterator r_end() const
			{
				return const_reverse_iterator(begin());
			}

			bool empty() const
			{
				return impl.node_count == 0;
			}

			size_type size() const
			{
				return impl.node_count;
			}

			size_type max_size() const
			{
				return get_node_allocator.max_size();
			}

			void swap(Rb_tree &t);

			template <typename Arg>
			pair<iterator, bool>
				insert_unique(Arg&& x);

			template <typename Arg>
			iterator insert_equal(Arg &&x);

			template <typename Arg>
			iterator insert_unique(const_iterator position, Arg &&x);

			template <typename Arg>
			iterator insert_equal(const_iterator position, Arg &&x);

			template <typename InputIterator>
			void insert_unique(InputIterator first, InputIterator last);

			template <typename InputIterator>
			void insert_equal(InputIterator first, InputIterator last);

		private:
			void erase_aux(const_iterator position);
			void erase_aux(const_iterator first, const_iterator last);

		public:
			iterator erase(const_iterator position)
			{
				const_iterator result = position;
				++result;
				erase_aux(position);
				return result._const_cast();
			}

			iterator erase(iterator position)
			{
				iterator result = position;
				++result;
				erase_aux(position);
				return result;
			}

			size_type erase(const key_type &x);
			iterator erase(const_iterator first, const_iterator last)
			{
				erase_aux(first, last);
				return last._const_cast();
			}

			void erase(const key_type *first, const key_type *last);

			void clear() noexcept
			{
				_erase(_begin());
				leftmost() = _end();
				root() = nullptr;
				rightmost = _end();
				impl.node_count = 0;
			}

			iterator find(const key_type &k);
			const_iterator find(const key_type &k) const;

			size_type count(const key_type &k) const;

			iterator lower_bound(const key_type &k)
			{
				return _lower_bound(_begin(), _end(), k);
			}

			const_iterator lower_bound(const key_type &k) const
			{
				return _lower_bound(_begin(), _end(), k);
			}

			iterator upper_bound(const key_type &k)
			{
				return _upper_bound(_begin(), _end(), k);
			}

			const_iterator upper_bound(const key_type &k) const
			{
				return _upper_bound(_begin(), _end(), k);
			}

			pair<iterator, iterator>
				equal_range(const key_type &k);

			pair<const_iterator, const_iterator>
				equal_range(const key_type &k) const;

	};

	template <typename Key,typename Val,typename KeyOfValue,
		typename Compare,typename Alloc>
		inline bool
		operator==(const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &x,
			const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &y)
	{
		return x.size() == y.size() &&
			miniSTL::equal(x.begin, x.end(), y.begin());
	}

	template <typename Key,typename Val,typename KeyOfValue,
		typename Compare,typename Alloc>
		inline bool
		operator<(const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &x,
			const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &y)
	{
		return std::lexicographical_compare(x.begin(), x.end(),
			y.begin(), y.end());
	}

	template <typename Key,typename Val,typename KeyOfValue,
		typename Compare,typename Alloc>
		inline bool
		operator!=(const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &x,
			const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &y)
	{
		return !(x == y);
	}

	template <typename Key,typename Val,typename KeyOfValue,
		typename Compare,typename Alloc>
		inline bool
		operator>(const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &x,
			const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &y)
	{
		return y < x;
	}

	template <typename Key,typename Val,typename KeyOfValue,
		typename Compare,typename Alloc>
		inline bool operator<=(const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &x,
			const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &y)
	{
		!(y < x);
	}

	template <typename Key,typename Val,typename KeyOfValue,
		typename Compare,typename Alloc>
		inline bool
		operator>=(const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &x,
			const Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &y)
	{
		return !(x < y);
	}

	template <typename Key,typename Val,typename KeyOfValue,
		typename Compare,typename Alloc>
		inline void
		swap(Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &x,
			Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &y)
	{
		x.swap(y);
	}

	template <typename Key, typename Val,typename KeyOfValue,
		typename Compare,typename Alloc>
		Rb_tree<Key,Val,KeyOfValue,Compare,Alloc>::
		Rb_tree(Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &&x)
		:impl(x.impl.key_compare,
			std::move(x.get_node_allocator()))
	{
		if (x.root() != nullptr)
		{
			root() = x.root();
			leftmost() = x.leftmost();
			rightmost() = x.rightmost();
			root()->parent = _end();

			x.root() = nullptr;
			x.leftmost() = x._end();
			x.rightmost() = x._end();

			this->impl.node_count = x.impl.node_count;
			x.impl.node_count = 0;
		}
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>&
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		operator=(const Rb_tree<_Key, Val, KeyOfValue, Compare, Alloc>& x)
	{
		if (this != &x)
		{
			// Note that _Key may be a constant type.
			clear();
			impl.key_compare = x.impl.key_compare;
			if (x.root() != nullptr)
			{
				root() = _copy(x._begin(), _end());
				leftmost() = minimum(root());
				rightmost() = maximum(root());
				impl.node_count =x.impl.node_count;
			}
		}
		return *this;
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		template<typename Arg>
	typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_insert(Const_Base_ptr x, Const_Base_ptr _p, Arg&& v)
	{
		bool insert_left = (x != 0 || p == _end()
			|| impl.key_compare(KeyOfValue()(v),
				key(p)));

		Link_type z = create_node(std::forward<Arg>(v));

		Rb_tree_insert_and_rebalance(insert_left, z,
			const_cast<_Base_ptr>(p),
			this->impl.header);
		++impl.node_count;
		return iterator(z);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		template<typename Arg>
	typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_insert_lower(Base_ptr x, Base_ptr p, Arg&& v)
	{
		bool insert_left = (x != 0 || p == _end()
			|| !impl.key_compare(key(p),
				KeyOfValue()(v)));

		Link_type z = create_node(std::forward<arg>(v);

		Rb_tree_insert_and_rebalance(insert_left, z, p,
			this->impl.header);
		++impl.M_node_count;
		return iterator(z);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		template<typename Arg>
	typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
		_Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_insert_equal_lower(Arg&& v)
	{
		Link_type x = _begin();
		Link_type y = _end();
		while (x != 0)
		{
			y = x;
			x = !impl.key_compare(key(x), KeyOfValue()(v)) ?
				left(x) : right(x);
		}
		return _insert_lower(x, y, std::forward<arg>(v));
	}

	template<typename Key, typename Val, typename KoV,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KoV, Compare, Alloc>::Link_type
		Rb_tree<Key, Val, KoV, Compare, Alloc>::
		_copy(Const_Link_type x, Link_type p)
	{
		Link_type top = clone_node(x);
		top->_parent = p;

		try
		{
			if (x->_right)
				top->right = _copy(right(x), top);
			p = top;
			x = left(x);

			while (x != 0)
			{
				Link_type y = clone_node(x);
				p->left = y;
				y->parent = p;
				if (x->right)
					y->right = _copy(right(x), y);
				p = y;
				x = left(x);
			}
		}
		catch(...)
		{
			_erase(top);
			throw;
		}
		return top;
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		void Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_erase(Link_type x)
	{
		// Erase without rebalancing.
		while (x != 0)
		{
			_erase(right(x));
			Link_type y = left(x);
			destroy_node(x);
			x = y;
		}
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_lower_bound(Link_type x, Link_type y,
			const Key& k)
	{
		while (x != 0)
			if (!impl.key_compare(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);
		return iterator(y);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::const_iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_lower_bound(Const_Link_type x, Const_Link_type y,
			const Key& k) const
	{
		while (x != 0)
			if (!impl.key_compare(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);
		return const_iterator(y);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_upper_bound(Link_type x, Link_type y,
			const Key& k)
	{
		while (x != 0)
			if (impl.key_compare(k, key(x)))
				y = x, x = left(x);
			else
				x = right(x);
		return iterator(y);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::const_iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_upper_bound(Const_Link_type x,Const_Link_type y,
			const Key& k) const
	{
		while (x != 0)
			if (impl.key_compare(k, key(x)))
				y = x, x = left(x);
			else
				x = right(x);
		return const_iterator(y);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		pair<typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::iterator,
		typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::iterator>
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		equal_range(const Key &k)
	{
		Link_type x = _begin();
		Link_type y = _end();
		while (x != 0)
		{
			if (impl.key_compare(key(x), k))
				x = right(x);
			else if (impl.key_compare(k, key(x)))
				y = x, x = left(x);
			else
			{
				Link_type xu(x), yu(y);
				y = x, x = left(x);
				xu = right(xu);
				return pair<iterator,
					iterator>(_lower_bound(x, y, k),
						_upper_bound(xu, yu, k));
			}
		}
		return pair<iterator, iterator>(iterator(y),
			iterator(y));
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		pair<typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::const_iterator,
		typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::const_iterator>
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		equal_range(const Key &k) const
	{
		Const_Link_type x = _begin();
		Const_Link_type y = _end();
		while (x != 0)
		{
			if (impl.key_compare(key(x), k))
				x = right(x);
			else if (impl.key_compare(k, key(x)))
				y = x, x = left(x);
			else
			{
				Const_Link_type xu(x), yu(y);
				y = x, x = left(x);
				xu = right(xu);
				return pair<const_iterator,
					const_iterator>(_lower_bound(x, y, k),
						_upper_bound(xu, yu, k));
			}
		}
		return pair<const_iterator, const_iterator>(const_iterator(y),
			const_iterator(y));
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		void Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		swap(Rb_tree<Key, Val, KeyOfValue, Compare, Alloc> &t)
	{
		if (root() == 0)
		{
			if (t.root() != 0)
			{
				root() = t.root();
				leftmost() = t.leftmost();
				rightmost() = t.rightmost();
				root()->parent = _end();

				t.root() = 0;
				t.leftmost() = t._end();
				t.rightmost() = t._end();
			}
		}
		else if (t.root() == 0)
		{
			t.root() = root();
			t.leftmost() = leftmost();
			t.rightmost() = rightmost();
			t.root()->parent = t._end();

			root() = 0;
			leftmost() = _end();
			rightmost() = _end();
		}
		else
		{
			miniSTL::swap(root(), t.root());
			miniSTL::swap(leftmost(), t.leftmost());
			miniSTL::swap(rightmost(), t.rightmost());

			root()->parent = _end();
			t.root()->parent = t._end();
		}
		// No need to swap header's color as it does not change.
		miniSTL::swap(this->impl.node_count, t.impl.node_count);
		miniSTL::swap(this->impl.key_compare, t.impl.key_compare);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		template<typename Arg>
	pair<typename Rb_tree<Key, Val, KeyOfValue,
		Compare,_Alloc>::iterator, bool>
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		insert_unique(Arg &&v)
	{
		Link_type x = _begin();
		Link_type y = _end();
		bool comp = true;
		while (x != 0)
		{
			y = x;
			comp = impl.key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		}
		iterator j = iterator(y);
		if (comp)
		{
			if (j == begin())
				return pair<iterator, bool>
				(_insert(x, y, std::forward<Arg>(v), true);
			else
				--j;
		}
		if (impl.key_compare(key(j.node), KeyOfValue()(v)))
			return pair<iterator, bool>
			(_insert(x, y,std::forward<Arg>(v), true);
		return pair<iterator, bool>(j, false);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		template<typename Arg>
	typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
		_Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		insert_equal(Arg &&v)
	{
		Link_type x = _begin();
		Link_type y = _end();
		while (x != 0)
		{
			y = x;
			x = impl.key_compare(KeyOfValue()(v), key(x)) ?
				left(x) : right(x);
		}
		return _insert(x, y, std::forward<Arg>(v));
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		template<typename Arg>
	typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		insert_unique(const_iterator position, Arg &&v)
	{
		// end()
		if (position.node == _end())
		{
			if (size() > 0
				&& impl.key_compare(key(rightmost()),
					_KeyOfValue()(v)))
				return _insert(0, rightmost(), std::forward<Arg>(v));
			else
				return insert_unique(std::forward<Arg>(v).first;
		}
		else if (impl.key_compare(KeyOfValue()(v),
			key(position.node)))
		{
			// First, try before...
			const_iterator before = position;
			if (position.node == leftmost()) // begin()
				return _insert(leftmost(), leftmost(),
					std::forward<Arg>(v);
			else if (impl.key_compare(key((--before).node),
				KeyOfValue()(v)))
			{
				if (right(before.node) == 0)
					return _insert(0, before.node,std::forward<Arg>(v));
				else
					return _insert(position.node, position.node, std::forward<Arg>(v));
			}
			else
				return insert_unique(std::forward<Arg>(v)).first;
		}
		else if (impl.key_compare(key(position.node),
			KeyOfValue()(v)))
		{
			// ... then try after.
			const_iterator after = position;
			if (position.node == rightmost())
				return _insert(0, rightmost(), std::forward<Arg>(v));
			else if (impl.key_compare(KeyOfValue()(v),
				key((++after).node)))
			{
				if (right(position.node) == 0)
					return insert_(0, position.node, std::forward<Arg>(v));
				else
					return _insert(after.node, after.node, std::forward<Arg>(v);
			}
			else
				return insert_unique(std::forward<Arg>(v)).first;
		}
		else
			// Equivalent keys.
			return position._const_cast();
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		template<typename Arg>
	typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		insert_equal(const_iterator position, Arg &&v)
	{
		// end()
		if (position.node == _end())
		{
			if (size() > 0
				&& !impl.key_compare(KeyOfValue()(v),
					key(rightmost())))
				return _insert(0, rightmost(), std::forward<Arg>(v);
			else
				return insert_equal(std::forward<Arg>(v));
		}
		else if (!impl.key_compare(key(position.node),
			KeyOfValue()(v)))
		{
			// First, try before...
			const_iterator before = position;
			if (position.node == leftmost()) // begin()
				return _insert(leftmost(), leftmost(), std::forward<Arg>(v));
			else if (!impl.key_compare(KeyOfValue()(v), key((--before).node)))
			{
				if right(before.node) == 0)
					return _insert(0, before._node, std::forward<Arg>(v));
				else
					return _insert(position.node, position.node, std::forward<Arg>(v));
			}
			else
				return insert_equal(std::forward<Arg>(v));
		}
		else
		{
			// ... then try after.  
			const_iterator after = position;
			if (position.node == rightmost())
				return _insert(0, rightmost(), std::forward<Arg>(v));
			else if (!impl.key_compare(key((++after).node), KeyOfValue()(v)))
			{
				if (right(position.node) == 0)
					return _insert(0, position.node, std::forward<Arg>(v));
				else
					return _insert(after.node, after.node, std::forward<Arg>(v));
			}
			else
				return _insert_equal_lower(std::forward<Arg>(v));
		}
	}

	template<typename Key, typename Val, typename KoV,
		typename Cmp, typename Alloc>
		template<class II>
	void Rb_tree<Key, Val, KoV, Cmp, Alloc>::
		insert_unique(II first, II last)
	{
		for (; first != last; ++first)
			insert_unique(end(), *first);
	}

	template<typename Key, typename Val, typename KoV,
		typename Cmp, typename Alloc>
		template<class II>
	void Rb_tree<Key, Val, KoV, Cmp, Alloc>::
		insert_equal(II first, II last)
	{
		for (; first != last; ++first)
			insert_equal(end(), *first);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		void Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		erase_aux(const_iterator position)
	{
		Link_type y =
			static_cast<Link_type>(Rb_tree_rebalance_for_erase(const_cast<Base_ptr>(position.node),
					this->impl.header));
		destroy_node(y);
		--impl.node_count;
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		void Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		erase_aux(const_iterator first, const_iterator last)
	{
		if (first == begin() && last == end())
			clear();
		else
			while (first != last)
				erase(first++);
	}
	
	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::size_type
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		erase(const Key& x)
	{
		pair<iterator, iterator> p = equal_range(x);
		const size_type old_size = size();
		erase(p.first, p.second);
		return old_size - size();
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		void Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		erase(const Key* first, const Key* last)
	{
		while (first != last)
			erase(*first++);
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		find(const Key& k)
	{
		iterator j = _lower_bound(_begin(), _end(), k);
		return (j == end()
			|| impl.key_compare(k, key(j.node))) ? end() : j;
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KeyOfValue,
		Compare, Alloc>::const_iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		find(const Key& k) const
	{
		const_iterator j = _lower_bound(_begin(), _end(), k);
		return (j == end()
			|| impl.key_compare(k,
				key(j.node))) ? end() : j;
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::size_type
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		count(const Key& k) const
	{
		pair<const_iterator, const_iterator> p = equal_range(k);
		const size_type n = miniSTL::distance(p.first, p.second);
		return n;
	}

	template<typename Key, typename Val, typename KeyOfValue,
		typename Compare, typename Alloc>
		template<typename Arg>
	typename Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
		Rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::
		_insert_equal_lower(Arg&& v)
	{
		Link_type x = _begin();
		Link_type y = _end();
		while (x != 0)
		{
			y = x;
			x = !impl.key_compare(key(x), KeyOfValue()(v)) ?
				left(x) : right(x);
		}
		return _insert_lower(x, y, std::forward<Arg>(v));
	}

}

namespace
{
	Rb_tree_node_base*
		Rb_tree_increment(Rb_tree_node_base *x) noexcept
	{
		if (x->right != 0) 
		{
			x = x->right;
			while (x->left != 0)
				x = x->left;
		}
		else 
		{
			auto y = x->parent;
			while (x == y->right)
			{
				x = y;
				y = y->parent;
			}
			if (x->right != y)
				x = y;
		}
		return x;
	}

	const Rb_tree_node_base*
		Rb_tree_increment(const Rb_tree_node_base *x) noexcept
	{
		if (x->right != 0)
		{
			x = x->right;
			while (x->left != 0)
				x = x->left;
		}
		else 
		{
			auto y = x->parent;
			while (x == y->right)
			{
				x = y;
				y = y->parent;
			}
			if (x->right != y)
				x = y;
		}
		return x;
	}

	Rb_tree_node_base*
		Rb_tree_decrement(Rb_tree_node_base* x) noexcept
	{
		if (x->color == Rb_tree_color::red &&
			x->parent->parent == x)
			x = x->right;
		else if (x->left != 0) {
			auto y = x->left;
			while (y->right != 0)
				y = y->right;
			x = y;
		}
		else {
			auto y = x->parent;
			while (x == y->left) {
				x = y;
				y = y->parent;
			}
			x = y;
		}
		return x;
	}

	const Rb_tree_node_base*
		Rb_tree_decrement(const Rb_tree_node_base *x) noexcept
	{
		if (x->color == Rb_tree_color::red &&
			x->parent->parent == x)
			x = x->right;
		else if (x->left != 0) {
			auto y = x->left;
			while (y->right != 0)
				y = y->right;
			x = y;
		}
		else {
			auto y = x->parent;
			while (x == y->left) {
				x = y;
				y = y->parent;
			}
			x = y;
		}
		return x;
	}

	void Rb_tree_insert_and_rebalance(const bool insert_left,
		Rb_tree_node_base *x,
		Rb_tree_node_base *p,
		Rb_tree_node_base &header) noexcept
	{
		Rb_tree_node_base *& root = header.parent;

		// Initialize fields in new node to insert.
		x->parent = p;
		x->left = 0;
		x->right = 0;
		x->color = Rb_tree_color::red;

		// Insert.
		// Make new node child of parent and maintain root, leftmost and
		// rightmost nodes.
		// N.B. First node is always inserted left.
		if (insert_left)
		{
			p->left = x; // also makes leftmost = x when __p == &__header

			if (p == &header)
			{
				header.parent = x;
				header.right = x;
			}
			else if (p == header.left)
				header.left = x; // maintain leftmost pointing to min node
		}
		else
		{
			p->right = x;

			if (p == header.right)
				header.right = x; // maintain rightmost pointing to max node
		}
		// Rebalance.
		while (x != root
			&& x->parent->color == Rb_tree_color::red)
		{
			Rb_tree_node_base* const xpp = x->parent->parent;

			if (x->parent == xpp->left)
			{
				Rb_tree_node_base* const y = xpp->right;
				if (y && y->color == Rb_tree_color::red)
				{
					x->parent->color = Rb_tree_color::black;
					y->color = Rb_tree_color::black;
					xpp->color = Rb_tree_color::red;
					x = xpp;
				}
				else
				{
					if (x == x->parent->right)
					{
						x = x->parent;
						Rb_tree_rotate_left(x, root);
					}
					x->parent->color = Rb_tree_color::black;
					xpp->color = Rb_tree_color::red;
					Rb_tree_rotate_right(xpp, root);
				}
			}
			else
			{
				Rb_tree_node_base* const y = xpp->left;
				if (y && y->color == Rb_tree_color::red)
				{
					x->parent->color = Rb_tree_color::black;
					y->color = Rb_tree_color::black;
					xpp->color = Rb_tree_color::red;
					x = xpp;
				}
				else
				{
					if (x == x->parent->left)
					{
						x = x->parent;
						Rb_tree_rotate_right(x, root);
					}
					x->parent->color = Rb_tree_color::black;
					xpp->color = Rb_tree_color::red;
					Rb_tree_rotate_left(xpp, root);
				}
			}
		}
		root->color = Rb_tree_color::black;
	}

	Rb_tree_node_base*
		Rb_tree_rebalance_for_erase(Rb_tree_node_base* const z,
			Rb_tree_node_base &header) noexcept
	{
		Rb_tree_node_base *& root = header.parent;
		Rb_tree_node_base *& leftmost = header.left;
		Rb_tree_node_base *& rightmost = header.right;
		Rb_tree_node_base* y = z;
		Rb_tree_node_base* x = 0;
		Rb_tree_node_base* x_parent = 0;

		if (y->left == 0)     // z has at most one non-null child. y == z.
			x = y->right;     // x might be null.
		else
			if (y->right == 0)  // z has exactly one non-null child. y == z.
				x = y->left;    // x is not null.
			else
			{
				// z has two non-null children.  Set y to
				y = y->right;   //   z's successor.  x might be null.
				while (y->left != 0)
					y = y->left;
				x = y->right;
			}
		if (y != z)
		{
			// relink y in place of z.  y is z's successor
			z->left->parent = y;
			y->left = z->left;
			if (y != z->right)
			{
				x_parent = y->parent;
				if (x) x->parent = y->parent;
				y->parent->left = x;   // y must be a child of left
				y->right = z->right;
				z->right->parent = y;
			}
			else
				x_parent = y;
			if (__root == z)
				__root = y;
			else if (z->parent->left == z)
				z->parent->left = y;
			else
				z->parent->right = y;
			y->parent = z->parent;
			std::swap(y->color, z->color);
			y = z;
			// y now points to node to be actually deleted
		}
		else
		{                        // y == z
			x_parent = y->parent;
			if (x)
				x->parent = y->parent;
			if (root == z)
				root = x;
			else
				if (z->parent->left == z)
					z->parent->left = x;
				else
					z->parent->right = x;
			if (leftmost == z)
			{
				if (z->right == 0)        // z->left must be null also
					leftmost = z->parent;
				// makes __leftmost == _M_header if z == __root
				else
					leftmost = Rb_tree_node_base::minimum(x);
			}
			if (rightmost == z)
			{
				if (z->left == 0)         // z->right must be null also
					rightmost = z->parent;
				// makes __rightmost == _M_header if z == __root
				else                      // x == z->left
					rightmost = Rb_tree_node_base::maximum(x);
			}
		}
		if (y->color != Rb_tree_color::red)
		{
			while (x != root && (x == 0 || x->color == Rb_tree_color::black))
				if (x == x_parent->left)
				{
					Rb_tree_node_base* w = x_parent->right;
					if (w->color == Rb_tree_color::red)
					{
						w->color = Rb_tree_color::black;
						x_parent->color = Rb_tree_color::red;
						Rb_tree_rotate_left(x_parent, root);
						w = x_parent->right;
					}
					if ((w->left == 0 ||
						w->left->color == Rb_tree_color::black) &&
						(w->right == 0 ||
							w->right->color == Rb_tree_color::black))
					{
						w->color = Rb_tree_color::red;
						x = x_parent;
						x_parent = x_parent->parent;
					}
					else
					{
						if (w->right == 0
							|| w->right->color == Rb_tree_color::black)
						{
							w->left->color = Rb_tree_color::black;
							w->color = Rb_tree_color::red;
							Rb_tree_rotate_right(w, root);
							w = x_parent->right;
						}
						w->color = x_parent->color;
						x_parent->color = Rb_tree_color::black;
						if (w->right)
							w->right->color = Rb_tree_color::black;
						Rb_tree_rotate_left(x_parent, root);
						break;
					}
				}
				else
				{
					// same as above, with right <-> left.
					Rb_tree_node_base* w = x_parent->left;
					if (w->color == Rb_tree_color::red)
					{
						w->color = Rb_tree_color::black;
						x_parent->color = Rb_tree_color::red;
						Rb_tree_rotate_right(x_parent, __root);
						w = x_parent->left;
					}
					if ((w->right == 0 ||
						w->right->color == Rb_tree_color::black) &&
						(w->left == 0 ||
							w->left->color == Rb_tree_color::black))
					{
						w->color = Rb_tree_color::red;
						x = x_parent;
						x_parent = x_parent->parent;
					}
					else
					{
						if (w->left == 0 || w->left->color == Rb_tree_color::black)
						{
							w->right->color = Rb_tree_color::black;
							w->color = Rb_tree_color::red;
							Rb_tree_rotate_left(w, root);
							w = x_parent->left;
						}
						w->color = x_parent->color;
						x_parent->color = Rb_tree_color::black;
						if (w->left)
							w->left->color = Rb_tree_color::black;
						Rb_tree_rotate_right(x_parent, root);
						break;
					}
				}
			if (x) x->color = Rb_tree_color::black;
		}
		return y;
	}
}


#endif