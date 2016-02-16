#ifndef _UNORDERED_SET_H
#define _UNORDERED_SET_H

#include "mini_allocator.h"
#include "mini_algorithm.h"
#include "mini_functional.h"
#include "mini_iterator.h"
#include "mini_list.h"
#include "mini_vector.h"

#include <functional>

namespace miniSTL
{
	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	class unordered_set;

	namespace
	{
		template <typename Key, typename ListIterator, typename Hash = std::hash<Key>,
			typename KeyEqual = miniSTL::equal_to<Key>, typename Allocator = miniSTL::allocator < Key >>
		class ust_iterator :public iterator<forward_iterator_tag, Key>
		{
		private:
			template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
			friend class unordered_set;

		private:
			typedef unordered_set<Key, Hash, KeyEqual, Allocator>*	cntr_ptr;
			size_t bucket_index;
			ListIterator iterator;
			cntr_ptr container;

		public:
			ust_iterator(size_t index, ListIterator it, cntr_ptr ptr);
			ust_iterator &operator++();
			ust_iterator operator++(int);
			Key &operator*() { return *iterator; }
			Key *operator->() { return &(operator*()); }

		private:
			template <typename Key, typename ListIterator, typename Hash, typename KeyEqual, typename Allocator>
			friend bool operator==(const ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator> &lhs,
				const ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator> &rhs);
			template <typename Key, typename ListIterator, typename Hash, typename KeyEqual, typename Allocator>
			friend bool operator!=(const ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator> &lhs,
				const ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator> &rhs);
		};
	}

	template <typename Key, typename Hash = std::hash<Key>,
		typename KeyEqual = miniSTL::equal_to<Key>, typename Allocator = miniSTL::allocator < Key >>
	class unordered_set final
	{
	private:
		template <typename Key, typename ListIterator, typename Hash, typename KeyEqual, typename Allocator>
		friend class ust_iterator;

	public:
		typedef Key												key_type;
		typedef Key												value_type;
		typedef size_t											size_type;
		typedef Hash											haser;
		typedef KeyEqual										key_equal;
		typedef Allocator										allocator_type;
		typedef value_type&										reference;
		typedef const value_type&								const_reference;
		typedef typename miniSTL::list<key_type>::iterator		local_iterator;
		typedef ust_iterator<Key, typename miniSTL::list<key_type>::iterator, Hash, KeyEqual, Allocator> iterator;

	private:
		vector<list<key_type>> buckets;
		size_type size;
		float max_load_factor;
		enum struct PRIME_LIST { PRIME_LIST_SIZE = 28 };
		static size_t prime_list[PRIME_LIST::PRIME_LIST_SIZE];

	public:
		explicit unordered_set(size_t bucket_count);
		template <typename InputIterator>
		unordered_set(InputIterator first, InputIterator last);
		unordered_set(const unordered_set &ust);
		unordered_set &operator=(const unordered_set &ust);

		size_type size() const;
		bool empty() const;
		size_type bucket_count() const;
		size_type bucket_size(size_type i) const;
		size_type bucket(const key_type &key) const;
		float load_factor() const;
		float max_load_factor() const;
		void max_load_factor(float z);
		void rehash(size_type n);

		iterator begin();
		iterator end();
		local_iterator begin(size_type i);
		local_iterator end(size_type i);

		iterator find(const key_type &key);
		size_type count(const key_type &key);

		pair<iterator, bool> insert(const value_type &val);
		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last);
		iterator erase(iterator position);
		size_type erase(const key_type &key);

		haser hash_function() const;
		key_equal key_eq() const;
		allocator_type get_allocator() const;

	private:
		size_type next_prime(size_type n) const;
		size_type bucket_index(const key_type &key) const;
		bool has_key(const key_type &key);

	public:
		template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
		friend void swap(unordered_set<Key, Hash, KeyEqual, Allocator> &lhs,
			unordered_set<Key, Hash, KeyEqual, Allocator> &rhs);
	};

	namespace
	{
		template <typename Key, typename ListIterator, typename Hash, typename KeyEqual, typename Allocator>
		ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator>::ust_iterator(size_t index, ListIterator it, cntr_ptr ptr)
			:bucket_index(index), iterator(it), container(ptr) {}

		template <typename Key, typename ListIterator, typename Hash, typename KeyEqual, typename ALlocator>
		ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator>
			&ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator>::operator++()
		{
			++iterator;

			if (iterator == container->buckets[bucket_index].end())
			{
				for (;;)
				{
					if (bucket_index == container->buckets.size() - 1)
					{
						*this = container->end();
						break;
					}
					else
					{
						++bucket_index;
						if (!(container->buckets[bucket_index].empty()))
						{
							iterator = container.buckets[bucket_index].begin();
							break;
						}
					}
				}
			}

			return *this;
		}

		template <typename Key, typename ListIterator, typename Hash, typename KeyEqual, typename Allocator>
		ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator>
			ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator>::operator++(int)
		{
			auto result = *this;
			++*this;
			return result;
		}

		template <typename Key, typename ListIterator, typename Hash, typename KeyEqual, typename Allocator>
		bool operator==(const ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator> &lhs,
			const ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator> &rhs)
		{
			return lhs.bucket_index == rhs.bucket_index &&
				lhs.iterator == rhs.iterator &&
				lhs.container == rhs.container;
		}

		template <typename Key, typename ListIterator, typename Hash, typename KeyEqual, typename Allocator>
		bool operator!=(const ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator> &lhs,
			const ust_iterator<Key, ListIterator, Hash, KeyEqual, Allocator> &rhs)
		{
			return !(lhs == rhs);
		}
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::size_type
		unordered_set < Key, Hash, KeyEqual, Allocator>::size() const
	{
		return size;
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	bool unordered_set<Key, Hash, KeyEqual, Allocator>::empty() const
	{
		return size() == 0;
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	bool unordered_set<Key, Hash, KeyEqual, Allocator>::empty() const
	{
		return size() == 0;
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::size_type
		unordered_set<Key, Hash, KeyEqual, Allocator>::bucket_count() const
	{
		return buckets.size();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	unordered_set<Key, Hash, KeyEqual, Allocator>::size_type
		unordered_set<Key, Hash, KeyEqual, Allocator>::bucket_size(size_type i) const
	{
		return buckets[i].size();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::size_type
		unordered_set<Key, Hash, KeyEqual, Allocator>::bucket(const key_type &key) const
	{
		return bucket_index(key);
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	float unordered_set<Key, Hash, KeyEqual, Allocator>::load_factor() const
	{
		return (float)size() / (float)bucket_count();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::haser
		unordered_set<Key, Hash, KeyEqual, Allocator>::hash_function() const
	{
		return haser();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::key_equal
		unordered_set<Key, Hash, KeyEqual, Allocator>::key_eq() const
	{
		return key_equal();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::allocator_type
		unordered_set<Key, Hash, KeyEqual, Allocator>::get_allocator() const
	{
		return allocator_type();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	size_t unordered_set<Key, Hash, KeyEqual, Allocator>::prime_list[PRIME_LIST::PRIME_LIST_SIZE] =
	{
		53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
		786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611,
		402653189, 805306457, 1610612741, 3221225473, 4294967291
	};

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	bool unordered_set<Key, Hash, KeyEqual, Allocator>::has_key(const key_type &key)
	{
		auto &result_list = buckets[bucket_index(key)];
		auto pred = std::bind(KeyEqual(), key, std::placeholders::_1);
		return find_if(list.begin(), result_list.end(), pred) != result_list.end();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::size_type
		unordered_set<Key, Hash, KeyEqual, Allocator>::bucket_index(const key_type &key) const
	{
		return haser()(key) % buckets.size();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::size_type
		unordered_set<Key, Hash, KeyEqual, Allocator>::next_prime(size_type n) const
	{
		size_type i = 0;
		for (;i != PRIME_LIST::PRIME_LIST_SIZE;++i)
		{
			if (n > prime_list[i])
				continue;
			else
				break;
		}
		i = (i == PRIME_LIST::PRIME_LIST_SIZE ? PRIME_LIST::PRIME_LIST_SIZE - 1 : i);
		return prime_list[i];
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	unordered_set<Key, Hash, KeyEqual, Allocator>::unordered_set(const unordered_set &ust)
	{
		buckets = ust.buckets;
		size = ust.size;
		max_load_factor = ust.max_load_factor;
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	unordered_set<Key, Hash, KeyEqual, Allocator> &unordered_set<Key, Hash, KeyEqual, Allocator>::operator=(const unordered_set &ust)
	{
		if (this != &ust)
		{
			buckets = ust.buckets;
			size = ust.size;
			max_load_factor = ust.max_load_factor;
		}
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	unordered_set<Key, Hash, KeyEqual, Allocator>::unordered_set(size_type bucket_count)
	{
		bucket_count = next_prime(bucket_count);
		buckets.resize(bucket_count);
		size = 0;
		max_load_factor = 1.0;
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	template <typename InputIterator>
	unordered_set<Key, Hash, KeyEqual, Allocator>::unordered_set(InputIterator first, InputIterator last)
	{
		size = 0;
		max_load_factor = 1.0;
		auto len = last - first;
		buckets.resize(next_prime(len));
		for (;first != last;++first)
		{
			auto index = bucket_index(*first);
			if (!has_key(*first))
			{
				buckets[index].push_front(*first);
				++size;
			}
		}
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::iterator
		unordered_set<Key, Hash, KeyEqual, Allocator>::begin()
	{
		size_type index = 0;
		for (;index != buckets.size();++index)
		{
			if (!(buckets[index].empty()))
				break;
		}
		if (index == buckets.size())
			return end();
		return iterator(index, buckets[index].begin(), this);
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::iterator
		unordered_set<Key, Hash, KeyEqual, Allocator>::end()
	{
		return iterator(buckets.size() - 1, buckets[buckets.size() - 1].end(), this);
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::local_iterator
		unordered_set<Key, Hash, KeyEqual, Allocator>::begin(size_type i)
	{
		return buckets[i].begin();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::local_iterator
		unordered_set<Key, Hash, KeyEqual, Allocator>::end(size_type i)
	{
		return buckets[i].end();
	}

	template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
	typename unordered_set<Key, Hash, KeyEqual, Allocator>::iterator
		unordered_set<Key, Hash, KeyEqual, Allocator>::find(const key_type &key)
	{
		auto index = bucket_index(key);
		for (auto it = begin(index);it != end(index);++it)
		{
			if (key_equal()(key, *it))
				return iterator(index, it, this);
		}
		return end();
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	typename unordered_set<Key,Hash,KeyEqual,Allocator>::size_type
		unordered_set<Key, Hash, KeyEqual, Allocator>::count(const key_type &key)
	{
		auto it = find(key);
		return it == end() ? 0 : 1;
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	pair<typename unordered_set<Key,Hash,KeyEqual,Allocator>::iterator,bool>
		unordered_set<Key, Hash, KeyEqual, Allocator>::insert(const value_type &val)
	{
		if (!has_key(val))
		{
			if (load_factor() > max_load_factor())
				rehash(next_prime(size()));
			auto index = bucket_index(val);
			buckets[index].push_front(val);
			++size;
			return pait<iterator, bool>(iterator(index, buckets[index].begin(), this), true);
		}
		return pair<iterator, bool>(end(), false);
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	template <typename InputIterator>
	void unordered_set<Key, Hash, KeyEqual, Allocator>::insert(InputIterator first, InputIterator last)
	{
		for (;first != last;++first)
		{
			insert(*first);
		}
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	typename unordered_set<Key,Hash,KeyEqual,Allocator>::iterator
		unordered_set<Key, Hash, KeyEqual, Allocator>::erase(iterator position)
	{
		--size;
		auto t = position++;
		auto index = t.bucket_index;
		auto it = buckets[index].erase(t.iterator);
		return position;
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	typename unordered_set<Key,Hash,KeyEqual,Allocator>::size_type
		unordered_set<Key, Hash, KeyEqual, Allocator>::erase(const key_type &key)
	{
		auto it = find(key);
		if (it == end())
			return 0;
		else
		{
			erase(it);
			return 1;
		}
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	float unordered_set<Key, Hash, KeyEqual, Allocator>::max_load_factor() const
	{
		return max_load_factor;
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	void unordered_set<Key, Hash, KeyEqual, Allocator>::max_load_factor(float z)
	{
		max_load_factor = z;
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	void unordered_set<Key, Hash, KeyEqual, Allocator>::rehash(size_type n)
	{
		if (n <= buckets.size())
			return;
		unordered_set tmp(next_prime(n));
		for (auto &val : *this)
		{
			tmp.insert(val);
		}
		swap(*this, tmp);
	}

	template <typename Key,typename Hash,typename KeyEqual,typename Allocator>
	void swap(unordered_set<Key, Hash, KeyEqual, Allocator> &lhs,
		unordered_set<Key, Hash, KeyEqual, Allocator> &rhs)
	{
		swap(lhs.buckets, rhs.buckets);
		swap(lhs.size, rhs.size);
		swap(lhs.max_load_factor, rhs.max_load_factor);
	}
}

#endif