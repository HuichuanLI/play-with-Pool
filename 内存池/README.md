# CPP的内存池

    读一下stl的内存池和nginx的内存池的实现，看看内存池的分配是如何实现的

## 内存池在哪里用？

SGI STL包含了一级空间配置器和二级空间配置器，其中一级空间配置器allocator采用malloc和free来 管理内存，和C++标准库中提供的allocator是一样的，但其二级空间配置器allocator采用了基于freelist 自由链表原理的内存池机制实现内存管理。

多读源码：SGL的STL库

## 空间配置器的相关定义
    template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
    class vector : protected _Vector_base<_Tp, _Alloc>

可以看到，容器的默认空间配置器是__STL_DEFAULT_ALLOCATOR( _Tp)，它是一个宏定义，如下: