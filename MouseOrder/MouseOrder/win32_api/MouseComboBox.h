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
	//是否从尾端匹配输入
	bool m_bReverFind;
	//
	bool m_bEditHeightSet;
	//选中行
	int	m_iSelectedRow;
	// 行文本
	vector<sUserInfo>	m_vUserInfo;

	// 每列的末尾
	int m_vecEndPixels[2];     
	//edit
	CMouseComboEdit m_Edit;
	
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	//是否需要禁止鼠标滚轮切换账户 只有droplist关闭的时候才禁止
	bool IsNeedForbidMosueWheel();
	//设置从尾端输入
	inline void SetReverFind(bool bReverFind)  { m_bReverFind = bReverFind; }
	//获取是否从尾端输入
	inline bool GetReverFind()					{ return m_bReverFind; }
	//清空
	void ResetContent();

	//增加
	int  AddRow(const sUserInfo& sInfo);
	//获取选择行
	int  GetSelectedRow();
	//获取文本
	void GetLBText(int nIndex, wstring& rString) const;
	//重置
	void ResetSelection();
public:
	//计算尺寸
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	//重新绘制
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	//更新
	void OnCbnEditupdate();
private:
	bool LineInStringLeft(const string& rsLine, const string& rsString) const;
	bool LineInStringRight(const string& rsLine, const string& rsString) const;
	int  FindUniqueMatchedItem(const string &line, string &sMatchedLine);
};

