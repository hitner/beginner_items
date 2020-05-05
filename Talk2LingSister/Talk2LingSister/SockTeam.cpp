
#include"stdafx.h"
#include"TalkSrvrSocket.h"
/*
class CSockTeam{
public:
	CSockTeam();
	~CSockTeam();
	
	//CTalkSrvrSocket *srvrRecv;
	CSocketFile* recvFile;
	CArchive* arIn;
	CArchive*arOut;
};*/
CSockTeam::CSockTeam(){
	//����null
	recvFile=arIn=arOut=NULL;
}

CSockTeam::~CSockTeam(){
	//��null������delete
	if(recvFile) delete recvFile;
	if(arIn) delete arIn;
	if(arOut) delete arOut;
}

void CSockTeam::Create(CSocket*pSrvrRecv){
	recvFile=new CSocketFile(pSrvrRecv);
	arIn=new CArchive(recvFile,CArchive::load);
	arOut=new CArchive(recvFile,CArchive::store);
}

