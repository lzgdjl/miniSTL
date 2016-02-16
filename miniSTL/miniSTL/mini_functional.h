#ifndef _FUNCTIONAL_H
#define _FUNCTIONAL_H

namespace miniSTL
{
	/*
	* 实现两个在容器模板中会大量运用的函数对象模板
	*/

	template <typename T>
	struct less
	{
		typedef T first_argument_type;
		typedef T second_argument_type;
		typedef bool result_type;

		result_type operator()(const first_argument_type &a, const second_argument_type &b)
		{
			return a < b;
		}
	};

	template <typename T>
	struct equal_to
	{
		typedef T first_argument_type;
		typedef T second_argument_type;
		typedef bool result_type;

		result_type operator()(const first_argument_type &a, const second_argument_type &b)
		{
			return a == b;
		}
	};

	template <typename Arg,typename Result>
	struct unary_function
	{
		typedef Arg		argument_type;
		typedef Result	result_type;
	};

	template <typename Arg1,typename Arg2,typename Result>
	struct binary_function
	{
		typedef Arg1	first_argument_type;
		typedef Arg2	second_argument_type;
		typedef Result	result_type;
	};
}

#endif