#ifndef _REVERSE_ITERATOR_H
#define _REVERSE_ITERATOR_H

#include "mini_iterator.h"

namespace miniSTL
{
	template <typename Iterator>
	class reverse_iterator_t
	{
	public:
		typedef Iterator iterator_type;
		typedef typename iterator_traits<Iterator>::iterator_category   iterator_category;
		typedef typename iterator_traits<Iterator>::value_type			value_type;
		typedef typename iterator_traits<Iterator>::difference_type		difference_type;
		typedef typename iterator_traits<Iterator>::pointer				pointer;
		typedef const pointer											const_pointer;
		typedef typename iterator_traits<Iterator>::reference			reference;
		typedef const reference											const_reference;
	private:
		Iterator _base;
		Iterator _cur;

	public:
		reverse_iterator_t() = default;
		explicit reverse_iterator_t(const iterator_type &it) :_base(it)
		{
			auto temp = it;
			_cur = --temp;
		}
		template <typename Iter>
		reverse_iterator_t(const reverse_iterator_t<Iter> &rev_it)
		{
			_base = reinterpret_cast<iterator_type>(rev_it.base());
			auto temp = _base;
			_cur = --temp;
		}

		// ½Ó¿Úº¯Êý
		iterator_type base() { return _base; }
		reference operator*() { return *_cur; }
		const_reference operator*() const { return *_cur; }
		pointer operator->() { return &(operator*()); }
		const_pointer operator->() const { return &(operator*()); }
		reverse_iterator_t &operator++()
		{
			--_base;
			--_cur;
			return *this;
		}
		reverse_iterator_t operator++(int)
		{
			reverse_iterator_t temp = *this;
			++(*this);
			return temp;
		}
		reverse_iterator_t &operator--()
		{
			++_base;
			++_cur;
			return *this;
		}
		reverse_iterator_t operator--(int)
		{
			reverse_iterator_t temp = *this;
			--(*this);
			return temp;
		}
		reference operator[](difference_type n)
		{
			return base()[-n - 1];
		}
		reverse_iterator_t operator+(difference_type n) const;
		reverse_iterator_t &operator+=(difference_type n);
		reverse_iterator_t operator-(difference_type n) const;
		reverse_iterator_t &operator-=(difference_type n);
		
	private:
		Iterator advance_n_step(Iterator it, difference_type n,
			bool right, random_access_iterator_tag)
		{
			if (right)
				it += n;
			else
				it -= n;
			return it;
		}

		Iterator advance_n_step(Iterator it, difference_type n,
			bool right, bidirectional_iterator_tag)
		{
			difference_type abs_n = n >= 0 ? n : -n;
			if ((right && n > 0) || (!right && n < 0))
			{
				for (difference_type i = 0;i != abs_n;++i)
				{
					it += 1;
				}
			}
			else if ((!right && n > 0) || (right && n < 0))
			{
				for (difference_type i = 0;i != abs_n;++i)
				{
					it -= 1;
				}
			}
			return it;
		}

	public:
		template <typename Iterator>
		friend bool operator==(const reverse_iterator_t<Iterator> &lhs,
			const reverse_iterator_t<Iterator> &rhs);
		template <typename Iterator>
		friend bool operator!=(const reverse_iterator_t<Iterator> &lhs,
			const reverse_iterator_t<Iterator> &rhs);
		template <typename Iterator>
		friend bool operator<(const reverse_iterator_t<Iterator> &lhs,
			const reverse_iterator_t<Iterator> &rhs);
		template <typename Iterator>
		friend bool operator<=(const reverse_iterator_t<Iterator> &lhs,
			const reverse_iterator_t<Iterator> &rhs);
		template <typename Iterator>
		friend bool operator>(const reverse_iterator_t<Iterator> &lhs,
			const reverse_iterator_t<Iterator> &rhs);
		template <typename Iterator>
		friend bool operator>=(const reverse_iterator_t<Iterator> &lhs,
			const reverse_iterator_t<Iterator> &rhs);

		template <typename Iterator>
		friend reverse_iterator_t<Iterator> operator+(
			typename reverse_iterator_t<Iterator>::difference_type n,
			const reverse_iterator_t<Iterator> &rev_it);
		template <typename Iterator>
		friend typename reverse_iterator_t<Iterator>::difference_type operator-(
			const reverse_iterator_t<Iterator> &lhs,
			const reverse_iterator_t<Iterator> &rhs);
	};// end

	template <typename Iterator>
	reverse_iterator_t<Iterator> &reverse_iterator_t<Iterator>::operator+=(difference_type n)
	{
		_base = advance_n_step(_base, n, false, iterator_category());
		_cur = advance_n_step(_cur, n, false, iterator_category());
		return *this;
	}

	template <typename Iterator>
	reverse_iterator_t<Iterator> &reverse_iterator_t<Iterator>::operator-=(difference_type n)
	{
		_base = advance_n_step(_base, n, true, iterator_category());
		_cur = advance_n_step(_cur, n, true, iterator_category());
		return *this;
	}

	template <typename Iterator>
	reverse_iterator_t<Iterator> reverse_iterator_t<Iterator>::operator+(difference_type n) const
	{
		auto res = *this;
		res += n;
		return res;
	}

	template <typename Iterator>
	reverse_iterator_t<Iterator> reverse_iterator_t<Iterator>::operator-(difference_type n) const
	{
		auto res = *this;
		res -= n;
		return res;
	}

	template <typename Iterator>
	bool operator==(const reverse_iterator_t<Iterator> &lhs,
		const reverse_iterator_t<Iterator> &rhs)
	{
		return lhs._cur == rhs._cur;
	}

	template <typename Iterator>
	bool operator!=(const reverse_iterator_t<Iterator> &lhs,
		const reverse_iterator_t<Iterator> &rhs)
	{
		return !(lhs == rhs);
	}

	template <typename Iterator>
	bool operator<(const reverse_iterator_t<Iterator> &lhs,
		const reverse_iterator_t<Iterator> &rhs)
	{
		return lhs._cur < rhs._cur;
	}

	template <typename Iterator>
	bool operator>(const reverse_iterator_t<Iterator> &lhs,
		const reverse_iterator_t<Iterator> &rhs)
	{
		return lhs._cur > rhs._cur;
	}

	template <typename Iterator>
	bool operator>=(const reverse_iterator_t<Iterator> &lhs,
		const reverse_iterator_t<Iterator> &rhs)
	{
		return !(lhs < rhs);
	}

	template <typename Iterator>
	bool operator<=(const reverse_iterator_t<Iterator> &lhs,
		const reverse_iterator_t<Iterator> &rhs)
	{
		return !(lhs < rhs);
	}

	template <typename Iterator>
	reverse_iterator_t<Iterator> operator+(
		typename reverse_iterator_t<Iterator>::difference_type n,
		const reverse_iterator_t<Iterator> &rev_it)
	{
		return rev_it + n;
	}

	template <typename Iterator>
	typename reverse_iterator_t<Iterator>::difference_type operator-(
		const reverse_iterator_t<Iterator> &lhs,
		const reverse_iterator_t<Iterator> &rhs)
	{
		return lhs._cur - rhs._cur;
	}
}

#endif