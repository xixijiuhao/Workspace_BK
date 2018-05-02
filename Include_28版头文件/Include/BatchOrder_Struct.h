#pragma once

#include <vector>
using namespace std;

//TradeType
#define ttRatio			'R'   //"比例"下单时填入基准账户的下单数量，系统按组内客户相对于基准客户的下单比例，计算该客户的下单量，发完一轮即结束
#define ttSplitRatio	'r'	  //"比例分拆"下单时填入下单总数，系统按组内客户相对于总下单量的比例，计算该客户的下单量，发完一轮即结束
#define ttSplitRation	'n'	  //"定额分拆"下单时填入下单总数，系统按组内客户的定单量发单，发完一轮如果还没达到总下单量，则继续第二轮，直到达到总下单量
#define ttMoneyRatio	'M'	  //资金比例"下单时填入基准账户的下单数量，系统按组内客户相对于基准客户的资金比例，计算该客户的下单量，发完一轮即结束


//批量组
struct TBatchGroup
{
	char GroupNo[11];	//组号
	char Name[21];		//组名
	char TradeType;		//类型（基准比例、比例剩余）
	char BaseUserNo[21];//组内基准客户
	char Sign[21];		//组内基准客户sign
public:
	TBatchGroup(){ memset(this,0,sizeof(*this)); }
};

//批量组内客户
struct TBatchUser
{
	char UserNo[21];	//客户编号
	char LoginUser[21]; //登陆账号
	char Password[21];	//登陆密码
	char Sign[21];      //sign
	int  TradeRatio;	//交易数量 比例
public:
	TBatchUser(){  memset(this,0,sizeof(*this)); }
};


class BatchGroup: public TBatchGroup
{
private:	
	typedef vector<TBatchUser> vctUser;

	int		m_BaseInd;
	int		m_TotalRatio;	//客户下单比例合计，TradeType==ttSplitRatio时有效
	vctUser	m_Users;

	friend class BatchOrder; 
public:
	BatchGroup(): m_TotalRatio(0), m_BaseInd(-1){}
	~BatchGroup()
	{	
		m_Users.clear(); 
	}
	BatchGroup& operator=(BatchGroup& BatchGroup)
	{
		Init();
		memcpy_s(this, sizeof(TBatchGroup), &BatchGroup, sizeof(TBatchGroup));
		for (size_t i =0; i<BatchGroup.m_Users.size(); i++)
		{
			m_Users.push_back(BatchGroup.m_Users[i]);
			m_TotalRatio += m_Users[i].TradeRatio;
		}
		SetBaseUser(BaseUserNo);
		return *this;
	}
	void GetUser(int i, TBatchUser & User) const
	{
		User = m_Users[i];
	}
	void SetUser(int i, TBatchUser & User)
	{
		m_Users[i] = User;
	}
	void GetAllUser(vector<TBatchUser> & Users) const
	{
		Users.clear();
		for (size_t i=0; i<m_Users.size(); i++)
			Users.push_back(m_Users[i]);
	}
	void AddUser(TBatchUser & User)
	{
		for (size_t i=0; i<m_Users.size(); i++)
			if (strcmp(User.UserNo, m_Users[i].UserNo)==0)
			{
				m_TotalRatio += (User.TradeRatio - m_Users[i].TradeRatio);
				m_Users[i] = User;
				return;
			}
			m_Users.push_back(User);
			m_TotalRatio += User.TradeRatio;
	}
	void DelUser(TBatchUser & User)
	{
		for (size_t i=0; i<m_Users.size(); i++)
			if (strcmp(User.UserNo, m_Users[i].UserNo)==0)
			{	
				m_TotalRatio -= m_Users[i].TradeRatio;
				if (i == m_BaseInd)
					m_BaseInd = -1;
				m_Users.erase(m_Users.begin() + i);		
				return;
			}
	}
	void Init()
	{
		memset((TBatchGroup*)this,0,sizeof(TBatchGroup)); 
		m_TotalRatio = 0;
		m_BaseInd = -1;
		m_Users.clear();
	}
	void Update()
	{		
		m_TotalRatio = 0;
		for (size_t i = 0; i<m_Users.size(); i++)
			m_TotalRatio += m_Users[i].TradeRatio;
		SetBaseUser(BaseUserNo);
	}
	void ClearUser()
	{
		m_Users.clear();
	}
	void SetBaseUser(const char * UserNo)
	{
		if (UserNo[0]=='\0')
			return;

		m_BaseInd = -1;
		for (size_t i=0; i<m_Users.size(); i++)
			if (strcmp(UserNo, m_Users[i].UserNo)==0)
			{
				m_BaseInd = i;
				break;
			}
			if (m_BaseInd>=0)
			{
				strcpy_s(BaseUserNo, m_Users[m_BaseInd].UserNo);
				strcpy_s(Sign, m_Users[m_BaseInd].Sign);
			}
	}
	void DelBaseUser()
	{
		m_BaseInd = -1;
		memset(BaseUserNo,0,sizeof(BaseUserNo));
	}
	bool IsExistBaseUser()
	{
		return m_BaseInd != -1;
	}
	int GetUserSize()
	{
		return m_Users.size();
	}
	int GetBaseUserIndex()
	{
		return m_BaseInd;
	}
};
typedef vector<BatchGroup> vctGroup;