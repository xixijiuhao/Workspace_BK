#pragma once

class CThread
{
public:
	CThread();
	~CThread();
public:
	void Start();
	void Close();
	bool GetInitComple();
	void SetInitComple(bool b);
	bool GetRunning();
	void SetRunning(bool b);
	int GetVersion();
	void SetVersion(int i);
	void SetSign(const char* Sign);
	DWORD GetThreadID(){ return m_ThreadID; }
	SContract*GetContractPtr(const string &strcode);
protected:
	static DWORD _stdcall stdThreadProc(LPVOID lpParameter);
	virtual void Run();
	void CreateThread();
	void UpdateMap();
private:
	HANDLE m_hThread;
	DWORD m_ThreadID;
	int m_iversion;
	char m_sign[21];
	typedef map<string, SContract*> mapContract;
	mapContract m_mapContract;
private:
	CMyLock m_lock;
	bool m_bInitComple;
	bool m_bRunning;
};

