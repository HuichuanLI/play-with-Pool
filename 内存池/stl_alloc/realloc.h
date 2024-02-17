//
// Created by lhc456 on 2024/2/17.
//

#ifndef PLAY_WITH_THREAD_POOL_REALLOC_H
#define PLAY_WITH_THREAD_POOL_REALLOC_H


template<bool __threads, int __inst>
char *
__default_alloc_template<__threads, __inst>::_S_chunk_alloc(size_t __size,
                                                            int &__nobjs) {
    char *__result;
    size_t __total_bytes = __size * __nobjs;
    size_t __bytes_left = _S_end_free - _S_start_free;
    // 备用逻辑整个内存
    if (__bytes_left >= __total_bytes) {
        __result = _S_start_free;
        _S_start_free += __total_bytes;
        return (__result);
        // 备用块
    } else if (__bytes_left >= __size) {
        // 修改了申请的内存个数
        __nobjs = (int) (__bytes_left / __size);
        __total_bytes = __size * __nobjs;
        __result = _S_start_free;
        _S_start_free += __total_bytes;
        return (__result);
    } else {

        // 初始化内容
        size_t __bytes_to_get =
                2 * __total_bytes + _S_round_up(_S_heap_size >> 4);
        // Try to make use of the left-over piece.
        if (__bytes_left > 0) {
            _Obj * __STL_VOLATILE * __my_free_list =
                    _S_free_list + _S_freelist_index(__bytes_left);

            ((_Obj *) _S_start_free)->_M_free_list_link = *__my_free_list;
            *__my_free_list = (_Obj *) _S_start_free;
        }
        _S_start_free = (char *) malloc(__bytes_to_get);
        if (0 == _S_start_free) {
            size_t __i;
            _Obj * __STL_VOLATILE * __my_free_list;
            _Obj *__p;
            // Try to make do with what we have.  That can't
            // hurt.  We do not try smaller requests, since that tends
            // to result in disaster on multi-process machines.
            for (__i = __size;
                 __i <= (size_t) _MAX_BYTES;
                 __i += (size_t) _ALIGN) {
                __my_free_list = _S_free_list + _S_freelist_index(__i);
                __p = *__my_free_list;
                if (0 != __p) {
                    *__my_free_list = __p->_M_free_list_link;
                    _S_start_free = (char *) __p;
                    _S_end_free = _S_start_free + __i;
                    return (_S_chunk_alloc(__size, __nobjs));
                    // Any leftover piece will eventually make it to the
                    // right free list.
                }
            }
            _S_end_free = 0;    // In case of exception.
            _S_start_free = (char *) malloc_alloc::allocate(__bytes_to_get);
            // This should either throw an
            // exception or remedy the situation.  Thus we assume it
            // succeeded.
        }
        // 为空的内容
        _S_heap_size += __bytes_to_get;
        _S_end_free = _S_start_free + __bytes_to_get;
        return (_S_chunk_alloc(__size, __nobjs));
    }
}


template<bool __threads, int __inst>
void *
__default_alloc_template<__threads, __inst>::_S_refill(size_t __n) {
    int __nobjs = 20;
    char *__chunk = _S_chunk_alloc(__n, __nobjs);
    _Obj * __STL_VOLATILE * __my_free_list;
    _Obj *__result;
    _Obj *__current_obj;
    _Obj *__next_obj;
    int __i;

    if (1 == __nobjs) return (__chunk);
    __my_free_list = _S_free_list + _S_freelist_index(__n);

    /* Build free list in chunk */
    __result = (_Obj *) __chunk;
    *__my_free_list = __next_obj = (_Obj * )(__chunk + __n);
    for (__i = 1;; __i++) {
        __current_obj = __next_obj;
        __next_obj = (_Obj * )((char *) __next_obj + __n);
        if (__nobjs - 1 == __i) {
            __current_obj->_M_free_list_link = 0;
            break;
        } else {
            __current_obj->_M_free_list_link = __next_obj;
        }
    }
    return (__result);
}

#endif //PLAY_WITH_THREAD_POOL_REALLOC_H
