#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include "QueueAPI.h"
#include "WinTools.h"

class QueueCell
{
public:
	char*       m_pcService;
	int         m_ID;
    char*		m_pcBuf;
    int			m_iLen;
};

class Queue : public TThread
{
public:
   Queue(CQueueNotify* pclNotify);

public:
	virtual int CanPut(const char* pcService, const int ID, const char* pcBuf, const int iLen);
	virtual void PutBuf(const char* pcService, const int ID, const char* pcBuf, const int iLen);

protected:
    virtual void Run();

private:
    CQueueNotify* m_pclNotify;

    TThreadQueue<QueueCell*> m_Queue;
};

class QueueUtils : public CQueueApi
{
public:
    virtual void SetQueueNotify(CQueueNotify* pclNotify);

	virtual int PutToQueue(const char* pcService, const int ID, const char* pcBuf, const int iLen);

private:
    typedef std::vector<Queue*> QueueVectorType;
    QueueVectorType m_QueueVector;
};

#endif