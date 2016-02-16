#ifndef _STRING_H
#define _STRING_H

#include "mini_allocator.h"
#include "mini_reverse_iterator.h"
#include "mini_uninitialized_functions.h"
#include "mini_utility.h"
#include "mini_type_traits.h"

#include <cstring>
#include <type_traits>

namespace miniSTL
{
	class string final
	{
	public:
		typedef char							value_type;
		typedef char*							iterator;
		typedef const char*						const_iterator;
		typedef reverse_iterator_t<char*>		reverse_iterator;
		typedef reverse_iterator_t<const char*> const_reverse_iterator;
		typedef value_type&						reference;
		typedef const value_type&				const_reference;
		typedef size_t							size_type;
		typedef ptrdiff_t						difference_type;
		// npos 表示最多含有的字符数
		static const size_t npos = -1;

	private:
		char *start;
		char *finish;
		char *end_of_storage;

		typedef miniSTL::allocator<char> data_allocator;

	public:
		string():start(nullptr),finish(nullptr),end_of_storage(nullptr){}
		string(const string &str);
		string(string &&str);
		string(const string &str, size_t pos, size_t len = npos);
		string(const char *s);
		string(const char *s, size_t n);
		string(size_t n, char c);
		
		template <typename InputIterator>
		string(InputIterator first, InputIterator last)
		{
			allocate_and_copy(first, last);
		}

		string &operator=(const string &str);
		string &operator=(string &&str);
		string &operator=(const char *s);
		string &operator=(char ch);

		~string();

		iterator begin() { return start; }
		const_iterator begin() const { return start; }

		iterator end() { return finish; }
		const_iterator end() const { return finish; }

		reverse_iterator rbegin() { return reverse_iterator(finish); }
		const_reverse_iterator rbegin() const { return const_reverse_iterator(finish); }

		reverse_iterator rend() { return reverse_iterator(start); }
		reverse_iterator rend() const { return reverse_iterator(start); }

		const_iterator cbegin() const { return start; }
		const_iterator cend() const { return finish; }
		const_reverse_iterator crbegin() const { return const_reverse_iterator(finish); }
		const_reverse_iterator crend() const { return const_reverse_iterator(start); }
		
		size_t size() const { return finish - start; }
		size_t length() const { return size(); }
		size_t capacity() const { return end_of_storage - start; }

		void clear()
		{
			data_allocator::destroy(start, finish);
			start = finish;
		}

		bool empty() const { return start == finish; }
		void resize(size_t n);
		void resize(size_t n, char ch);
		void reserve(size_t n = 0);
		void shrink_to_fit()
		{
			data_allocator::deallocate(finish, end_of_storage - finish);
			end_of_storage = finish;
		}

		char &operator[](size_t pos) { return *(start + pos); }
		const char &operator[](size_t pos) const { return *(start + pos); }

		char &back() { return *(finish - 1); }
		const char &back() const { return *(finish - 1); }

		char &front() { return *start; }
		const char &front() const { return *start; }

		string &insert(size_t pos, const string &str);
		string &insert(size_t pos, const string &str, size_t subpos, size_t sublen = npos);
		string &insert(size_t pos, const char *s);
		string &insert(size_t pos, const char *s, size_t n);
		string &insert(size_t pos, size_t n, char c);

		iterator insert(iterator p, size_t n, char c);
		iterator insert(iterator p, char c);

		void push_back(char c) { insert(finish, c); }

		template <typename InputIterator>
		iterator insert(iterator p, InputIterator first, InputIterator last);

		string &append(const string &str);
		string &append(const string &str, size_t subpos, size_t sublen = npos);
		string &append(const char *s);
		string &append(const char *, size_t n);
		string &append(size_t n, char c);

		template <typename InputIterator>
		string &append(InputIterator first, InputIterator last);

		string &operator+=(const string &str);
		string &operator+=(const char *s);
		string &operator+=(char c);

		string &erase(size_t pos = 0, size_t len = npos);
		void pop_back() { erase(finish - 1, finish); }
		iterator erase(iterator p);
		iterator erase(iterator first, iterator last);

		string &replace(size_t pos, size_t len, const string &str);
		string &replace(iterator i1, iterator i2, const string &str);
		string &replace(size_t pos, size_t len, const string &str, size_t subpos, size_t sublen = npos);
		string &replace(size_t pos, size_t len, const char *s);
		string &replace(iterator i1, iterator i2, const char *s);
		string &replace(size_t pos, size_t len, const char *s, size_t n);
		string &replace(iterator i1, iterator i2, const char *s, size_t n);
		string &replace(size_t pos, size_t len, size_t n, char c);
		string &replace(iterator i1, iterator i2, size_t n, char c);

		template <typename InputIterator>
		string &replace(iterator i1, iterator i2, InputIterator first, InputIterator last);

		void swap(string &str)
		{
			miniSTL::swap(start, str.start);
			miniSTL::swap(finish, str.finish);
			miniSTL::swap(end_of_storage, str.end_of_storage);
		}

		size_t copy(char *s,size_t len,size_t pos = 0) const
		{
			auto ptr = miniSTL::globals::uninitialized_copy(start + pos, start + pos + len, s);
			return static_cast<size_t>(ptr - s);
		}

		size_t find(const string &str, size_t pos = 0) const;
		size_t find(const char *s, size_t pos = 0) const;
		size_t find(const char *s, size_t pos, size_t n) const;
		size_t find(char c, size_t pos = 0) const;

		size_t rfind(const string &str, size_t pos = npos) const;
		size_t rfind(const char *s, size_t pos = npos) const;
		size_t rfind(const char *s, size_t pos, size_t n) const;
		size_t rfind(char c, size_t pos = npos) const;

		size_t find_first_of(const string &str, size_t pos = 0) const;
		size_t find_first_of(const char *s, size_t pos = 0) const;
		size_t find_first_of(const char *s, size_t pos, size_t n) const;
		size_t find_first_of(char c, size_t pos = 0) const;

		size_t find_last_of(const string &str, size_t pos = npos) const;
		size_t find_last_of(const char *s, size_t pos = npos) const;
		size_t find_last_of(const char *s, size_t pos, size_t n) const;
		size_t find_last_of(char c, size_t pos = npos) const;

		size_t find_first_not_of(const string &str, size_t pos = 0) const;
		size_t find_first_not_of(const char *s, size_t pos = 0) const;
		size_t find_first_not_of(const char *s, size_t pos, size_t n) const;
		size_t find_first_not_of(char c, size_t pos = 0) const;

		size_t find_last_not_of(const string &str, size_t pos = npos) const;
		size_t find_last_not_of(const char *s, size_t pos = npos) const;
		size_t find_last_not_of(const char *s, size_t pos, size_t n) const;
		size_t find_last_not_of(char c, size_t pos = npos) const;

		string substr(size_t pos = 0, size_t len = npos) const
		{
			len = change_var_when_equal_npos(len, size(), pos);
			return string(begin() + pos, begin() + pos + len);
		}

		int compare(const string &str) const;
		int compare(size_t pos, size_t len, const string &str) const;
		int compare(size_t pos, size_t len, const string &str,
			size_t subpos, size_t sublen = npos) const;
		int compare(const char *s) const;
		int compare(size_t pos, size_t len, const char *s) const;
		int compare(size_t pos, size_t len, const char *s, size_t n) const;

	private:
		void move_data(string &str);
		// 插入时空间不足
		template <typename InputIterator>
		iterator insert_aux_copy(iterator p, InputIterator first, InputIterator last);

		// 插入时空间不足的情况
		iterator insert_aux_fill_n(iterator p, size_t n, value_type c);
		size_type get_new_capacity(size_type len) const;

		void allocate_and_fill_n(size_t n, char c);

		template <typename InputIterator>
		void allocate_and_copy(InputIterator first, InputIterator last);
		
		void destroy_and_deallocate();

		size_t rfind_aux(const_iterator cit, size_t pos, size_t length_of_s, int cond) const;
		size_t find_aux(const_iterator cit, size_t pos, size_t length_of_s, size_t cond) const;
		int compare_aux(size_t pos, size_t len, const_iterator cit, size_t subpos, size_t sublen) const;
		bool is_contained(char c, const_iterator first, const_iterator last) const;
		size_t change_var_when_equal_npos(size_t pos, size_t min_uend, size_t minue) const;

	public:
		friend std::ostream &operator <<(std::ostream &os, const string &str);
		friend std::istream &operator>>(std::istream &is, string &str);
		friend string operator+(const string &lhs, const string &rhs);
		friend string operator+(const string &lhs, const char *rhs);
		friend string operator+(const char *lhs, const string &rhs);
		friend string operator+(const string &lhs, char rhs);
		friend string operator+(char lhs, const string &rhs);

		friend bool operator==(const string &lhs, const string &rhs);
		friend bool operator==(const char *lhs, const string &rhs);
		friend bool operator==(const string &lhs, const char *rhs);

		friend bool operator!=(const string &lhs, const char *rhs);
		friend bool operator!=(const char *lhs, const string &rhs);
		friend bool operator!=(const string &lhs, const char *rhs);

		friend bool operator<(const string &lhs, const string &rhs);
		friend bool operator<(const char *lhs, const string &rhs);
		friend bool operator<(const string &lhs, const char *rhs);

		friend bool operator<=(const string &lhs, const string &rhs);
		friend bool operator<=(const char *lhs, const string &rhs);
		friend bool operator<=(const char *lhs, const string &rhs);

		friend bool operator>(const string &lhs, const string &rhs);
		friend bool operator>(const char *lhs, const string &rhs);
		friend bool operator>(const string &lhs, const char *rhs);

		friend bool operator>=(const string &lhs, const string &rhs);
		friend bool operator>=(const char *lhs, const string &rhs);
		friend bool operator>=(const string &lhs, const char *rhs);

		friend void swap(string &x, string &y);
		friend std::istream &getline(std::istream &is, string &str);
		friend std::istream &getline(std::istream &is, string &str, char delim);
	};// 声明结束

	template <typename InputIterator>
	string::iterator string::insert_aux_copy(iterator p, InputIterator first, InputIterator last)
	{
		size_t length_of_insert = distance(first, last);
		auto new_capacity = get_new_capacity(length_of_insert);
		iterator new_start = data_allocator::allocate(new_capacity);
		iterator new_finish = miniSTL::globals::uninitialized_copy(start, p, new_start);
		new_finish = miniSTL::globals::uninitialized_copy(first, last, new_finish);
		auto res = new_finish;
		new_finish = miniSTL::globals::uninitialized_copy(p, finish, new_finish);

		destroy_and_deallocate();
		// 调整
		start = new_start;
		finish = new_finish;
		end_of_storage = start + new_capacity;
		return res;
	}

	template <typename InputIterator>
	string::iterator string::insert(iterator p, InputIterator first, InputIterator last)
	{
		auto length_of_left = capacity() - size();
		size_t length_of_insert = distance(first, last);
		if (length_of_insert <= length_of_left)
		{
			// 剩余空间大于插入所需空间
			for (iterator it = finish - 1;it >= p;--it)
			{
				*(it + length_of_insert) = *it;
			}

			globals::uninitialized_copy(first, last, p);
			finish += length_of_insert;
			return p + length_of_insert;
		}
		else
		{
			// 空间不足
			return insert_aux_copy(p, first, last);
		}
	}

	template <typename InputIterator>
	string &string::append(InputIterator first, InputIterator last)
	{
		insert(end(), first, last);
		return *this;
	}

	template <typename InputIterator>
	string &string::replace(iterator i1, iterator i2,
		InputIterator first, InputIterator last)
	{
		auto ptr = erase(i1, i2);
		insert(ptr, first, last);
		return *this;
	}

	template <typename InputIterator>
	void string::allocate_and_copy(InputIterator first, InputIterator last)
	{
		start = data_allocator::allocate(last - first);
		finish = globals::uninitialized_copy(first, last, start);
		end_of_storage = finish;
	}
}

#endif
