#pragma once
#ifndef _TYPE_TRAITS_H
#define _TYPE_TRAITS_H

namespace miniSTL
{
	/*
	* _type_traits������ȡ���ͣ�type�������ԡ�
	* �˴���ע������������ָ��
	*   ��������Ƿ�߱� non-trivial-default-ctor?
	*   ��������Ƿ�߱� non-trivial-copy-ctor?
	*   ��������Ƿ�߱� non-trivial-assignment-operator?
	*   ��������Ƿ�߱� non-trivial-dtor?
	* ������Ƿ񶨵ģ���������ͽ��й��졢��������������ֵ
	* �Ȳ���ʱ���Ϳ��Բ�������Ч�ʵĴ�ʩ��
	*
	* �û�һ�㲻Ӧֱ�Ӱ�����ͷ�ļ�
 	*/

		template <bool,typename T1,typename T2>
		struct FirstOrSecond{};

		//ƫ�ػ� FirstOrSecond
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
	* ��ȡ��������͵���������
	* ������ص�ֵ���� _false_type
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

	// ��һЩ�������ͽ����ػ�
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