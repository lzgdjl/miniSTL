#ifndef _SET_H
#define _SET_H

#include "rb_tree.h"

namespace miniSTL
{
	template <typename Key,typename Compare = miniSTL::less<Key>,
		typename Alloc = _alloc>
	class set final
	{
	public:
		typedef Key			key_type;
		typedef Key			value_type;
		typedef Compare		key_compare;
		typedef Compare		value_compare;

	private:
		typedef Rb_tree<key_type, value_type,
			identity<value_type>, key_compare, Alloc> rep_type;
		rep_type t;

	public:
		typedef typename rep_type::const_pointer pointer;
		typedef typename rep_type::const_pointer const_pointer;
		typedef typename rep_type::const_reference reference;
		typedef typename rep_type::const_reference const_reference;
		typedef typename rep_type::const_iterator iterator;
		typedef typename rep_type::const_iterator const_iterator;
		typedef typename rep_type::size_type size_type;
		typedef typename rep_type::difference_type difference_type;

		set():t(Compare()) {}
		explicit set(const Compare &comp):t(comp) {}

		template <typename InputIterator>
		set(InputIterator first,InputIterator last)
			: t(Compare()) {
			t.insert_unique(first, last);
		}

		template <typename InputIterator>
		set(InputIterator first,InputIterator last,const Compare &comp)
			: t(comp) {
			t.insert_unique(first, last);
		}

		set(const set<Key,Compare,Alloc> &x):t(x.t) {}

		set<Key, Compare, Alloc> &operator=(const set<Key, Compare, Alloc> &x)
		{
			t = x.t;
			return *this;
		}

		key_compare key_comp() const { return t.key_comp(); }
		value_compare value_comp() const { return t.key_comp(); }
		iterator begin() const { return t.begin(); }
		iterator end() const { return t.end(); }
		bool empty() const { return t.empty(); }
		size_type size() const { return t.size(); }
		size_type max_size() const { return t.max_size(); }
		void swap(set<Key, Compare, Alloc> &x) { t.swap(x.t); }

		typedef pair<iterator, bool> pair_iterator_bool;
		pair<iterator, bool> insert(const value_type &val)
		{
			pair<typename rep_type::iterator, bool> p = t.insert_unique(val);
			return pair<iterator, bool>(p.first, p.second);
		}

		iterator insert(iterator position, const value_type &val)
		{
			typedef typename rep_type::iterator rep_iterator;
			return t.insert_unique((rep_iterator&)position, val);
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last)
		{
			t.insert_unique(first, last);
		}

		void erase(iterator position)
		{
			typedef typename rep_type::iterator rep_iterator;
			t.erase((rep_iterator&)position);
		}

		size_type erase(const key_type &x)
		{
			return t.erase(x);
		}

		void clear()
		{
			t.clear();
		}

		iterator find(const key_type &x) const { return t.find(x); }
		size_type count(const key_type &x) const
		{
			return t.count(x);
		}
		
		iterator lower_bound(const key_type &x) const
		{
			return t.lower_bound(x);
		}

		iterator upper_bound(const key_type &x) const
		{
			return t.upper_bound(x);
		}

		pair<iterator, iterator> equal_range(const key_type &x) const
		{
			return t.equal_range(x);
		}

		friend bool operator==(const set &x, const set &y);
		friend bool operator<(const set&, const set&);
	};

	template <typename Key,typename Compare,typename Alloc>
	inline bool operator==(const set<Key, Compare, Alloc> &x,
		const set<Key, Compare, Alloc> &y)
	{
		return x.t == y.t;
	}

	template <typename Key,typename Compare,typename Alloc>
	inline bool operator<(const set<Key, Compare, Alloc> &x,
		const set<Key, Compare, Alloc> &y)
	{
		return x.t < y.t;
	}
}

#endif