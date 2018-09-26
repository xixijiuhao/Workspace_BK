#pragma once

struct C_KeyBoardOrderCfg
{
	bool				bSetDefaultPos;		//Ĭ�Ϲ��
	unsigned int		uiFocusPos;			//Ĭ�Ͻ���λ�ڵڼ���

	bool				bUpDownEqualTab;	//���¼�
	bool				bRightLeftEqualTab;	//���Ҽ�

	unsigned int		uiDoOrderStyle;		//�����µ����	0 ȷ��  1 ����  2 ȷ�� �� �Զ���

	unsigned int		uiKeyBoardFont;		//�����µ������С 0С  1�� 2��
	bool				bShowOrderValidType;//��ʾ��Ч���� �������� ����

	uchar				ucBuy;
	uchar				ucSell;
	uchar				ucOpen;
	uchar				ucCover;
	uchar				ucCoverT;
	bool				bEnterSendOrder;	//Enter����

	int					OutPriceType;		//Ĭ�ϼ۶������
	int					OutPriceTick;		//Ĭ�ϼ۶�۵���
	bool				bStartOrderAssistant;//����׷������
	unsigned int		uiAssistType;		//׷������ 0 ��׷ 1 ֻ׷���� 2 ֻ׷ƽ��
	std::map<std::wstring, std::wstring> mapContractHotkey;//��Լ��ݼ�
	C_KeyBoardOrderCfg()
	{
		memset(this, 0, sizeof(C_KeyBoardOrderCfg)-sizeof(std::map<std::wstring, std::wstring>));
	}
};
struct C_KeyBoardFontInfoCfg 
{
	LOGFONT			lgBtnFont;			//��ť����
	LOGFONT			lgCheckFont;		//CheckBox����
	LOGFONT			lgStaticFont1;		//��̬�ı�����
	LOGFONT			lgStaticFont2;		//״̬��ʾ����
	LOGFONT			lgComboFont;		//�ɱ༭����
	LOGFONT			lgPositionFont;		//�ֲ�����

	int				iLeftMargin;		//�ؼ���߾�
	int				iTopMargin;			//�ؼ����߾�

	int				iHorCtrlInterval;	//ˮƽ�ؼ����
	int				iVerCtrlInterval1;	//��ֱ�ؼ����1 ��̬�ı���ˮƽ�ؼ��ļ��
	int				iVerCtrlInterval2;	//��ֱ�ؼ����2 ˮƽ�ؼ���״̬��ʾ�ļ��
	int				iVerCtrlInverval3;	//��ֱ�ؼ����2 ��ť֮��Ĵ�ֱ���

	COLORREF		rgbBkPosition;		//�ֲֵı���ɫ
	COLORREF		rgbBuy;				//��ͷ�ĳֲ���ɫ
	COLORREF		rgbSell;			//��ͷ�ĳֲ���ɫ

	C_KeyBoardFontInfoCfg()
	{
		memset(this, 0, sizeof(C_KeyBoardFontInfoCfg));
	}
};

struct C_OrderField
{
	int		id;						// �ֶα�ţ���Ӧ��TradeOrderField������� FutureInputArea
	int		width;					// ��ʾ���(������) �Զ���� �������޹�
	int		height;					// ��ʾ���(������) �Զ���� �������޹�
	bool	bShow;					// �Ƿ���ʾ
	C_OrderField()
	{
		memset(this, 0, sizeof(C_OrderField));
	}
};

struct C_KeyBoardSeqCfg
{
	int				nFieldCount;
	C_OrderField	*pOrderField;
	C_KeyBoardSeqCfg()
	{
		memset(this, 0, sizeof(C_KeyBoardSeqCfg));
	}
};

class IKeyOrderCfg
{
public:
	virtual void __cdecl OnKeyBoardCfgChange(const C_KeyBoardOrderCfg* kbcfg) = 0;

	virtual void __cdecl OnKeyBoardCfgFontChange(const C_KeyBoardFontInfoCfg* kbcfg) = 0;

	virtual void __cdecl OnKeyBoardSeqChange(const C_KeyBoardSeqCfg* kbcfg) = 0;
};