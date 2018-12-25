#ifndef QUEUEAPI_H
#define QUEUEAPI_H


class CQueueNotify
{
public:
	virtual void OnMessage(const char* pcService, const int ID, const char* pcBuf, const int iLen) = 0;

	/*
	ָ�������ݰ��Ƿ���Ҫ�ŵ������
	0��������У�1�����������
	*/
	virtual int CanPut(const char* pcService, const int ID, const char* pcBuf, const int iLen) = 0;
};


class CQueueApi
{
public:

	virtual void SetQueueNotify(CQueueNotify* pclNotify) = 0;

public:
	/*
	���յ�����Ϣ��Ͷ��ʧ����Ϣ�������
	0������ɹ���1������ʧ��

	flag��FLAG_MESSAGE �յ�����Ϣ
	flag��FLAG_RETURNED Ͷ��ʧ����Ϣ
	*/
	virtual int PutToQueue(const char* pcService, const int ID, const char* pcBuf, const int iLen) = 0;
};

////#ifdef _WIN32
////extern "C"
////{
////	__declspec(dllexport) CQueueApi* CreateQueueApi(void);
////	__declspec(dllexport) void DestroyQueueApi(CQueueApi* pclQueueAPI);
////}
////#endif
//
//#ifdef __linux__
//extern "C"
//{
//	CQueueApi* CreateQueueApi(void);
//	void DestroyQueueApi(CQueueApi* pclAPI);
//}
//#endif


#endif /* QUEUEAPI_H */