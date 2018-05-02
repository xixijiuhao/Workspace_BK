#pragma once

#include <vector>
using namespace std;

//TradeType
#define ttRatio			'R'   //"����"�µ�ʱ�����׼�˻����µ�������ϵͳ�����ڿͻ�����ڻ�׼�ͻ����µ�����������ÿͻ����µ���������һ�ּ�����
#define ttSplitRatio	'r'	  //"�����ֲ�"�µ�ʱ�����µ�������ϵͳ�����ڿͻ���������µ����ı���������ÿͻ����µ���������һ�ּ�����
#define ttSplitRation	'n'	  //"����ֲ�"�µ�ʱ�����µ�������ϵͳ�����ڿͻ��Ķ���������������һ�������û�ﵽ���µ�����������ڶ��֣�ֱ���ﵽ���µ���
#define ttMoneyRatio	'M'	  //�ʽ����"�µ�ʱ�����׼�˻����µ�������ϵͳ�����ڿͻ�����ڻ�׼�ͻ����ʽ����������ÿͻ����µ���������һ�ּ�����


//������
struct TBatchGroup
{
	char GroupNo[11];	//���
	char Name[21];		//����
	char TradeType;		//���ͣ���׼����������ʣ�ࣩ
	char BaseUserNo[21];//���ڻ�׼�ͻ�
	char Sign[21];		//���ڻ�׼�ͻ�sign
public:
	TBatchGroup(){ memset(this,0,sizeof(*this)); }
};

//�������ڿͻ�
struct TBatchUser
{
	char UserNo[21];	//�ͻ����
	char LoginUser[21]; //��½�˺�
	char Password[21];	//��½����
	char Sign[21];      //sign
	int  TradeRatio;	//�������� ����
public:
	TBatchUser(){  memset(this,0,sizeof(*this)); }
};


class BatchGroup: public TBatchGroup
{
private:	
	typedef vector<TBatchUser> vctUser;

	int		m_BaseInd;
	int		m_TotalRatio;	//�ͻ��µ������ϼƣ�TradeType==ttSplitRatioʱ��Ч
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