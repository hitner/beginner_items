#include"ConversationPool.h"


COneMessage::COneMessage(CString&ID,CString &cont){
	//


}

void CConversationPool::Add(CString&hostID,CString&content){
	COneMessage *pMess=new COneMessage(hostID,content);
	m_MessageList.AddTail(*pMess);

}