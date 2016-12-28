#include "AMemPool.h"

ATHENA_BEGIN

AMemPool::autoPtr AMemPool::pool;
int AMemPool::nodeNum = 2;
size_t AMemPool::nodeSize = 100 * 1024 * 1024;//100MB

struct AMemPool::memNode
{
	explicit memNode(size_t);
	~memNode();
	void* allocate(size_t s);
	void deallocate(void *);
	size_t size;
	size_t available;
	memNode *next;
	void *block;
	void *currentPos;
	static const size_t leastBlockSize = 20;
	static const size_t infoSize = sizeof(bool) + sizeof(size_t);
};

AMemPool::memNode::memNode(size_t s) :size(s), next(nullptr)
{
	block = malloc(s);
	if (block == nullptr)
	{
		throw ABadAlloc("BadAlloc In AMemPool::memNode::memNode(size_t)\n");
	}
	bool *i = reinterpret_cast<bool *>(block);
	char *temp = reinterpret_cast<char *>(block);
	*i = true;
	i++;
	*reinterpret_cast<size_t *>(i) = s;

	i = reinterpret_cast<bool *>(temp + s - sizeof(bool) - sizeof(size_t));
	*i = true;
	i++;
	*reinterpret_cast<size_t *>(i) = s;

	currentPos = block;

	available = s - 2 * infoSize;
}

AMemPool::memNode::~memNode()
{
	free(block);
	if (next)
	{
		delete next;
	}
}

void *AMemPool::memNode::allocate(size_t s)
{
	char *start = reinterpret_cast<char *>(block);
	bool *i = reinterpret_cast<bool *>(currentPos);
	char *j = reinterpret_cast<char *>(currentPos);
	size_t *temp = reinterpret_cast<size_t *>(i + 1);

	auto getNext = [&start, this](char *ptr, size_t least)->char *
	{
		char *temp = ptr;
		bool *isAva = reinterpret_cast<bool *>(ptr);
		size_t *blockSize = reinterpret_cast<size_t *>(ptr + 1);

		for (; static_cast<size_t>(temp - start)<size;)
		{
			if ((*isAva == true) && (*blockSize>least))
			{
				return temp;
			}
			temp += *blockSize;
			isAva = reinterpret_cast<bool *>(temp);
			blockSize = reinterpret_cast<size_t *>(temp + 1);
		}
		temp = start;
		for (; temp<ptr;)
		{
			if (*isAva == true)
			{
				return temp;
			}
			temp += *blockSize;
			isAva = reinterpret_cast<bool *>(temp);
			blockSize = reinterpret_cast<size_t *>(temp + 1);
		}
		return nullptr;
	};

	auto memHandle = [&, this]()->void *
	{

		if (*temp - s - 2 * infoSize<leastBlockSize)
		{
			*i = false;
			*(i + *temp - infoSize) = false;
			available -= *temp - 2 * infoSize;
			j += *temp;
			currentPos = getNext(j, 0);
			return i+infoSize;
		}

		size_t oldSize = *temp;

		*i = false;
		*reinterpret_cast<size_t *>(i + 1) = s + 2 * infoSize;
		j += s + infoSize;

		*reinterpret_cast<bool *>(j) = false;
		*reinterpret_cast<size_t *>(j + 1) = s + 2 * infoSize;

		j += infoSize;

		*reinterpret_cast<bool *>(j) = true;
		*reinterpret_cast<size_t *>(j + 1) = oldSize - (s + 2 * infoSize);

		*reinterpret_cast<bool *>(i + oldSize - infoSize) = true;
		*reinterpret_cast<size_t *>(i + oldSize - infoSize + 1) = oldSize - (s + 2 * infoSize);

		currentPos = getNext(j, 0);
		available -= s + 2 * infoSize;
		return i+infoSize;
	};

	if (*temp >s + 2 * infoSize)
	{
		return memHandle();
	}

	char *target = getNext(j, s + 2 * infoSize);
	if (target == nullptr)
	{
		return target;
	}
	currentPos = target;
	i = reinterpret_cast<bool *>(currentPos);
	j = reinterpret_cast<char *>(currentPos);
	temp = reinterpret_cast<size_t *>(i + 1);
	return memHandle();
}

void AMemPool::memNode::deallocate(void *ptr)
{
	bool *start = reinterpret_cast<bool *>(block);
	bool *end = start + size;
	bool *target = reinterpret_cast<bool *>(ptr);
	bool *flag = reinterpret_cast<bool *>(ptr)-infoSize;
	size_t tempSize = *reinterpret_cast<size_t *>(flag+1);

	if (*flag != false)
	{
		abort();
	}

	size_t prevSize=0,nextSize=0;

	if (flag != start)
	{
		if (*(flag - infoSize))
		{
			prevSize = *reinterpret_cast<size_t *>(flag - sizeof(size_t));
		}
	}

	if (flag + tempSize != end)
	{
		if (*(flag + tempSize))
		{
			nextSize = *reinterpret_cast<size_t *>(flag + tempSize + sizeof(bool));
		}
	}

	*(flag - prevSize)=true;
	*reinterpret_cast<size_t *>(flag - prevSize + sizeof(bool)) = tempSize + prevSize + nextSize;

	*(flag + tempSize + nextSize - infoSize) = true;
	*reinterpret_cast<size_t *>(flag + nextSize + tempSize - sizeof(size_t)) = tempSize + prevSize + nextSize;

	available +=tempSize;
	if (prevSize)
	{
		available += 2*infoSize;
	}
	if (nextSize)
	{
		available += 2*infoSize;
	}

	if (currentPos == (flag + tempSize))
	{
		currentPos = flag - prevSize;
	}

}


AMemPool::AMemPool() :currentNodeNum(nodeNum)
{
	memNode *p, *q;
	p = new memNode(nodeSize);
	firstNode = p;
	q = p;
	for (int i = 1; i < currentNodeNum; i++)
	{
		p = new memNode(nodeSize);
		q->next = p;
		q = p;
	}

	lastNode = q;
	currentNode = firstNode;
}

void * AMemPool::allocate(size_t s)
{
	void *result;

	if (s > nodeSize- 2 * (currentNode->infoSize))
	{
		return nullptr;
	}

	auto getNext = [this](memNode *node)->memNode *
	{
		if (node == lastNode)
		{
			return firstNode;
		}
		return node->next;
	};

	memNode *temp = currentNode;
	for (;;)
	{
		temp = getNext(temp);
		if (temp->available >= s + 2 * (currentNode->infoSize))
		{
			currentNode = getNext(currentNode);
			result = temp->allocate(s);
			if (result != nullptr)
				break;
		}
		if (temp == currentNode)
		{
			lastNode->next = new memNode(nodeSize);
			lastNode = lastNode->next;
			result = lastNode->allocate(s);
			currentNode = lastNode;
			currentNodeNum++;
			break;
		}

	}

	return result;
}

bool AMemPool::deallocate(void *ptr)
{
	memNode *temp=firstNode;
	if (ptr == nullptr)
	{
		return false;
	}
	for (;temp!=nullptr;temp=temp->next)
	{
		if (ptr >= temp->block&&ptr < (reinterpret_cast<char *>(temp->block) + temp->size))
		{
			temp->deallocate(ptr);
			return true;
		}
	}
	return false;
}

AMemPool::~AMemPool()
{
	delete firstNode;
}

AMemPool &AMemPool::getMemPool()
{
	if (pool.p == nullptr)
	{
		pool.p=new AMemPool;
	}
	return *pool.p;
}

bool AMemPool::iniPool(int n, size_t s)
{
	if (pool.p != nullptr)
	{
		return false;
	}
	if (n < 0)
	{
		return false;
	}
	nodeNum = n;
	nodeSize = s;
	return true;
}

poolPtr::poolPtr(size_t n):pool(AMemPool::getMemPool())
{
	if (n == 0)
	{
		ptr = nullptr;
	}
	else
	{
		ptr = pool.allocate(n);
	}
}

poolPtr::~poolPtr()
{
	pool.deallocate(ptr);
}

void poolPtr::reset(size_t n)
{
	pool.deallocate(ptr);
	if (n == 0)
	{
		ptr = nullptr;
	}
	else
	{
		ptr = pool.allocate(n);
	}
}

void *poolPtr::get()
{
	return ptr;
}

ATHENA_END
