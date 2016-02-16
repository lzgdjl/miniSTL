/*
* 本文件实现 空间配置器 mini_alloc
*/
#include "mini_alloc.h"

namespace miniSTL
{
	char *_alloc::_start_free = nullptr;
	char *_alloc::_end_free = nullptr;
	size_t _alloc::_heap_size = 0;
	_alloc::obj *_alloc::_free_lists[_alloc::_ENFREELISTS::NFREELISTS] =
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	void *_alloc::allocate(size_t bytes)
	{
		if (bytes > static_cast<size_t>(_EMAXBYTES::MAXBYTES))
		{
			// 超出最大区块大小，直接调用 malloc()
			return malloc(_round_up(bytes));
		}

		size_t index = _FREELIST_INDEX(bytes);
		obj *list = _free_lists[index];
		if (list == 0)
		{
			// 没找到可用的 free-list，准备重新填充free-list
			void *result = _refill(_round_up(bytes));
			return result;
		}

		//调整 free list
		_free_lists[index] = list->next;
		return list;
	}

	// ptr 不可以是空指针
	void _alloc::deallocate(void *ptr, size_t bytes)
	{
		if (bytes > static_cast<size_t>(_EMAXBYTES::MAXBYTES))
		{
			// 直接释放内存
			free(ptr);
		}
		else
		{
			// 调整free list,收回区块
			size_t index = _FREELIST_INDEX(bytes);
			obj *node = static_cast<obj*>(ptr);
			node->next = _free_lists[index];
			_free_lists[index] = node;
		}
	}

	void *_alloc::reallocate(void *ptr, size_t old_sz, size_t new_sz)
	{
		deallocate(ptr, old_sz);
		void *result = allocate(new_sz);

		return result;
	}

	/*
	* 返回一个大小为 n 的对象，并且有时候会为适当的 free list增加节点
	* 假设 bytes 已经适当上调至 8 的倍数
	*/
	void *_alloc::_refill(size_t bytes)
	{
		int nobjs = _ENOBJS::NOBJS;
		
		/*
		* 调用 _chunk_alloc(),尝试取得 nobjs 个区块作为 free-list的新节点
		* 注意参数 nobjs 是传递引用
		*/
		char *chunk = _chunk_alloc(bytes, nobjs);
		obj **my_free_list = 0;
		obj *result = 0;
		obj *current_obj = 0, *next_obj = 0;

		// 如果只获得一个区块，这个区块就分配给调用者用，free list无新节点
		if (1 == nobjs)
			return chunk;

		// 否则准备调整 free list，添加新节点
		my_free_list = _free_lists + _FREELIST_INDEX(bytes);
		// 以下在 chunk 空间内建立free list
		result = reinterpret_cast<obj *>(chunk); // 这一块准备返回给调用者
		*my_free_list = next_obj = reinterpret_cast<obj*>(chunk + bytes);
		// 以下将 free list 的各节点串接起来
		// 从1开始，因为第0个返回给调用者
		for (int i = 1;;++i)
		{
			current_obj = next_obj;
			next_obj = reinterpret_cast<obj*>(reinterpret_cast<char*>(next_obj) + bytes);
			if (nobjs - 1 == i)
			{
				current_obj->next = 0;
				break;
			}
			else
			{
				current_obj->next = next_obj;
			}
		}
		return result;
	}

	/*
	* 假设 bytes 已经上调给 free list使用，是 _chunk_alloc() 的工作
	* 注意参数 nobjs 是传递引用
	*/
	char *_alloc::_chunk_alloc(size_t bytes, int &nobjs)
	{
		char *result = 0;
		size_t total_bytes = bytes * nobjs;
		size_t bytes_left = _end_free - _start_free;// 内存池剩余空间

		if (bytes_left >= total_bytes)
		{
			// 内存池剩余空间完全满足需求
			result = _start_free;
			_start_free += total_bytes;
			return result;
		}
		else if (bytes_left >= bytes)
		{
			// 内存池剩余空间不能完全满足需求量，但足够供应一个（含)以上的区块
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = _start_free;
			_start_free += total_bytes;
			return result;
		}
		else
		{
			// 内存池剩余空间连一个区块的大小都无法提供
			size_t bytes_to_get = 2 * total_bytes + _round_up(_heap_size >> 4);
			// 以下试着让内存池中的残余零头还有利用价值
			if (bytes_left > 0)
			{
				/*
				* 内存池还有一些零头，先添加给适当的 free list
				* 首先寻找适当的 free list
				*/
				obj **my_free_list = _free_lists + _FREELIST_INDEX(bytes_left);
				(reinterpret_cast<obj*>(_start_free))->next = *my_free_list;
				*my_free_list = reinterpret_cast<obj*>(_start_free);
			}
			
			// 分配 heap 空间，用来补充内存池
			_start_free = (char *)malloc(bytes_to_get);
			if (!_start_free)
			{
				// heap 空间不足，malloc() 失败
				obj **my_free_list = 0, *p = 0;

				/*
				* 试着检查手上拥有的东西。这不会造成伤害。我们不打算尝试分配
				* 较小的区块，因为拿在多线程机器上容易导致灾难
				* 以下搜寻适当的 free list
				* 所谓适当是指“尚有未用区块，且区块够大”之 free list
				*/
				for (int i = bytes;i <= _EMAXBYTES::MAXBYTES;i += _EALIGN::ALIGN)
				{
					my_free_list = _free_lists + _FREELIST_INDEX(i);
					p = *my_free_list;
					if (p)
					{
						// free list 内有未用区块
						// 调整 free list 以释放未用区块
						*my_free_list = p->next;
						_start_free = reinterpret_cast<char*>(p);
						_end_free = _start_free + i;
						// 递归调用自己，为了修正 nobjs
						return _chunk_alloc(bytes, nobjs);
						// 注意，任何残余零头终将被编入适当的 free list 中备用
					}
				}

				// 到处都没内存可用了,抛出异常
				_end_free = 0;
				throw;
			}
			// 增加内存池成功
			_heap_size += bytes_to_get;
			_end_free = _start_free + bytes_to_get;
			// 递归调用自己，为了修正 nobjs
			return _chunk_alloc(bytes, nobjs);
		}
	}
}