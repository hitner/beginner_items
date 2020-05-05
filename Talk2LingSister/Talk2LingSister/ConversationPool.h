
#include"stdafx.h"
//对话池中的数据

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
	
	void Add(CString&hostID,CString&content);//时间自动加进去
protected:
	CList<CString> m_MessageList;
};