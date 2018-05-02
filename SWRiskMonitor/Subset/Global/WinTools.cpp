#include "WinTools.h"
#include <cstdlib>

void TBoundedQueue::Buffer::Put(const char* buf, const int len)
{
	memcpy(&Data[size], buf, len);
	size += len;
}

void TBoundedQueue::Buffer::Take(char*& buf, int& len)
{
	buf = &Data[0];
	len = size;
	size = 0;
}

const int MB_BYTES = 1000 * 1000;
TBoundedQueue::TBoundedQueue(int MB) : notEmpty(mutex), notFull(mutex), swapBuffer(NULL)
{
	int buffersize(MB * MB_BYTES);

	currBuffer = (Buffer*)malloc(buffersize);
	memset(currBuffer, 0, buffersize);
	currBuffer->Init(buffersize);

	nextBuffer = (Buffer*)malloc(buffersize);
	memset(nextBuffer, 0, buffersize);
	nextBuffer->Init(buffersize);

	workBuffer = (Buffer*)malloc(buffersize);
	memset(workBuffer, 0, buffersize);
	workBuffer->Init(buffersize);
}

TBoundedQueue::~TBoundedQueue()
{
	delete currBuffer;
	delete nextBuffer;
	delete swapBuffer;
	delete workBuffer;
}

void TBoundedQueue::Put(const char* buf, const int len)
{
	TMutex::Lock lock(mutex);
	while ((!currBuffer->CanPut(len)) && (!nextBuffer))
	{
		notFull.Wait();
	}

	if (currBuffer->CanPut(len))
	{
		currBuffer->Put(buf, len);
	}
	else
	{
		swapBuffer = currBuffer;
		currBuffer = nextBuffer;
		nextBuffer = NULL;
		currBuffer->Put(buf, len);
	}

	notEmpty.Notify();
}

void TBoundedQueue::Take(char*& buf, int& len)
{
	TMutex::Lock lock(mutex);
	while ((!swapBuffer) && (currBuffer->IsEmpty()))
	{
		notEmpty.Wait();
	}

	if (swapBuffer)
	{
		nextBuffer = workBuffer;
		workBuffer = swapBuffer;
		swapBuffer = NULL;
		workBuffer->Take(buf, len);
	}
	else
	{
		swapBuffer = currBuffer;
		currBuffer = workBuffer;
		workBuffer = swapBuffer;
		swapBuffer = NULL;
		workBuffer->Take(buf, len);
	}

	notFull.Notify();
}