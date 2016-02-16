/*
* ���ļ�ʵ�� �ռ������� mini_alloc
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
			// ������������С��ֱ�ӵ��� malloc()
			return malloc(_round_up(bytes));
		}

		size_t index = _FREELIST_INDEX(bytes);
		obj *list = _free_lists[index];
		if (list == 0)
		{
			// û�ҵ����õ� free-list��׼���������free-list
			void *result = _refill(_round_up(bytes));
			return result;
		}

		//���� free list
		_free_lists[index] = list->next;
		return list;
	}

	// ptr �������ǿ�ָ��
	void _alloc::deallocate(void *ptr, size_t bytes)
	{
		if (bytes > static_cast<size_t>(_EMAXBYTES::MAXBYTES))
		{
			// ֱ���ͷ��ڴ�
			free(ptr);
		}
		else
		{
			// ����free list,�ջ�����
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
	* ����һ����СΪ n �Ķ��󣬲�����ʱ���Ϊ�ʵ��� free list���ӽڵ�
	* ���� bytes �Ѿ��ʵ��ϵ��� 8 �ı���
	*/
	void *_alloc::_refill(size_t bytes)
	{
		int nobjs = _ENOBJS::NOBJS;
		
		/*
		* ���� _chunk_alloc(),����ȡ�� nobjs ��������Ϊ free-list���½ڵ�
		* ע����� nobjs �Ǵ�������
		*/
		char *chunk = _chunk_alloc(bytes, nobjs);
		obj **my_free_list = 0;
		obj *result = 0;
		obj *current_obj = 0, *next_obj = 0;

		// ���ֻ���һ�����飬�������ͷ�����������ã�free list���½ڵ�
		if (1 == nobjs)
			return chunk;

		// ����׼������ free list������½ڵ�
		my_free_list = _free_lists + _FREELIST_INDEX(bytes);
		// ������ chunk �ռ��ڽ���free list
		result = reinterpret_cast<obj *>(chunk); // ��һ��׼�����ظ�������
		*my_free_list = next_obj = reinterpret_cast<obj*>(chunk + bytes);
		// ���½� free list �ĸ��ڵ㴮������
		// ��1��ʼ����Ϊ��0�����ظ�������
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
	* ���� bytes �Ѿ��ϵ��� free listʹ�ã��� _chunk_alloc() �Ĺ���
	* ע����� nobjs �Ǵ�������
	*/
	char *_alloc::_chunk_alloc(size_t bytes, int &nobjs)
	{
		char *result = 0;
		size_t total_bytes = bytes * nobjs;
		size_t bytes_left = _end_free - _start_free;// �ڴ��ʣ��ռ�

		if (bytes_left >= total_bytes)
		{
			// �ڴ��ʣ��ռ���ȫ��������
			result = _start_free;
			_start_free += total_bytes;
			return result;
		}
		else if (bytes_left >= bytes)
		{
			// �ڴ��ʣ��ռ䲻����ȫ���������������㹻��Ӧһ������)���ϵ�����
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = _start_free;
			_start_free += total_bytes;
			return result;
		}
		else
		{
			// �ڴ��ʣ��ռ���һ������Ĵ�С���޷��ṩ
			size_t bytes_to_get = 2 * total_bytes + _round_up(_heap_size >> 4);
			// �����������ڴ���еĲ�����ͷ�������ü�ֵ
			if (bytes_left > 0)
			{
				/*
				* �ڴ�ػ���һЩ��ͷ������Ӹ��ʵ��� free list
				* ����Ѱ���ʵ��� free list
				*/
				obj **my_free_list = _free_lists + _FREELIST_INDEX(bytes_left);
				(reinterpret_cast<obj*>(_start_free))->next = *my_free_list;
				*my_free_list = reinterpret_cast<obj*>(_start_free);
			}
			
			// ���� heap �ռ䣬���������ڴ��
			_start_free = (char *)malloc(bytes_to_get);
			if (!_start_free)
			{
				// heap �ռ䲻�㣬malloc() ʧ��
				obj **my_free_list = 0, *p = 0;

				/*
				* ���ż������ӵ�еĶ������ⲻ������˺������ǲ����㳢�Է���
				* ��С�����飬��Ϊ���ڶ��̻߳��������׵�������
				* ������Ѱ�ʵ��� free list
				* ��ν�ʵ���ָ������δ�����飬�����鹻��֮ free list
				*/
				for (int i = bytes;i <= _EMAXBYTES::MAXBYTES;i += _EALIGN::ALIGN)
				{
					my_free_list = _free_lists + _FREELIST_INDEX(i);
					p = *my_free_list;
					if (p)
					{
						// free list ����δ������
						// ���� free list ���ͷ�δ������
						*my_free_list = p->next;
						_start_free = reinterpret_cast<char*>(p);
						_end_free = _start_free + i;
						// �ݹ�����Լ���Ϊ������ nobjs
						return _chunk_alloc(bytes, nobjs);
						// ע�⣬�κβ�����ͷ�ս��������ʵ��� free list �б���
					}
				}

				// ������û�ڴ������,�׳��쳣
				_end_free = 0;
				throw;
			}
			// �����ڴ�سɹ�
			_heap_size += bytes_to_get;
			_end_free = _start_free + bytes_to_get;
			// �ݹ�����Լ���Ϊ������ nobjs
			return _chunk_alloc(bytes, nobjs);
		}
	}
}