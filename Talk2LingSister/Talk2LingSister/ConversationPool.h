
#include"stdafx.h"
//�Ի����е�����

/*
class COneMessage{
public:
	COneMessage(CString&hostID,CString &content);
	CString  hostID;
	CString time;
	CString content;
};*/
class CConversationPool{
public:
	CConversationPool();
	
	void Add(CString&hostID,CString&content);//ʱ���Զ��ӽ�ȥ
protected:
	CList<CString> m_MessageList;
};