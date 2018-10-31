#include <iostream>

#define getNodeHeadPTR(a) (((unsigned long)(a))|0x6000000000000000)
#define getMultiPTR(a) (((unsigned long)(a))|0x8000000000000000)
#define notMultiPTR(a) (!((((unsigned long)(a)) & 0xFFFF000000000000) ==  0x8000000000000000))
#define truePtrValue(a) (((unsigned long)(a))&0x00007FFFFFFFFFFF)

extern long long DPPtrCheckerCount;
extern long long DPPtrProtectCount;
extern long long DPObjectAllocCount;
extern long long DPObjectFreeCount;
extern long long DPPtrTrackerCount;
extern long long DPPtrTrackeredCount;

typedef struct heapNode{
    void *addr;
    struct heapNode *head;
    struct heapNode *next;
}heapNode;
//_LIBCPP_OVERRIDABLE_FUNC_VIS void* operator new(std::size_t __sz) _THROW_BAD_ALLOC
//{
//    cout << "内存分配";
//    return malloc(__sz);
//}

void* newSafeMalloc(size_t size){
    heapNode *ptr = (heapNode *)malloc(sizeof(heapNode));
    ++DPObjectAllocCount;
    if (ptr) {
        ptr->addr = malloc(size);
        ptr->head = ptr;
        ptr->next = NULL;
        return (heapNode *)getMultiPTR(ptr);
    }else{
        return NULL;
    }
}


void newSafeFree(void* ptr){
    if (ptr == NULL || notMultiPTR(((heapNode *)(ptr)))) {
        ++DPObjectFreeCount;
        free(ptr);
        return;
    }
    heapNode *nodeHeadP = (heapNode *)truePtrValue(ptr);
    ++DPObjectFreeCount;
    free((void *)truePtrValue(nodeHeadP->addr));
    nodeHeadP->addr = NULL;
    while (nodeHeadP->next) {
        nodeHeadP = nodeHeadP->next;
        nodeHeadP->addr = NULL;
    }
}



//_LIBCPP_WEAK
//void *
//operator new(std::size_t size) _THROW_BAD_ALLOC
//{
//    if (size == 0)
//        size = 1;
//    void* p;
//    while ((p = newSafeMalloc(size)) == 0)
//    {
//        // If malloc fails and there is a new_handler,
//        // call it to try free up memory.
//        std::new_handler nh = std::get_new_handler();
//        if (nh)
//            nh();
//        else
//#ifndef _LIBCPP_NO_EXCEPTIONS
//            throw std::bad_alloc();
//#else
//        break;
//#endif
//    }
////    std::cout << "内存分配" << p << "   size:" <<  size << std::endl;
//    return p;
//}

_LIBCPP_WEAK
void *
MPnew(std::size_t size) _THROW_BAD_ALLOC
{
    if (size == 0)
        size = 1;
    void* p;
    while ((p = newSafeMalloc(size)) == 0)
    {
        // If malloc fails and there is a new_handler,
        // call it to try free up memory.
        std::new_handler nh = std::get_new_handler();
        if (nh)
            nh();
        else
#ifndef _LIBCPP_NO_EXCEPTIONS
            throw std::bad_alloc();
#else
        break;
#endif
    }
    //    std::cout << "内存分配" << p << "   size:" <<  size << std::endl;
    return p;
}

//_LIBCPP_WEAK
//void*
//operator new(size_t size, const std::nothrow_t&) _NOEXCEPT
//{
//    void* p = 0;
//#ifndef _LIBCPP_NO_EXCEPTIONS
//    try
//    {
//#endif  // _LIBCPP_NO_EXCEPTIONS
//        p = MPnew(size);
//#ifndef _LIBCPP_NO_EXCEPTIONS
//    }
//    catch (...)
//    {
//    }
//#endif  // _LIBCPP_NO_EXCEPTIONS
////    std::cout << "内存分配new(size, std::nothrow_t):" << p << "   size:" <<  size << std::endl;
//    return p;
//}

_LIBCPP_WEAK
void*
MPnewArray(size_t size) _THROW_BAD_ALLOC
{
//    std::cout << "内存分配new[]:" << "   size:" <<  size << std::endl;
    return MPnew(size);
}

//_LIBCPP_WEAK
//void*
//operator new[](size_t size, const std::nothrow_t&) _NOEXCEPT
//{
//    void* p = 0;
//#ifndef _LIBCPP_NO_EXCEPTIONS
//    try
//    {
//#endif  // _LIBCPP_NO_EXCEPTIONS
//        p = operator new[](size);
//#ifndef _LIBCPP_NO_EXCEPTIONS
//    }
//    catch (...)
//    {
//    }
//#endif  // _LIBCPP_NO_EXCEPTIONS
////    std::cout << "内存分配new[](size, std::nothrow_t):" << p << "   size:" <<  size << std::endl;
//    return p;
//}


//delete

_LIBCPP_WEAK
void
operator delete(void* ptr) _NOEXCEPT
{
    if (ptr)
        newSafeFree(ptr);
//    std::cout << "内存释放" << ptr << std::endl;
}

_LIBCPP_WEAK
void
operator delete(void* ptr, const std::nothrow_t&) _NOEXCEPT
{
    operator delete(ptr);
//    std::cout << "内存释放(ptr, nothrow_t)" << ptr << std::endl;
}

_LIBCPP_WEAK
void
operator delete(void* ptr, size_t) _NOEXCEPT
{
    operator delete(ptr);
//    std::cout << "内存释放(ptr, size_t)" << ptr << std::endl;
}

_LIBCPP_WEAK
void
operator delete[] (void* ptr) _NOEXCEPT
{
    operator delete(ptr);
//    std::cout << "内存释放[]" << ptr << std::endl;
}

_LIBCPP_WEAK
void
operator delete[] (void* ptr, const std::nothrow_t&) _NOEXCEPT
{
    operator delete[](ptr);
//    std::cout << "内存释放[](ptr, nothrow_t)" << ptr << std::endl;
}

_LIBCPP_WEAK
void
operator delete[] (void* ptr, size_t) _NOEXCEPT
{
    operator delete[](ptr);
//    std::cout << "内存释放[]]" << ptr << std::endl;
}
