# CPP的内存池

    读一下stl的内存池和nginx的内存池的实现，看看内存池的分配是如何实现的

## 内存池在哪里用？

SGI STL包含了一级空间配置器和二级空间配置器，其中一级空间配置器allocator采用malloc和free来 管理内存，和C++标准库中提供的allocator是一样的，但其二级空间配置器allocator采用了基于freelist 自由链表原理的内存池机制实现内存管理。

多读源码：SGL的STL库

## 空间配置器的相关定义
    template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
    class vector : protected _Vector_base<_Tp, _Alloc>

可以看到，容器的默认空间配置器是__STL_DEFAULT_ALLOCATOR( _Tp)，它是一个宏定义，如下:

    # ifndef __STL_DEFAULT_ALLOCATOR
    #   ifdef __STL_USE_STD_ALLOCATORS
    #     define __STL_DEFAULT_ALLOCATOR(T) allocator< T > // 一级空间配置器内存管理类 -- 通过malloc和free管理内
    #   else
    #     define __STL_DEFAULT_ALLOCATOR(T) alloc // 二级空间配置器内存管理类 -- 通过自定义内存池实现内 存管理
    #   endif
    # endif

从上面可以看到__STL_DEFAULT_ALLOCATOR通过宏控制有两种实现，一种是allocator< T >，另一种 是alloc，这两种分别就是SGI STL的一级空间配置器和二级空间配置器的实现。



### 内存池的定义

```c++
 // 内存池的粒度信息
enum {_ALIGN = 8};
enum {_MAX_BYTES = 128};
enum {_NFREELISTS = 16};

// 每一个内存chunk块的头信息 union _Obj {
};
union _Obj* _M_free_list_link;
char _M_client_data[1];    /* The client sees this. */

// 组织所有自由链表的数组，数组的每一个元素的类型是_Obj*，全部初始化为0 
static _Obj* __STL_VOLATILE _S_free_list[_NFREELISTS];

// Chunk allocation state. 记录内存chunk块的分配情况 static char* _S_start_free;
static char* _S_end_free;
static size_t _S_heap_size;
template <bool __threads, int __inst>
char* __default_alloc_template<__threads, __inst>::_S_start_free = 0;
template <bool __threads, int __inst>
char* __default_alloc_template<__threads, __inst>::_S_end_free = 0;
template <bool __threads, int __inst>
size_t __default_alloc_template<__threads, __inst>::_S_heap_size = 0;
```
**重要的辅助接口函数**

```c++
 /*将 __bytes 上调至最邻近的 8 的倍数*/
 static size_t _S_round_up(size_t __bytes)
    { return (((__bytes) + (size_t) _ALIGN-1) & ~((size_t) _ALIGN - 1)); }

/*返回 __bytes 大小的chunk块位于 free-list 中的编号*/ 
static size_t _S_freelist_index(size_t __bytes) {
        return (((__bytes) + (size_t)_ALIGN-1)/(size_t)_ALIGN - 1); }

```

**内存池管理函数**

```c++
 // 分配内存的入口函数
  /* __n must be > 0      */
  static void* allocate(size_t __n)
  {
    void* __ret = 0;

    if (__n > (size_t) _MAX_BYTES) {
      __ret = malloc_alloc::allocate(__n);
    }
    else {
      _Obj* __STL_VOLATILE* __my_free_list
        // 获取位置
          = _S_free_list + _S_freelist_index(__n);
      // Acquire the lock here with a constructor call.
      // This ensures that it is released in exit or during stack
      // unwinding.
#     ifndef _NOTHREADS
      /*REFERENCED*/
      // 加锁
      _Lock __lock_instance;
#     endif
      _Obj* __RESTRICT __result = *__my_free_list;
      if (__result == 0)
        __ret = _S_refill(_S_round_up(__n));
      else {
        *__my_free_list = __result -> _M_free_list_link;
        __ret = __result;
      }
    }

    return __ret;
  };



// 负责把分配好的chunk块进行连接，添加到自由链表当中  
static void* _S_refill(size_t __n);
// 分配相应内存字节大小的chunk块，并且给下面三个成员变量初始化 static char* _S_chunk_alloc(size_t __size, int& __nobjs);
// 把chunk块归还到内存池
static void deallocate(void* __p, size_t __n);

/* __p may not be 0 */
static void deallocate(void* __p, size_t __n)
{
if (__n > (size_t) _MAX_BYTES)
malloc_alloc::deallocate(__p, __n);
else {
_Obj* __STL_VOLATILE*  __my_free_list
= _S_free_list + _S_freelist_index(__n);
_Obj* __q = (_Obj*)__p;

// acquire lock
#       ifndef _NOTHREADS
/*REFERENCED*/
_Lock __lock_instance;
#       endif /* _NOTHREADS */
__q -> _M_free_list_link = *__my_free_list;
*__my_free_list = __q;
// lock is released here
}
}

static void* reallocate(void* __p, size_t __old_sz, size_t __new_sz);

} ;
// 内存池扩容函数
template <bool threads, int inst>
void*
__default_alloc_template<threads, inst>::reallocate(void* __p,
                                                    size_t __old_sz,
                                                    size_t __new_sz);
```



# my-mem-pool

![language](https://img.shields.io/badge/language-c++-DeepPink.svg) [![GitHub license](https://img.shields.io/github/license/YuzeZhang/my-mem-pool.svg)](https://github.com/YuzeZhang/my-mem-pool/blob/master/LICENSE) [![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)

通过剖析开源代码可以积累优秀的代码设计思想和良好的编程规范，了解不同应用场景下不同的内存池实现也是一种重要的能力，本仓库对SGI STL二级空间配置器源码和nginx内存池源码进行了剖析，并使用C++ OOP进行仿写，使其便于移植到其它项目当中。

本仓库包含以下内容：

1. 注释过的SGI STL二级空间配置器源码以及对其的分析整理
2. 注释过的nginx内存池源码以及对其的分析整理
3. my_stl_allocator
4. my_nginx_mem_pool

## Table of Contents

- [背景](#Background)
- [使用](#Usage)
- [TODO](#TODO)
- [使用许可](#License)

## Background

在学习编程的过程中，一味的闭门造车是不可取的，就和作家为什么要看书一样，积累优秀资源是创造优秀资源的必要条件。

“所谓创意，只是把原有的元素重新组合而已。”

通过对SGI STL二级空间配置器源码的剖析和对nginx内存池源码的剖析来学习内存池的设计，并且体会SGI STL二级空间配置器和nginx中的内存池在设计上的区别，不同的应用场景选择不同的内存池的设计实现。

## TODO

- [ ] ngx_create_pool的代码逻辑可以直接实现在ngx_mem_pool的构造函数中
- [ ] ngx_destory_pool的代码逻辑可以直接实现在ngx_mem_pool的析构函数中

## License

