#pragma once

struct sUserInfo 
{
	char UserNo[21];
	char UserName[21];
	sUserInfo()
	{
		memset(this, 0, sizeof(sUserInfo));
	}
};
class CMouseComboBox:public CxSubClassWnd
{
public:
	CMouseComboBox();
	~CMouseComboBox();
	void Init(HWND hComboBox);
public:
	//?
	bool m_bAutoComplete;
	//�Ƿ��β��ƥ������
	bool m_bReverFind;
	//
	bool m_bEditHeightSet;
	//ѡ����
	int	m_iSelectedRow;
	// ���ı�
	vector<sUserInfo>	m_vUserInfo;

	// ÿ�е�ĩβ
	int m_vecEndPixels[2];     
	//edit
	CMouseComboEdit m_Edit;
	
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	//�Ƿ���Ҫ��ֹ�������л��˻� ֻ��droplist�رյ�ʱ��Ž�ֹ
	bool IsNeedForbidMosueWheel();
	//���ô�β������
	inline void SetReverFind(bool bReverFind)  { m_bReverFind = bReverFind; }
	//��ȡ�Ƿ��β������
	inline bool GetReverFind()					{ return m_bReverFind; }
	//���
	void ResetContent();

	//����
	int  AddRow(const sUserInfo& sInfo);
	//��ȡѡ����
	int  GetSelectedRow();
	//��ȡ�ı�
	void GetLBText(int nIndex, wstring& rString) const;
	//����
	void ResetSelection();
public:
	//����ߴ�
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	//���»���
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	//����
	void OnCbnEditupdate();
private:
	bool LineInStringLeft(const string& rsLine, const string& rsString) const;
	bool LineInStringRight(const string& rsLine, const string& rsString) const;
	int  FindUniqueMatchedItem(const string &line, string &sMatchedLine);
};

