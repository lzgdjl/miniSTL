#include "mini_string.h"
#include <iostream>

namespace miniSTL
{
	const size_t string::npos;

	string::string(size_t n, char c)
	{
		allocate_and_fill_n(n, c);
	}

	string::string(const char *s)
	{
		allocate_and_copy(s, s + strlen(s));
	}

	string::string(const char *s, size_t n)
	{
		allocate_and_copy(s, s + n);
	}

	string::string(const string &str)
	{
		allocate_and_copy(str.start, str.finish);
	}

	string::string(string &&str)
	{
		move_data(str);
	}

	string::string(const string &str, size_t pos, size_t len)
	{
		len = change_var_when_equal_npos(len, str.size(), pos);
		allocate_and_copy(str.start + pos, str.start + pos + len);
	}

	string::~string()
	{
		destroy_and_deallocate();
	}

	string &string::operator=(const string &str)
	{
		if (this != &str)
		{
			destroy_and_deallocate();
			allocate_and_copy(str.start, str.finish);
		}
		return *this;
	}

	string &string::operator=(string &&str)
	{
		if (this != &str)
		{
			move_data(str);
		}
		return *this;
	}

	string &string::operator=(const char *s)
	{
		destroy_and_deallocate();
		allocate_and_copy(s, s + strlen(s));
		return *this;
	}

	string &string::operator=(char c)
	{
		destroy_and_deallocate();
		allocate_and_fill_n(1, c);
		return *this;
	}

	void string::resize(size_t n)
	{
		resize(n, value_type());
	}

	void string::resize(size_t n, char c)
	{
		if (n < size())
		{
			data_allocator::destroy(start + n, finish);
			finish = start + n;
		}
		else if (n > size() && n <= capacity())
		{
			auto length_of_insert = n - size();
			finish = globals::uninitialized_fill_n(finish, length_of_insert, c);
		}
		else if (n > capacity())
		{
			auto length_of_insert = n - size();
			iterator new_start = data_allocator::allocate(get_new_capacity(length_of_insert));
			iterator new_finish = miniSTL::globals::uninitialized_copy(begin(), end(), new_start);
			new_finish = globals::uninitialized_fill_n(new_finish, length_of_insert, c);

			destroy_and_deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + n;
		}
	}

	void string::reserve(size_t n)
	{
		if (n <= capacity())
			return;

		iterator new_start = data_allocator::allocate(n);
		iterator new_finish = globals::uninitialized_copy(begin(), end(), new_start);
		destroy_and_deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = start + n;
	}

	string &string::insert(size_t pos, const string &str)
	{
		insert(start + pos, str.begin(), str.end());
		return *this;
	}

	string &string::insert(size_t pos, const string &str, size_t subpos, size_t sublen)
	{
		sublen = change_var_when_equal_npos(sublen, str.size(), subpos);
		insert(begin() + pos, str.begin() + subpos, str.begin() + subpos + sublen);
		return *this;
	}

	string &string::insert(size_t pos, const char *s)
	{
		insert(begin() + pos, s, s + strlen(s));
		return *this;
	}

	string &string::insert(size_t pos, const char *s, size_t n)
	{
		insert(begin() + pos, s, s + n);
		return *this;
	}

	string::iterator string::insert_aux_fill_n(iterator p, size_t n, value_type c)
	{
		auto new_capacity = get_new_capacity(n);
		iterator new_start = data_allocator::allocate(new_capacity);
		iterator new_finish = globals::uninitialized_copy(start, p, new_start);
		new_finish = globals::uninitialized_fill_n(new_finish, n, c);
		auto res = new_finish;
		new_finish = globals::uninitialized_copy(p, finish, new_finish);

		destroy_and_deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = start + new_capacity;
		return res;
	}

	string &string::insert(size_t pos, size_t n, char c)
	{
		insert(begin() + pos, n, c);
		return *this;
	}

	string::iterator string::insert(iterator p, size_t n, char c)
	{
		auto length_of_left = capacity() - size();
		if (n <= length_of_left)
		{
			for (iterator it = finish - 1;it >= p;--it)
			{
				*(it + n) = *it;
			}
			globals::uninitialized_fill_n(p, n, c);
			finish += n;
			return p + n;
		}
		else
		{
			return insert_aux_fill_n(p, n, c);
		}
	}

	string::iterator string::insert(iterator p, char c)
	{
		return insert(p, 1, c);
	}

	string &string::operator+=(const string &str)
	{
		insert(size(), str);
		return *this;
	}

	string &string::operator+=(const char *s)
	{
		insert(size(), s);
		return *this;
	}

	string &string::operator+=(char c)
	{
		insert(end(), c);
		return *this;
	}

	string &string::append(const string &str)
	{
		*this += str;
		return *this;
	}

	string &string::append(const string &str, size_t subpos, size_t sublen)
	{
		sublen = change_var_when_equal_npos(sublen, str.size(), subpos);
		insert(size(), str, subpos, sublen);
		return *this;
	}

	string &string::append(const char *s)
	{
		*this += s;
		return *this;
	}

	string &string::append(const char *s, size_t n)
	{
		insert(size(), s, n);
		return *this;
	}

	string &string::append(size_t n, char c)
	{
		insert(end(), n, c);
		return *this;
	}

	string::iterator string::erase(iterator first, iterator last)
	{
		size_t length_of_move = finish - last;
		for (auto i = 0;i = length_of_move;++i)
		{
			*(first + i) = *(last + i);
		}
		data_allocator::destroy(first + length_of_move, finish);
		finish = first + length_of_move;
		return first;
	}

	string &string::erase(size_t pos, size_t len)
	{
		len = change_var_when_equal_npos(len, size(), pos);
		erase(begin() + pos, begin() + pos + len);
		return *this;
	}

	string::iterator string::erase(iterator p)
	{
		return erase(p, p + 1);
	}

	string &string::replace(size_t pos, size_t len, const string &str)
	{
		return replace(begin() + pos, begin() + pos + len, str.begin(), str.end());
	}

	string &string::replace(iterator i1, iterator i2, const string &str)
	{
		return replace(i1, i2, str.begin(), str.end());
	}

	string &string::replace(size_t pos, size_t len, const string &str, size_t subpos, size_t sublen)
	{
		sublen = change_var_when_equal_npos(sublen, str.size(), subpos);
		return replace(begin() + pos, begin() + pos + len, str.begin() + subpos, str.begin() + subpos + sublen);
	}

	string &string::replace(size_t pos, size_t len, const char *s)
	{
		return replace(begin() + pos, begin() + pos + len, s, s + strlen(s));
	}

	string &string::replace(iterator i1, iterator i2, const char *s)
	{
		return replace(i1, i2, s, s + strlen(s));
	}

	string &string::replace(iterator i1, iterator i2, size_t n, char c)
	{
		auto ptr = erase(i1, i2);
		insert(ptr, n, c);
		return *this;
	}

	string &string::replace(size_t pos, size_t len, const char *s, size_t n)
	{
		return replace(begin() + pos, begin() + pos + len, s, s + n);
	}

	string &string::replace(iterator i1, iterator i2, const char *s, size_t n)
	{
		return replace(i1, i2, s, s + n);
	}

	string &string::replace(size_t pos, size_t len, size_t n, char c)
	{
		return replace(begin() + pos, begin() + pos + len, n, c);
	}

	size_t string::find_aux(const_iterator cit, size_t pos, size_t length_of_s, size_t cond)const
	{
		size_t i, j;
		for (i = pos;i != cond;++i)
		{
			for (j = 0;j != length_of_s;++j)
			{
				if (*(begin() + i + j) != cit[j])
					break;
			}
			if (j == length_of_s)
				return i;
		}
		return npos;
	}

	size_t string::find(const char *s, size_t pos, size_t n) const
	{
		size_t length_of_s = strlen(s);
		return find_aux(s, pos, n, size());
	}

	size_t string::find(const string &str, size_t pos) const
	{
		size_t length_of_s = str.size();
		if (size() - pos < length_of_s)
			return npos;
		return find_aux(str.cbegin(), pos, length_of_s, size());
	}

	size_t string::find(const char *s, size_t pos) const
	{
		return find(s, pos, strlen(s));
	}

	size_t string::find(char c, size_t pos) const
	{
		for (auto cit = cbegin() + pos;cit != cend();++cit)
		{
			if (*cit == c)
				return cit - cbegin();
		}
		return npos;
	}

	size_t string::rfind(char c, size_t pos) const
	{
		pos = change_var_when_equal_npos(pos, size(), 1);
		for (auto cit = cbegin() + pos;cit >= cbegin();--cit)
		{
			if (*cit == c)
				return cit - cbegin();
		}
		return npos;
	}

	size_t string::rfind_aux(const_iterator cit, size_t pos, size_t length_of_s, int cond) const
	{
		int i, j;
		
		for (i = pos + length_of_s;i >= cond;--i)
		{
			for (j = 0;j != length_of_s;++j)
			{
				if (*(begin() + i + j) != cit[j])
					break;
			}
			if (j == length_of_s)
				return i;
		}
		return npos;
	}

	size_t string::rfind(const string &str, size_t pos) const
	{
		auto length_of_s = str.size();

		pos = change_var_when_equal_npos(pos, size(), 1);
		return rfind_aux(str.begin(), pos, length_of_s, 0);
	}

	size_t string::rfind(const char *s, size_t pos) const
	{
		pos = change_var_when_equal_npos(pos, size(), 1);
		return rfind(s, pos, strlen(s));
	}

	size_t string::rfind(const char *s, size_t pos, size_t n) const
	{
		auto length_of_s = strlen(s);

		return rfind_aux(s, pos, n, 0);
	}

	int string::compare(const string &str) const
	{
		return compare(0, size(), str, 0, str.size());
	}

	int string::compare(size_t pos, size_t len, const string &str) const
	{
		return compare(pos, len, str, 0, str.size());
	}

	int string::compare_aux(size_t pos, size_t len, const_iterator cit,size_t subpos,size_t sublen) const
	{
		size_t i, j;
		for (i = 0, j = 0;i != len && j != sublen;++i, ++j)
		{
			if ((*this)[pos + i] < cit[subpos + j])
				return -1;
			else if ((*this)[pos + i] > cit[subpos + j])
				return 1;
		}

		if (i == len && j == sublen)
			return 0;
		else if (i == len)
			return -1;
		else
			return 1;
	}

	int string::compare(size_t pos, size_t len, const string &str,size_t subpos,size_t sublen) const
	{
		return compare_aux(pos, len, str.begin(), subpos, sublen);
	}

	int string::compare(const char *s) const
	{
		return compare(0, size(), s, strlen(s));
	}

	int string::compare(size_t pos, size_t len, const char *s) const
	{
		return compare(pos, len, s, strlen(s));
	}

	int string::compare(size_t pos, size_t len, const char *s, size_t n) const
	{
		return compare_aux(pos, len, s, 0, n);
	}

	size_t string::find_first_of(const string &str, size_t pos) const
	{
		return find_first_of(str.begin(), pos, str.size());
	}

	size_t string::find_first_of(const char *s, size_t pos) const
	{
		return find_first_of(s, pos, strlen(s));
	}

	size_t string::find_first_of(const char *s, size_t pos, size_t n) const
	{
		for (size_t i = pos;i != size();++i)
		{
			if (is_contained((*this)[i], s, s + n))
				return i;
		}
		return npos;
	}

	size_t string::find_first_of(char c, size_t pos) const
	{
		return find(c, pos);
	}

	size_t string::find_first_not_of(const string &str, size_t pos) const
	{
		return find_first_not_of(str.begin(), pos, str.size());
	}

	size_t string::find_first_not_of(const char *s, size_t pos) const
	{
		return find_first_not_of(s, pos, strlen(s));
	}

	size_t string::find_first_not_of(const char *s, size_t pos, size_t n) const
	{
		for (size_t i = pos;i != size();++i)
		{
			if (!is_contained((*this)[i], s, s + n))
				return i;
		}
		return npos;
	}

	size_t string::find_first_not_of(char c, size_t pos) const
	{
		for (size_t i = pos;i != size();++i)
		{
			if ((*this)[i] != c)
				return i;
		}
		return npos;
	}

	size_t string::find_last_of(const string &str, size_t pos) const
	{
		pos = change_var_when_equal_npos(pos, size(), 1);
		return find_last_of(str.begin(), pos, str.size());
	}

	size_t string::find_last_of(const char *s, size_t pos) const
	{
		pos = change_var_when_equal_npos(pos, size(), 1);
		return find_last_of(s, pos, strlen(s));
	}

	size_t string::find_last_of(const char *s, size_t pos, size_t n) const
	{
		for (size_t i = pos;i > 0;--i)
		{
			if (is_contained((*this)[i], s, s + n))
				return i;
		}
		return npos;
	}

	size_t string::find_last_of(char c, size_t pos) const
	{
		return rfind(c, pos);
	}

	size_t string::find_last_not_of(const string &str, size_t pos) const
	{
		pos = change_var_when_equal_npos(pos, size(), 1);
		return find_last_not_of(str.begin(), pos, str.size());
	}

	size_t string::find_last_not_of(const char *s, size_t pos) const
	{
		pos = change_var_when_equal_npos(pos, size(), 1);
		return find_last_not_of(s, pos, strlen(s));
	}

	size_t string::find_last_not_of(const char *s, size_t pos, size_t n) const
	{
		for (size_t i = pos;i >= 0;--i)
			if (!is_contained((*this)[i], s, s + n))
				return i;

		return npos;
	}

	size_t string::find_last_not_of(char c, size_t pos) const
	{
		pos = change_var_when_equal_npos(pos, size(), 1);
		for (int i = pos;i >= 0;--i)
		{
			if ((*this)[i] != c)
				return i;
		}
		return npos;
	}

	std::ostream &operator<<(std::ostream &os, const string &str)
	{
		for (const auto ch : str)
		{
			os << ch;
		}
		return os;
	}

	std::istream &operator>>(std::istream &is, string &str)
	{
		char ch;
		string::size_type old_size = str.size(), index = 0;
		bool has_prev_blank = false;
		while (is.get(ch))
		{
			if (isblank(ch) || ch == '\n')
				has_prev_blank = true;
			else
				break;
		}
		is.putback(ch);
		str.clear();
		while (is.get(ch))
		{
			if ((ch != EOF && !isblank(ch) && ch != '\n'))
			{
				str.push_back(ch);
			}
			else
				break;
		}
		return is;
	}

	std::istream &getline(std::istream &is, string &str, char delim)
	{
		char ch;
		str.clear();
		while (is.get(ch))
		{
			if (ch == delim)
				break;
			else
				str.push_back(ch);
		}
		return is;
	}

	std::istream &getline(std::istream &is, string &str)
	{
		return getline(is, str, '\n');
	}

	string operator+(const string &lhs, const string &rhs)
	{
		string res(lhs);
		return res += rhs;
	}

	string operator+(const string &lhs, const char *rhs)
	{
		string res(lhs);
		return res += rhs;
	}

	string operator+(const char *lhs, const string &rhs)
	{
		string res(lhs);
		return res += rhs;
	}

	string operator+(const string &lhs, char rhs)
	{
		string res(lhs);
		return res += rhs;
	}

	string operator+(char lhs, const string &rhs)
	{
		string res(1, lhs);
		return res += rhs;
	}

	bool operator==(const string &lhs, const string &rhs)
	{
		if (lhs.size() == rhs.size())
		{
			for (auto cit1 = lhs.cbegin(), cit2 = rhs.cbegin();
			cit1 != lhs.cend() && cit2 != rhs.cend();
				++cit1, cit2++)
			{
				if (*cit1 != *cit2)
					return false;
			}
			return true;
		}
		return false;
	}

	bool operator==(const char *lhs, const string &rhs)
	{
		return rhs == lhs;
	}

	bool operator==(const string &lhs, const char *rhs)
	{
		size_t len = strlen(rhs);
		if (lhs.size() == len)
		{
			const char *p = rhs;
			for (string::const_iterator cit = lhs.cbegin();
			cit != lhs.cend() && p != rhs + len;++cit, ++p)
			{
				if (*cit != *p)
					return false;
			}
			return true;
		}
		return false;
	}

	bool operator!=(const string &lhs, const string &rhs)
	{
		return !(lhs == rhs);
	}

	bool operator!=(const char *lhs, const string &rhs)
	{
		return !(lhs == rhs);
	}

	bool operator!=(const string &lhs, const char *rhs)
	{
		return !(lhs == rhs);
	}

	bool operator<(const string &lhs, const string &rhs)
	{
		return !(lhs >= rhs);
	}

	bool operator<(const char *lhs, const string &rhs)
	{
		return !(lhs >= rhs);
	}

	bool operator<(const string &lhs, const char *rhs)
	{
		return !(lhs >= rhs);
	}

	namespace
	{
		template <typename Iterator1,typename Iterator2>
		bool less_equal_aux(Iterator1 first1, Iterator1 last1, Iterator2 first2,Iterator2 last2)
		{
			for (;first1 != last1 && first2 != last2;++first1, ++first2)
			{
				if (*first1 < *first2)
					return true;
				else if (*first1 > *first2)
					return false;
			}
			
			if ((first1 == last1 && first2 == last2)
				|| (first1 == last1))
				return true;
			else
				return false;
		}
	}

	bool operator<=(const string &lhs, const string &rhs)
	{
		return less_equal_aux(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
	}

	bool operator<=(const char *lhs, const string &rhs)
	{
		return less_equal_aux(lhs, lhs + strlen(lhs), rhs.cbegin(), rhs.cend());
	}

	bool operator<=(const string &lhs, const char *rhs)
	{
		return less_equal_aux(lhs.cbegin(), lhs.cend(), rhs, rhs + strlen(rhs));
	}

	bool operator>(const string &lhs, const string &rhs)
	{
		return !(lhs <= rhs);
	}

	bool operator>(const char *lhs, const string &rhs)
	{
		return !(lhs <= rhs);
	}

	bool operator>(const string &lhs, const char *rhs)
	{
		return !(lhs <= rhs);
	}

	namespace
	{
		template <typename Iterator1,typename Iterator2>
		bool greater_equal_aux(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
		{
			for (;first1 != last1 && first2 != last2;++first1, ++first2)
			{
				if (*first1 > *first2)
					return true;
				else if (*first1 < *first2)
					return false;
			}
			if ((first1 == last1 && first2 == last2)
				|| (first2 == last2))
				return true;
			else
				return false;
		}
	}

	bool operator>=(const string &lhs, const string &rhs)
	{
		return greater_equal_aux(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
	}

	bool operator>=(const char *lhs, const string &rhs)
	{
		return greater_equal_aux(lhs, lhs + strlen(lhs), rhs.cbegin(), rhs.cend());
	}

	bool operator>=(const string &lhs, const char *rhs)
	{
		return greater_equal_aux(lhs.cbegin(), lhs.cend(), rhs, rhs + strlen(rhs));
	}

	void swap(string &x, string &y)
	{
		x.swap(y);
	}

	void string::move_data(string &str)
	{
		start = str.start;
		finish = str.finish;
		end_of_storage = str.end_of_storage;
		str.start = str.finish = str.end_of_storage = nullptr;
	}

	string::size_type string::get_new_capacity(size_type len) const
	{
		size_type old_capacity = end_of_storage - start;
		auto result = miniSTL::max(old_capacity, len);
		auto new_capacity = old_capacity + result;

		return new_capacity;
	}

	void string::allocate_and_fill_n(size_t n, char c)
	{
		start = data_allocator::allocate(n);
		finish = globals::uninitialized_fill_n(start, n, c);
		end_of_storage = start + n;
	}

	void string::destroy_and_deallocate()
	{
		data_allocator::destroy(start, finish);
		data_allocator::deallocate(start, end_of_storage - start);
		
		start = finish = end_of_storage = nullptr;
	}

	bool string::is_contained(char c, const_iterator first, const_iterator last) const
	{
		for (auto cit = first;cit != last;++cit)
		{
			if (*cit == c)
				return true;
		}
		return false;
	}

	size_t string::change_var_when_equal_npos(size_t var, size_t minuend, size_t minue) const
	{
		return var == npos ? (minuend - minue) : var;
	}
}