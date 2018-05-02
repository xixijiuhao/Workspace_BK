#include "PreInclude.h"


Queue::Queue(CQueueNotify* pclNotify)
{
    m_pclNotify = pclNotify;

    this->Start();
}

int Queue::CanPut(const char* pcService, const int ID, const char* pcBuf, const int iLen)
{
	return m_pclNotify->CanPut(pcService, ID, pcBuf, iLen);
}

void Queue::PutBuf(const char* pcService, const int ID, const char* pcBuf, const int iLen)
{
    QueueCell* pData = new QueueCell;
	//strncpy_s(pData->m_pcService, sizeof(pData->m_pcService), pcService, sizeof(pcService));
	pData->m_ID = ID;
    pData->m_iLen = iLen;
    pData->m_pcBuf = (char*)malloc(iLen);
    memcpy(pData->m_pcBuf, pcBuf, iLen);

    m_Queue.Put(pData);
}

void Queue::Run()
{
    while (!isTerminated())
    {
        QueueCell* pData(NULL);

        pData = m_Queue.Take();

        if (NULL != pData)
        {	
			m_pclNotify->OnMessage(pData->m_pcService, pData->m_ID, pData->m_pcBuf, pData->m_iLen);

            delete pData->m_pcBuf;
            delete pData;
        }
    }
}

void QueueUtils::SetQueueNotify(CQueueNotify* pclNotify)
{
    Queue* pclQueue = new Queue(pclNotify);

    m_QueueVector.push_back(pclQueue);
}

int QueueUtils::PutToQueue(const char* pcService, const int ID, const char* pcBuf, const int iLen)
{
    for(QueueVectorType::iterator iter = m_QueueVector.begin(); iter != m_QueueVector.end(); iter++)
    {
        Queue* pclQueue(*iter);

		if (pclQueue->CanPut(pcService, ID, pcBuf, iLen) == 0)
        {
			pclQueue->PutBuf(pcService, ID, pcBuf, iLen);
        }
    }
    return 1;
}

