#ifndef I_APPFRAMEWORK_H
#define I_APPFRAMEWORK_H

//中介者
class IColleague;
class IMediator
{
public:
	virtual void Send(unsigned int CommandID, const void* content, IColleague* colleague) = 0;
	virtual void Get(unsigned int CommandID, void *& pVoid, IColleague *colleague) = 0;
};

class IColleague
{
public:
	IColleague(IMediator* Mediator)
	{
		m_Mediator = Mediator;
	}
	virtual ~IColleague(){};
protected:
	IMediator *m_Mediator;
};

class IPresenterCallBack
{
public:
	virtual bool ShowMsgBox(int type, char *pszReason = NULL) = 0;
};

//命令模式
class IReceiver;
class ICommand
{
public:
	ICommand(IReceiver* receiver)
	{
		m_receiver = receiver;
	}
	virtual ~ICommand(){};
protected:
	IReceiver* m_receiver;
public:
	virtual void Execute();
};


class IInvoker
{
public:
	void SetCommand(ICommand* command)
	{
		m_CurrentCmd = command;
	}
	void ExecuteCmd()
	{
		m_CurrentCmd->Execute();
	}
private:
	ICommand* m_CurrentCmd;
};

class IReceiver
{
public:
	virtual void Action(){};
};

#endif