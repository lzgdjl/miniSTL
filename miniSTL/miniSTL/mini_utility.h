#ifndef _UTILITY_H
#define _UTILITY_H

namespace miniSTL
{
	// swap
	template <typename T>
	void swap(T &a, T &b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	// pair
	template <typename T1,typename T2>
	struct pair
	{
	public:
		typedef T1 first_type;
		typedef T2 second_type;
		
	public:
		T1 first;
		T2 second;

	public:
		pair() = default;

		template <typename U,typename V>
		pair(const pair<U, V> &pr);
		pair(const first_type &a, const second_type &b);
		pair& operator=(const pair &pr);
		void swap(pair &pr);

	public:
		template <typename T11,typename T22>
		friend bool operator==(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs);

		template <typename T11,typename T22>
		friend bool operator!=(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs);

		template <typename T11,typename T22>
		friend bool operator<(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs);

		template <typename T11,typename T22>
		friend bool operator<=(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs);

		template <typename T11,typename T22>
		friend bool operator>(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs);

		template <typename T11,typename T22>
		friend bool operator>=(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs);

		template <typename T11,typename T22>
		friend void swap(pair<T11, T22> &x, pair<T11, T22> &y);
	};

	template <typename T1,typename T2>
	template <typename U,typename V>
	pair<T1,T2>::pair(const pair<U,V> &pr):first(pr.first),second(pr.second){}

	template <typename T1,typename T2>
	pair<T1,T2>::pair(const first_type &a,const second_type &b):first(a),second(b){}

	template <typename T1,typename T2>
	pair<T1, T2>& pair<T1, T2>::operator=(const pair<T1, T2> &pr)
	{
		if (this != &pr)
		{
			first = pr.first;
			second = pr.second;
		}
	}

	template <typename T1,typename T2>
	void pair<T1, T2>::swap(pair<T1, T2> &pr)
	{
		miniSTL::swap(first, pr.first);
		miniSTL::swap(second, pr.second);
	}

	template <typename T11,typename T22>
	bool operator==(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs)
	{
		return lhs.first == rhs.first && lhs.second == rhs.second;
	}

	template <typename T11,typename T22>
	bool operator!=(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs)
	{
		return !(lhs == rhs);
	}

	template <typename T11,typename T22>
	bool operator<(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs)
	{
		return lhs.first < rhs.first || (!(rhs.first < lhs.first) && lhs.second < rhs.second);
	}

	template <typename T11,typename T22>
	bool operator<=(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs)
	{
		return !(rhs < lhs);
	}

	template <typename T11,typename T22>
	bool operator>(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs)
	{
		return rhs < lhs;
	}

	template <typename T11,typename T22>
	bool operator>=(const pair<T11, T22> &lhs, const pair<T11, T22> &rhs)
	{
		return !(lhs < rhs);
	}

	// make_pair
	template <typename T1,typename T2>
	pair<T1, T2> make_pair(const T1 &t1, const T2 &t2)
	{
		return pair<T1, T2>(t1, t2);
	}
}

#endif