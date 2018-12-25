#ifndef QUEUEAPI_H
#define QUEUEAPI_H


class CQueueNotify
{
public:
	virtual void OnMessage(const char* pcService, const int ID, const char* pcBuf, const int iLen) = 0;

	/*
	指定该数据包是否需要放到队列里。
	0：放入队列；1：不放入队列
	*/
	virtual int CanPut(const char* pcService, const int ID, const char* pcBuf, const int iLen) = 0;
};


class CQueueApi
{
public:

	virtual void SetQueueNotify(CQueueNotify* pclNotify) = 0;

public:
	/*
	将收到的消息、投递失败消息放入队列
	0：放入成功；1：放入失败

	flag：FLAG_MESSAGE 收到的消息
	flag：FLAG_RETURNED 投递失败消息
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