#pragma once
#ifndef _ALLOC_H
#define _ALLOC_H

#include <cstdlib>

namespace miniSTL
{
	/*
	* 自定义的空间配置器
	* 为本项目实现的容器类提供内存管理服务
	* 用户一般不应直接使用
	* 参考资料：《STL源码剖析》by 侯捷
	*/
	class _alloc
	{
	private:
		/*
		* 下面定义几个辅助枚举类
		*/
		enum _EALIGN{ALIGN = 8}; // 小型区块的上调边界
		enum _EMAXBYTES {MAXBYTES = 128}; // 小型区块的上限，超出这一大小的区块由malloc()分配
		enum _ENFREELISTS{ NFREELISTS = (_EMAXBYTES::MAXBYTES / _EALIGN::ALIGN) }; // free-list的个数
		enum _ENOBJS {NOBJS = 20}; // 默认每次增加的节点区块数

	private:
		// free-lists的节点构造
		// 为了节省空间采用 union 结构
		union obj
		{
			union obj *next;
			char client[1];
		};

		static obj* _free_lists[_ENFREELISTS::NFREELISTS]; // 16 

	private:
		static char *_start_free; // 内存池起始位置	
		static char *_end_free; // 内存池结束位置
		static size_t _heap_size; // 内存池大小

	private:
		// 将 bytes 上调至8的倍数
		static size_t _round_up(size_t bytes)
		{
			return ((bytes + _EALIGN::ALIGN - 1) & ~(_EALIGN::ALIGN - 1));
		}

		//根据区块大小，决定使用第n号free-list, n从0开始计算
		static size_t _FREELIST_INDEX(size_t bytes)
		{
			return ((bytes +_EALIGN::ALIGN - 1) / _EALIGN::ALIGN - 1);
		}

		// 返回一个大小为n的对象，并可能加入大小为n的其他区域
		static void *_refill(size_t n);

		// 分配一大块空间，可容纳nobjs个大小为size的区块
		// 如果无法完整分配nobjs个区块，返回的nobjs的值会减小
		static char *_chunk_alloc(size_t size, int &nobjs);

	public:
		// 提供的三个接口
		static void *allocate(size_t bytes);
		static void deallocate(void *ptr, size_t bytes);
		static void *reallocate(void *ptr, size_t old_sz, size_t new_sz);
	};

}

#endif