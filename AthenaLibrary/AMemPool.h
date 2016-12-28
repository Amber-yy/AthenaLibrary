/*

Yuyue's Classes Library For Microsoft Visual C++

Copyright @2016 CDU INnoVation Studio
All rights reserved. 

文件名称：AMemPool.h
摘要：
1.本文件含有内存池类的声明，实现在AMemPool.cpp中。
2.内存池是对内存分配函数的缓冲，在经常需要内存分配/回收的场景使用内存池，会显著提升运行效率。
3.内存池的实例对象应该尽量少，所以内存池类使用了单例模式来设计。
4.提供了绑定到内存池对象的分配子，使STL容器可以直接从内存池中获取内存。
5.提供了两种智能指针，用于提供自动内存回收的功能。

当前版本：V2.0
作者：余悦
e-mail：1397011447@qq.com
创建日期：2016年08月06日
更新日期：2016年08月15日

修正日志：
V2.0，增加了两个智能指针

*/

#pragma once

#include "ACore.h"
#include <cstdlib>

ATHENA_BEGIN

class AMemPool final
{
public:
	AMemPool(const AMemPool&) = delete;
	AMemPool& operator=(const AMemPool&) = delete;
	void *allocate(size_t);
	bool deallocate(void *);
	static bool iniPool(int, size_t);
	static AMemPool & getMemPool();
private:
	AMemPool();
	~AMemPool();
	struct memNode;
	struct autoPtr
	{
		AMemPool *p=nullptr;
		~autoPtr(){ delete p;}
	};
	static autoPtr pool;
	static size_t nodeSize;
	static int nodeNum;
	memNode *currentNode;
	memNode *lastNode;
	memNode *firstNode;
	int currentNodeNum;
};

template<class T>
class APoolAllocator
{
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	template<class U>
	struct rebind
	{
		typedef APoolAllocator<U> other;
	};
public:
	APoolAllocator():pool(AMemPool::getMemPool()){}

	APoolAllocator(const APoolAllocator<T>&) :pool(AMemPool::getMemPool()) {}

	template<class U>
	APoolAllocator(const APoolAllocator<U>&) : pool(AMemPool::getMemPool()) {}

	template<class U>
	APoolAllocator<U>& operator=(const APoolAllocator<U>&)
	{	
		return (*this);
	}

	pointer address(reference r) const
	{
		return &r;
	}
	const_pointer address(const_reference r) const
	{
		return &r;
	}
	pointer allocate(size_type n, const_pointer = 0) 
	{
		void* p = pool.allocate(n*sizeof(value_type));
		if (!p)
		{
			throw ABadAlloc("BadAlloc In APoolAllocator::allocate(size_type, const_pointer)\n");
		}
		return reinterpret_cast<pointer>(p);
	}
	void deallocate(pointer p, size_type) 
	{
		pool.deallocate(p);
	}
	size_type max_size() const 
	{
		return static_cast<size_type>(-1) / sizeof(value_type);
	}
	void construct(pointer p, const value_type& x) 
	{
		new(p) value_type(x);
	}
	void destroy(pointer p) 
	{ 
		p->~value_type(); 
	}
	AMemPool &pool;
};

class poolPtr
{
public:
	poolPtr(const poolPtr &) = delete;
	poolPtr(poolPtr &&) = delete;
	poolPtr& operator=(const poolPtr &) = delete;
	explicit poolPtr(size_t);
	virtual ~poolPtr();
	void reset(size_t);
	void *get();
protected:
	void *ptr;
	AMemPool &pool;
};

template<class T>
class objectPtr :protected poolPtr
{
public:
	objectPtr() :poolPtr(sizeof(T))
	{
		new(ptr) T;
	}
	objectPtr(const objectPtr &) = delete;
	virtual ~objectPtr()
	{
		get()->~T();
	}
	T *get()
	{
		return reinterpret_cast<T *>(ptr);
	}
	T *operator->()
	{
		return reinterpret_cast<T *>(ptr);
	}
	T &operator*()
	{
		return *reinterpret_cast<T *>(ptr);
	}
};

ATHENA_END
