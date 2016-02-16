#pragma once
#ifndef _ALLOC_H
#define _ALLOC_H

#include <cstdlib>

namespace miniSTL
{
	/*
	* �Զ���Ŀռ�������
	* Ϊ����Ŀʵ�ֵ��������ṩ�ڴ�������
	* �û�һ�㲻Ӧֱ��ʹ��
	* �ο����ϣ���STLԴ��������by ���
	*/
	class _alloc
	{
	private:
		/*
		* ���涨�弸������ö����
		*/
		enum _EALIGN{ALIGN = 8}; // С��������ϵ��߽�
		enum _EMAXBYTES {MAXBYTES = 128}; // С����������ޣ�������һ��С��������malloc()����
		enum _ENFREELISTS{ NFREELISTS = (_EMAXBYTES::MAXBYTES / _EALIGN::ALIGN) }; // free-list�ĸ���
		enum _ENOBJS {NOBJS = 20}; // Ĭ��ÿ�����ӵĽڵ�������

	private:
		// free-lists�Ľڵ㹹��
		// Ϊ�˽�ʡ�ռ���� union �ṹ
		union obj
		{
			union obj *next;
			char client[1];
		};

		static obj* _free_lists[_ENFREELISTS::NFREELISTS]; // 16 

	private:
		static char *_start_free; // �ڴ����ʼλ��	
		static char *_end_free; // �ڴ�ؽ���λ��
		static size_t _heap_size; // �ڴ�ش�С

	private:
		// �� bytes �ϵ���8�ı���
		static size_t _round_up(size_t bytes)
		{
			return ((bytes + _EALIGN::ALIGN - 1) & ~(_EALIGN::ALIGN - 1));
		}

		//���������С������ʹ�õ�n��free-list, n��0��ʼ����
		static size_t _FREELIST_INDEX(size_t bytes)
		{
			return ((bytes +_EALIGN::ALIGN - 1) / _EALIGN::ALIGN - 1);
		}

		// ����һ����СΪn�Ķ��󣬲����ܼ����СΪn����������
		static void *_refill(size_t n);

		// ����һ���ռ䣬������nobjs����СΪsize������
		// ����޷���������nobjs�����飬���ص�nobjs��ֵ���С
		static char *_chunk_alloc(size_t size, int &nobjs);

	public:
		// �ṩ�������ӿ�
		static void *allocate(size_t bytes);
		static void deallocate(void *ptr, size_t bytes);
		static void *reallocate(void *ptr, size_t old_sz, size_t new_sz);
	};

}

#endif