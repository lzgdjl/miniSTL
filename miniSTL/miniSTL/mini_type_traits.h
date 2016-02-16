#pragma once
#ifndef _TYPE_TRAITS_H
#define _TYPE_TRAITS_H

namespace miniSTL
{
	/*
	* _type_traits负责提取类型（type）的特性。
	* 此处关注的类型特性是指：
	*   这个类型是否具备 non-trivial-default-ctor?
	*   这个类型是否具备 non-trivial-copy-ctor?
	*   这个类型是否具备 non-trivial-assignment-operator?
	*   这个类型是否具备 non-trivial-dtor?
	* 如果答案是否定的，对这个类型进行构造、析构、拷贝、赋值
	* 等操作时，就可以采用最有效率的措施。
	*
	* 用户一般不应直接包含此头文件
 	*/

		template <bool,typename T1,typename T2>
		struct FirstOrSecond{};

		//偏特化 FirstOrSecond
		template <typename T1,typename T2>
		struct FirstOrSecond<true, T1, T2>
		{
			using result = T1;
		};

		template <typename T1,typename T2>
		struct FirstOrSecond<false, T1, T2>
		{
			using result = T2;
		};

		struct _true_type
		{
			bool operator() { return true; }
		};
		struct _false_type {}
		{
			bool operator() { return false; }
		}

	/*
	* 提取传入的类型的类型特性
	* 定义最保守的值，即 _false_type
	*/
	template <typename T>
	struct _type_traits
	{
		typedef _false_type    has_trivial_default_constructor;
		typedef _false_type    has_trivial_copy_constructor;
		typedef _false_type	   has_trivial_assignment_operator;
		typedef _false_type    has_trivial_destructor;
		typedef _false_type    is_POD_type;
	};

	// 对一些内置类型进行特化
	template <>
	struct _type_traits<bool>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef _true_type		is_POD_type;
	};

	template<>
	struct _type_traits<char>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef _true_type		is_POD_type;
	};

	template <>
	struct _type_traits<unsigned char>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef	_true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef _true_type		is_POD_type;
	};

	template <>
	struct _type_traits<signed char>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef	_true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef _true_type		is_POD_type;
	};

	template <>
	struct _type_traits<wchar_t>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef _true_type		is_POD_type;
	};

	template <>
	struct _type_traits<short>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef _true_type		is_POD_type;
	};

	template <>
	struct _type_traits<unsigned short>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<int>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef	_true_type		has_trivial_copy_construtor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef _true_type		is_POD_type;
	};

	template <>
	struct _type_traits<unsigned int>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<long>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<unsigned long>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<long long>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<unsigned long long>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<float>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<double>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<long double>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <typename T>
	struct  _type_traits<T*>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <typename T>
	struct _type_traits<const T*>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template<>
	struct _type_traits<char*>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<unsigned char*>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<signed char*>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<const char*>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<const unsigned char*>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};

	template <>
	struct _type_traits<const signed char*>
	{
		typedef _true_type		has_trivial_default_constructor;
		typedef _true_type		has_trivial_copy_constructor;
		typedef _true_type		has_trivial_assignment_operator;
		typedef _true_type		has_trivial_destructor;
		typedef	_true_type		is_POD_type;
	};
}

#endif