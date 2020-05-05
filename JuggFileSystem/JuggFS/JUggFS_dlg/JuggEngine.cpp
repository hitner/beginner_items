
#include "JuggEngine.h"
#include <stack>
using namespace std;
JuggEngine::JuggEngine(){
	buf=NULL;
	m_pFile=NULL;

	m_pNodeBitmap=NULL;//����λͼ
	m_pBlockBitmap=NULL;

	m_pRootNode=NULL;
	bbuf_main=NULL;//ÿ���СΪblock�Ĵ�С--
	bbuf_secnd=NULL;
	bbuf_third=NULL;

}
JuggEngine::~JuggEngine(){
	if(m_pNodeBitmap){
		delete [] m_pNodeBitmap;
	}
	if(m_pBlockBitmap){
		delete [] m_pBlockBitmap;
	}

	if(bbuf_main){
		delete []bbuf_main;//ÿ���СΪblock�Ĵ�С
	}
	if(bbuf_secnd){
		delete []bbuf_secnd;
	}
	if(bbuf_third){
		delete []bbuf_third;
	}
	if(buf){
		delete []buf;
	}
	//ɾ��JuggNode��������Close���棬�и�����
	if(m_pRootNode==NULL){
		return;
	}
	stack<JuggNode*> del_stack;
	JuggNode *pJN,*pTmp;
	pJN=m_pRootNode;
	while(pJN || !del_stack.empty()){
		if(pJN){
			del_stack.push(pJN);
			pJN=pJN->m_pLChild;
		}
		else{
			pJN=del_stack.top();
			del_stack.pop();
			pTmp=pJN->m_pRChild;
			delete pJN;
			pJN=pTmp;
		}
	}


}
BOOL JuggEngine::PreHandle(CFile &cfile){
	m_pFile=&cfile;
	if(buf) return TRUE;
	try{
		buf=new UChar[FILE_BUF_LEN];
		return TRUE;
	}
	catch(...){
		m_pFile=NULL;
		return FALSE;
	}
	/**/
}
//����Disk
BOOL JuggEngine::CreateDisk(Int disk_size,Int block_size){
	int i;
	m_pFile->SetLength(disk_size*MB_SIZE);
	InitRule(disk_size,block_size);

	m_pNodeBitmap=new UChar[jr_nodebitmap_size];
	m_pBlockBitmap=new UChar[jr_blockbitmap_size];
	memset(m_pNodeBitmap,0,jr_nodebitmap_size);
	memset(m_pBlockBitmap,0,jr_blockbitmap_size);

	m_FreeNodesCnt=jr_nodes_cnt;
	m_FreeBlocksCnt=jr_blocks_cnt;
	m_iBlockSize=jr_block_size*1024;
//block_table֮ǰ�����򶼵�����Ϊ0
	
	m_pFile->SeekToBegin();
	for(i=0;i<jr_block_table/m_iBlockSize;i++){//--ggW --�᲻������ϣ�
		m_pFile->Write(m_pBlockBitmap,m_iBlockSize);
	}
	m_pFile->Flush();
	SaveJRule2File(m_pFile);//�����½���ʱ����������Ĳ���

	//������һ����Ŀ¼
	int node_loc=0;
	node_loc=GetSetFirstEmptyNode();
	if(node_loc==-1){
		return FALSE;
	}

	m_pRootNode=new JuggNode();
	m_pRootNode->Initialize(_T("JUGG_A"),node_loc+1,-1);//��Ŀ¼

	//���浽�ļ���
	SaveJNode2Disk(m_pRootNode);
	//������ʱblock����
	bbuf_main=new Int[m_iBlockSize/4];//ÿ���СΪblock/4�Ĵ�С �ɴ�ŵ�����
	bbuf_secnd=new Int[m_iBlockSize/4];
	bbuf_third=new Int[m_iBlockSize/4];

	return TRUE;
}


BOOL JuggEngine::OpenDisk(){//��Ҫһ���ϴ���ļ�����
	//char check[1024];
try{
	if(m_pFile->GetLength()<1024*1024){
		m_pFile=NULL;
		return FALSE;
	}
	if(!LoadJRule(m_pFile)){
		m_pFile=NULL;
		return FALSE;
	}
	//bitmap Ҳ�ó�ʼ��
	m_pNodeBitmap=new UChar[jr_nodebitmap_size];
	m_pBlockBitmap=new UChar[jr_blockbitmap_size];
	m_pFile->Seek(jr_node_bitmap,CFile::begin);
	m_pFile->Read(m_pNodeBitmap,jr_nodebitmap_size);
	m_pFile->Seek(jr_block_bitmap,CFile::begin);
	m_pFile->Read(m_pBlockBitmap,jr_blockbitmap_size);
	//����free��ֵ
	m_FreeNodesCnt=CalculateFreeEntries(m_pNodeBitmap,jr_nodes_cnt);
	m_FreeBlocksCnt=CalculateFreeEntries(m_pBlockBitmap,jr_blocks_cnt);

	m_iBlockSize=jr_block_size*1024;
	//juggNode��ʼ����ȫ�������ڴ����Ǹ������ص�����--��Ҫ���8M���ڴ�ռ䣬û����
	ReadAllJNode();
	//������ʱblock����
	bbuf_main=new Int[m_iBlockSize/4];//ÿ���СΪblock/4�Ĵ�С �ɴ�ŵ�����
	bbuf_secnd=new Int[m_iBlockSize/4];
	bbuf_third=new Int[m_iBlockSize/4];
}
	catch(...){
		//�п��ܵ����ڴ�й©��....
		RealseDynamicBuf();
		return FALSE;
	}

	return TRUE;
}
BOOL JuggEngine::CloseDisk(){//��Ҫ��Ҫ�ͷ���Щnew�����Ŀռ�
	RealseDynamicBuf();
	//ɾ��JuggNode��������Close���棬�и�����
	if(m_pRootNode==NULL){
		return TRUE;
	}
	stack<JuggNode*> del_stack;
	JuggNode *pJN,*pTmp;
	pJN=m_pRootNode;
	while(pJN || !del_stack.empty()){
		if(pJN){
			del_stack.push(pJN);
			pJN=pJN->m_pLChild;
		}
		else{
			pJN=del_stack.top();
			del_stack.pop();
			pTmp=pJN->m_pRChild;
			delete pJN;
			pJN=pTmp;
		}
	}
	m_pRootNode=NULL;
	return TRUE;
}
BOOL JuggEngine::ReadAllJNode(){
	JuggNode* pCur,*pPre;
	Int NextNodeLoc=0;
	JuggNode::ChildType child_tp;
	ParentType pPreType;
	stack<JuggNode*> JNodeStack;
	try{
		m_pRootNode=new JuggNode();
		ReadOneJNode(m_pRootNode,1);//��ȡ���ڵ�
		pCur=m_pRootNode;

		while(1){
			child_tp=pCur->GetNextReadChild();
			switch(child_tp){
			case JuggNode::NoChild:
				if(JNodeStack.empty()){
					NextNodeLoc=0;
				}
				else{
					pPre=JNodeStack.top();//���ʵģ��������Һ��ӣ���Ϊֻ��bothChild����ջ
					JNodeStack.pop();
					NextNodeLoc=pPre->jn_rchild;
					pPreType=RChildP;
				}
				break;
			case JuggNode::OnlyLChild:
				pPreType=LChildP;
				pPre=pCur;
				NextNodeLoc=pPre->jn_lchild;
				break;
			case JuggNode::OnlyRChild:
				pPreType=RChildP;
				pPre=pCur;
				NextNodeLoc=pPre->jn_rchild;
				break;
			case JuggNode::BothChilds:
				//��ջ
				JNodeStack.push(pCur);

				pPreType=LChildP;
				pPre=pCur;
				NextNodeLoc=pPre->jn_lchild;
				break;
			default :break;
			}

			if(NextNodeLoc!=0){
				pCur=new JuggNode();
				ReadOneJNode(pCur,NextNodeLoc);
				//���ø��ӵ�ָ������
				pCur->m_pParent=pPre;
				if(pPreType==LChildP){
					pPre->m_pLChild=pCur;	
					pCur->m_ParentType=JuggNode::JN_LChilde;
				}
				else if(pPreType==RChildP){
					pPre->m_pRChild=pCur;
					pCur->m_ParentType=JuggNode::JN_RChilde;
				}
			}
			else{
				break;
			}
		}
			
	}
	catch(...){
		return FALSE;
	}
	return TRUE;

}
BOOL JuggEngine::ReadOneJNode(JuggNode*pJNode,Int node_loc){
	pJNode->m_nodeLoc=node_loc;

	m_pFile->Seek(jr_node_table+(node_loc-1)*jr_node_size*64,CFile::begin);

	m_pFile->Read(&pJNode->jn_lchild,4);
	m_pFile->Read(&pJNode->jn_rchild,4);;
	//m_pFile->Read(&pJNode->jn_parent,4); ;
	//�ļ���Ϣ��
	m_pFile->Read(pJNode->jn_name,2*JuggNode::Max_Name_Len);
/*
	tmp=0;
	for(k=0;k<JuggNode::Max_Name_Len-2*wcslen(pJNode->jn_name);k++){
		m_pFile->Read(&tmp,1);
	}*/
	m_pFile->Read(&pJNode->jn_mark,1);//λָʾ�����λָʾ�Ƿ�Ϊ�ļ���

	m_pFile->Read(&pJNode->jn_size,8); // // Size in bytes 8

	m_pFile->Read(&pJNode->jn_atime,4); ;//  Access time 
	m_pFile->Read(&pJNode->jn_ctime,4); ;// ULONG Creation time 
	m_pFile->Read(&pJNode->jn_mtime,4); ;// ULONG Modification time 

	m_pFile->Read(&pJNode->jn_blocks_cnt,4);// ULONG Blocks count 
	m_pFile->Read(pJNode->jn_block,60); //jn_block[15];// Pointers to blocks 

	//------------ggW �Ϸ����ж�
	return TRUE;
}

BOOL JuggEngine::SaveJNode2Disk(JuggNode*pJNode){
	int nLoc0;
	int i,j;

	UChar tmp;

	nLoc0=pJNode->m_nodeLoc-1;
	//��λnode_loc��CFile�е�λ��
	i=nLoc0%8;//
	j=(nLoc0-i)/8;

	m_pFile->Seek(jr_node_bitmap+j,CFile::begin);
	m_pFile->Read(&tmp,1);
	tmp|=(0x80>>i);
	m_pFile->Seek(-1,CFile::current);
	m_pFile->Write(&tmp,1);

	m_pFile->Seek(jr_node_table+nLoc0*jr_node_size*64,CFile::begin);//��λ��0��ʼ

	m_pFile->Write(&pJNode->jn_lchild,4);
	m_pFile->Write(&pJNode->jn_rchild,4);;
	//m_pFile->Write(&pJNode->jn_parent,4); ;
	//�ļ���Ϣ��
	m_pFile->Write(pJNode->jn_name,2*JuggNode::Max_Name_Len);
/*
	tmp=0;
	for(k=0;k<JuggNode::Max_Name_Len-2*wcslen(pJNode->jn_name);k++){
		m_pFile->Write(&tmp,1);
	}*/
	m_pFile->Write(&pJNode->jn_mark,1);//λָʾ�����λָʾ�Ƿ�Ϊ�ļ���

	m_pFile->Write(&pJNode->jn_size,8); // // Size in bytes

	m_pFile->Write(&pJNode->jn_atime,4); ;//  Access time 
	m_pFile->Write(&pJNode->jn_ctime,4); ;// ULONG Creation time 
	m_pFile->Write(&pJNode->jn_mtime,4); ;// ULONG Modification time 

	m_pFile->Write(&pJNode->jn_blocks_cnt,4);// ULONG Blocks count 
	m_pFile->Write(pJNode->jn_block,60); //jn_block[15];// Pointers to blocks 

	m_pFile->Flush();

	//JuggRule�����ǲ���ģ������˰�

	return TRUE;
}
/*
BOOL JuggEngine::RenameFile(LPCTSTR target,LPCTSTR newname){


}*/

int JuggEngine::GetSetFirstEmptyNode(){
	int i,j;
	UChar mask;
	for(i=0;i<jr_nodebitmap_size;i++){
		mask=0x80;
		for(j=0;j<8;j++){
			if(!(m_pNodeBitmap[i]&mask)){
				//���ø�λΪ1
				m_pNodeBitmap[i]|=mask;
				m_FreeNodesCnt--;
				return 8*i+j;
			}
			mask=mask>>1;
		}
	}
	return -1;
}
int JuggEngine::GetSetFirstEmptyBlock(int *begLoc){//���ڴ���0��ʼ������
	int i,j;
	UChar mask;
	if(begLoc==NULL){
		i=0;
	}
	else{
		j=*begLoc%8;
		i=(*begLoc-j)/8;//�ӿ������ٶ�,����ָʾ���浽���λͼλ��
	}
	for(;i<jr_blockbitmap_size;i++){
		mask=0x80;
		for(j=0;j<8;j++){
			if(!(m_pBlockBitmap[i]&mask)){
				//���ø�λΪ1
				m_pBlockBitmap[i]|=mask;
				m_FreeBlocksCnt--;
				if(begLoc==NULL){
					return 8*i+j;
				}
				*begLoc=8*i+j;
				return *begLoc;
			}
			mask=mask>>1;
		}
	}
	return -1;
}
void JuggEngine::SetNodeBitmapFree(Int loc0){//�ͷ�ָ��λ�õ�bitmap
	int i,j;
	UChar mask(0x80);
	ASSERT(loc0>=0);
	j=loc0%8;
	i=(loc0-j)/8;

	mask=mask>>j;
	m_pNodeBitmap[i]&=(~mask);//ȡ��
	m_FreeNodesCnt++;
}
void JuggEngine::SetBlockBitmapFree(Int loc0){
	int i,j;
	UChar mask(0x80);
	ASSERT(loc0>=0);
	j=loc0%8;
	i=(loc0-j)/8;

	mask=mask>>j;
	m_pBlockBitmap[i]&=(~mask);//ȡ��
	m_FreeBlocksCnt++;

}


int JuggEngine::CalculateFreeEntries(const UChar *src,int bit_len){
	int total;
	UChar mask;
	total=0;
	int byte_len,rmn_bit;
	rmn_bit=bit_len%8;
	byte_len=(bit_len-rmn_bit)/8;

	for(int i=0;i<byte_len;i++){
		mask=0x80;
		for(int j=0;j<8;j++){
			if(!(src[i]&mask)){
				total++;
			}
			mask=mask>>1;
		}
	}
	//�����ܲ���byte����
	mask=0x80;
	for(int j=0;j<rmn_bit;j++){
		if(!(src[byte_len]&mask)){
			total++;
		}
		mask=mask>>1;
	}
	return total;
}
//=============================
//�༭����
//============================


JN_Error JuggEngine::CreatFolder(JuggNode* pEnvNode,LPCWSTR nm){//��Ŀ¼�´���
	int node_loc;
	//��һ�����ж������Ƿ�Ϸ�
	JuggNode* pTemp ,*pPrt;
	if(!pEnvNode){
		return JNE_NullPoint;
	}
	if(wcslen(nm)>JuggNode::Max_Name_Len){
		return JNE_TooLongName;
	}
	//�ڶ�����1,�ж��Ƿ��������ļ���--����һ��ͳһ����
	pTemp=pEnvNode->SameNamerInFolder(nm,1);
	if(pTemp){
		return JNE_ExistSameName;
	}
	//--------2,�ҵ�һ��node ��ַ
	node_loc=GetSetFirstEmptyNode();
	if(node_loc==-1){
		return JNE_NEnoughNode;
	}
	//������������һ��JuggNode
	pTemp=new JuggNode();
	pTemp->Initialize(nm,node_loc+1,1);//Ŀ¼


	if(pEnvNode->m_pLChild){
		//�ҵ���ڵ����ҵ��ֵ�
		pPrt=pEnvNode->m_pLChild->GetLastRChild();//����NULL��ʾû���ֵ�
		if(!pPrt){
			pPrt=pEnvNode->m_pLChild;
		}
		pTemp->SetPointRelative(pPrt,JuggNode::JN_RChilde);
		SaveJNode2Disk(pPrt);//�޸��˸��ڵ㣬��Ҫ���±���
	}
	else{
		//�½�����ڵ�
		pTemp->SetPointRelative(pEnvNode,JuggNode::JN_LChilde);
		SaveJNode2Disk(pEnvNode);
	}
	//���浽file
	SaveJNode2Disk(pTemp);

	return JNE_Correct;
}
JN_Error JuggEngine::AddFile(JuggNode* pEnvNode,CFile &sFile){
	//��һ�����жϲ����Ƿ�Ϸ�
	JuggNode* pTemp ,*pPrt;
	CString cst_nm;
	Int nLoc0;
	Int max_blocks;

	if(!pEnvNode){
		return JNE_NullPoint;
	}
	cst_nm=sFile.GetFileName();
	if(cst_nm.GetLength()>JuggNode::Max_Name_Len){
		return JNE_TooLongName;
	}
	//�ڶ�����Ԥ�ж�
	if(!(pEnvNode->jn_mark&0x01)){
		return JNE_NotAFolder;//���ļ���
	}
	pTemp=pEnvNode->SameNamerInFolder(cst_nm,0);
	if(pTemp){
		return JNE_ExistSameName;
	}
	//          2�ҵ�һ��node ��ַ
	nLoc0=GetSetFirstEmptyNode();
	if(nLoc0==-1){
		return JNE_NEnoughNode;
	}
	//          3,�ļ���С�ж�
	UInt64 filesize=sFile.GetLength();
	Int remain=filesize%m_iBlockSize;
	Int blocks_of_file=(filesize-remain)/m_iBlockSize;
	if(remain){
		blocks_of_file++;
	}
	if(blocks_of_file>m_FreeBlocksCnt){
		return JNE_NEnoughBlock;
	}
	max_blocks=m_iBlockSize/4;

	if(blocks_of_file>max_blocks*max_blocks*max_blocks){
		return JNE_TooBigFile;
	}

	//������������һ��JuggNode
	pTemp=new JuggNode();
	pTemp->Initialize(cst_nm,nLoc0+1);
	pTemp->jn_size=filesize;
	pTemp->jn_blocks_cnt=blocks_of_file;


	if(pEnvNode->m_pLChild){
		//�ҵ���ڵ����ҵ��ֵ�
		pPrt=pEnvNode->m_pLChild->GetLastRChild();//����NULL��ʾû���ֵ�
		if(!pPrt){
			pPrt=pEnvNode->m_pLChild;
		}
		pTemp->SetPointRelative(pPrt,JuggNode::JN_RChilde);
		SaveJNode2Disk(pPrt);//�޸��˸��ڵ㣬��Ҫ���±���
	}
	else{
		//�½�����ڵ�
		pTemp->SetPointRelative(pEnvNode,JuggNode::JN_LChilde);
		SaveJNode2Disk(pEnvNode);
	}
	//���Ĳ�,�����ļ�����

	if(remain==0&&blocks_of_file==0){//���ļ�
		SaveJNode2Disk(pTemp);
	}
	else{
		//��һ��ѭ��
		SaveBlockBtimapAndFileCont(pTemp,sFile,blocks_of_file,remain);
		SaveJNode2Disk(pTemp);
	}

	return JNE_Correct;

}
BOOL JuggEngine::SaveBlockBtimapAndFileCont(JuggNode*pNode,CFile&sFile,Int blocks_cnt,Int remain_size){
	int m512,d512_rmn;//һ��buf ��
	int beginSrchLoc0=0,blockLoc0;//��ʼ����ָ��λ��
	int end_flag=0;
	sFile.SeekToBegin();
	if(blocks_cnt<=15){//ȫ��ֱ������
		pNode->jn_mark|=0x02;//˵��15����ַ����ֱ�ӵ�ַ
		for(int i=0;i<blocks_cnt;i++){
			blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);
			pNode->jn_block[i]=blockLoc0+1;
			if(i==blocks_cnt-1){//���һ����remain_size 0��������block
				CopyBlockByte(blockLoc0,sFile,remain_size);
			}
			else{
				CopyBlockByte(blockLoc0,sFile);
			}
		}
	}
	else{
		//�漰�������ַ--��ʵֱ�ӽ��оͿ�����--�ֶ�����
		int OneStage=m_iBlockSize/4;
		int TwoStage=OneStage*OneStage;
		blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);
		pNode->jn_block[0]=blockLoc0+1;

		if(blocks_cnt<=OneStage){//һ���м�㶨
			for(int i=0;i<blocks_cnt;i++){//���һ���ó���
				blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);
				bbuf_main[i]=blockLoc0+1;//ֻ�д˴���15���Ͳ�һ��
				if(i==blocks_cnt-1){//���һ����remain_size 0��������block
					CopyBlockByte(blockLoc0,sFile,remain_size);
				}
				else{
					CopyBlockByte(blockLoc0,sFile);
				}
			}
			//����bbuf_main ����
			CopyStageBlockBuf(pNode->jn_block[0]-1,bbuf_main,blocks_cnt);
		}
		//===================
		//---------�м����--------------
		else if(blocks_cnt<=TwoStage){
			d512_rmn=blocks_cnt%OneStage;
			m512=(blocks_cnt-d512_rmn)/OneStage;
			if(d512_rmn){
				m512++;//��Ϊ0������һ����������
			}
			int saved_block_cnts=0;
			for(int j=0;j<m512;j++){
				blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);				 
				bbuf_main[j]=blockLoc0+1;
				int k;
				for(k=0;k<OneStage;k++){//����ڶ���block����Ҫ�ж�i�Ƿ���
					blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);					 
					bbuf_secnd[k]=blockLoc0+1;
					if(saved_block_cnts==blocks_cnt-1){//���һ����remain_size 0��������block
						CopyBlockByte(blockLoc0,sFile,remain_size);
						k++;
						break;
					}
					else{
						CopyBlockByte(blockLoc0,sFile);
					}
					saved_block_cnts++;//ָʾ�ļ��е�ֵ��������
				}
				//�����м�buf
				CopyStageBlockBuf(bbuf_main[j]-1,bbuf_secnd,k);
			}
			CopyStageBlockBuf(pNode->jn_block[0]-1,bbuf_main,m512);//������bbuf
		}
		//===================
		//---------�м�����--------------
		else {
			int dD512_rmn=blocks_cnt%TwoStage;
			int mD512=(blocks_cnt-dD512_rmn)/TwoStage;
			if(dD512_rmn){
				mD512++;
			}
			int saved_block_cnts=0;
			for(int j=0;j<mD512;j++){//jָʾ�����
				blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);				 
				bbuf_main[j]=blockLoc0+1;
				int k=0;
				for(;k<OneStage;k++){//����ڶ���block����Ҫ�ж�i�Ƿ���
					blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);					 
					bbuf_secnd[k]=blockLoc0+1;
					int z;
					for(z=0;z<OneStage;z++){
						blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);
						bbuf_third[z]=blockLoc0+1;
						if(saved_block_cnts==blocks_cnt-1){
							CopyBlockByte(blockLoc0,sFile,remain_size);//���һ����remain_size 0��������block
							z++;
							end_flag=1;
							break;
						}
						else{
							CopyBlockByte(blockLoc0,sFile);
						}
						saved_block_cnts++;//ָʾ�ļ��е�ֵ��������
					}//end z
					//������β����
					CopyStageBlockBuf(bbuf_secnd[k]-1,bbuf_third,z);

					if(end_flag==1){//ָʾ����
						k++;
						break;
					}
				}//end k
				//�����м�buf
				CopyStageBlockBuf(bbuf_main[j]-1,bbuf_secnd,k);
			}
			CopyStageBlockBuf(pNode->jn_block[0]-1,bbuf_main,mD512);//������bbuf
		}
	}
	//����blockλͼ
	return SaveBlockBitmap2Disk(beginSrchLoc0);

}
JN_Error JuggEngine::DownloadNodedFile(CFile &dFile,const JuggNode* pNode)const{
	Int bcks_num;
	int d512_rmn;//û����������block�������ֽ���
	int readed_cnt;

	int OneStage=m_iBlockSize/4;
	int TwoStage=OneStage*OneStage;

	if(!pNode){
		return JNE_NullPoint;
	}
	if(pNode->jn_mark&0x01){
		return JNE_NotAFile;
	}
	
	if(pNode->jn_size==0){
		dFile.SetLength(0);
		return  JNE_Correct;
	}
	//dFile.SetLength(pNode->jn_size);
	//dFile.SeekToBegin();
	//��ʼ�����ˣ�ͬѧ�ǣ���4�����
	bcks_num=pNode->jn_blocks_cnt;
	d512_rmn=bcks_num*m_iBlockSize-pNode->jn_size;
	if(d512_rmn){
		d512_rmn=m_iBlockSize-d512_rmn;
	}
	//15��ֱ�ӵ�ַ
	if(bcks_num<=15){
		for(int i=0;i<bcks_num;i++){
			if(i==bcks_num-1){
				DownloadBlockByte(dFile,pNode->jn_block[i]-1,d512_rmn);
			}
			else{
				DownloadBlockByte(dFile,pNode->jn_block[i]-1);
			}
		}
	}
	//<=512����ַ��һ���м�--ֻ��������ַ��λ�÷����仯
	else if(bcks_num<=OneStage){
		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);
		for(int i=0;i<bcks_num;i++){
			if(i==bcks_num-1){
				DownloadBlockByte(dFile,bbuf_main[i]-1,d512_rmn);
			}
			else{
				DownloadBlockByte(dFile,bbuf_main[i]-1);
			}
		}
	}
	//<=512*512,�����м�
	else if(bcks_num<=TwoStage){
		int last_s_item_cnt,main_item_cnt;
		last_s_item_cnt=bcks_num%OneStage;//���һ����block����Ŀ�����������Ϊ0����ʵ��������Ŀ
		main_item_cnt=(bcks_num-last_s_item_cnt)/OneStage;
		if(last_s_item_cnt){
			main_item_cnt++;//��buf����Ŀ������
		}
		readed_cnt=0;
		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);//������block
		for(int i=0;i<main_item_cnt;i++){//������block
			DownloadStageBlockBuf(bbuf_secnd,bbuf_main[i]-1);//�����block
			for(int j=0;j<OneStage;j++){//����������block
				if(readed_cnt==bcks_num-1){
					DownloadBlockByte(dFile,bbuf_secnd[j]-1,d512_rmn);
					break;//������־
				}
				else{
					DownloadBlockByte(dFile,bbuf_secnd[j]-1);
					readed_cnt++;
				}
			}
		}
	}
	//>512*512,�����м�
	else{
int tmp,main_item_cnt;//;
	
tmp=bcks_num%TwoStage;
main_item_cnt=(bcks_num-tmp)/TwoStage;//��block����
if(tmp){
	main_item_cnt++;
}
readed_cnt=0;
DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);//������block
for(int i=0;i<main_item_cnt;i++){//������block
	DownloadStageBlockBuf(bbuf_secnd,bbuf_main[i]-1);//�����block
	for(int j=0;j<OneStage;j++){//����������block
		DownloadStageBlockBuf(bbuf_third,bbuf_secnd[j]-1);//�������block
		for(int k=0;k<OneStage;k++){//��������block
			if(readed_cnt==bcks_num-1){
				DownloadBlockByte(dFile,bbuf_third[k]-1,d512_rmn);//��ȡ����
				return JNE_Correct;//������־
			}
			else{
				DownloadBlockByte(dFile,bbuf_third[k]-1);//��ȡ����
				readed_cnt++;
			}
		}//end ����
	}//end��
}//end ��
	}
	return  JNE_Correct;
}
JN_Error JuggEngine::DownloadNamedFile(CFile &dFile,const JuggNode* pEnvNode,LPCWSTR nm)const{
	//��1����Ԥ�ж�	
	if(!pEnvNode){
		return JNE_NullPoint;
	}
	if(!(pEnvNode->jn_mark&0x01)){
		return JNE_NotAFolder;//���ļ���
	}
	const JuggNode*pTmp;
	pTmp=pEnvNode->SameNamerInFolder(nm);
	if(pTmp){
		return DownloadNodedFile(dFile,pTmp);
	}
	else{
		return  JNE_NoSameNameFile;
	}
}

JN_Error JuggEngine::MoveNode(JuggNode *pEnvNode,JuggNode* pBeChild){//�ƶ��ļ����ļ���
	if(!pEnvNode||! pBeChild){
		return JNE_NullPoint;
	}
	if(!(pEnvNode->jn_mark&0x01)){
		return JNE_NotAFolder;//���ļ���
	}
	int folder_flag=0;
	if(pBeChild->IsFolder()){
		folder_flag=1;
	}
	JuggNode*pTemp=pEnvNode->SameNamerInFolder(pBeChild->jn_name,folder_flag);
	if(pTemp){
		return JNE_ExistSameName;
	}

	//1��-----------------ԭchild���ڵı仯------------------
	JuggNode *pPrnt=pBeChild->m_pParent;
	JuggNode *pRChld=pBeChild->m_pRChild;
	switch(pBeChild->m_ParentType){
	case JuggNode::JN_NoParent:return JNE_NotDeletable;//���ڵ㣬�޷�ɾ��/�ƶ�
	case JuggNode::JN_LChilde://��Ϊ����
		if(pRChld){//���в����ֵܣ���������
			pPrnt->m_pLChild=pRChld;
			pPrnt->jn_lchild=pRChld->m_nodeLoc;
			pRChld->m_pParent=pPrnt;
			pRChld->m_ParentType=JuggNode::JN_LChilde;
		}
		else{
			pPrnt->m_pLChild=NULL;
			pPrnt->jn_lchild=0;
		}
		break;
	case JuggNode::JN_RChilde://�Һ���
		if(pRChld){//���в����ֵ�
			pPrnt->m_pRChild=pRChld;
			pPrnt->jn_rchild=pRChld->m_nodeLoc;
			pRChld->m_pParent=pPrnt;
			pRChld->m_ParentType=JuggNode::JN_RChilde;
		}
		else{
			pPrnt->m_pRChild=NULL;
			pPrnt->jn_rchild=0;
		}
		break;
	default:break;
	}
	//���游�ڵ�--�����������Ϊ�ܶ���Ϣ��û��
	SaveJNode2Disk(pPrnt);
	//�����Һ���
	if(pRChld){
		SaveJNode2Disk(pRChld);
	}

	pBeChild->m_pRChild=NULL;//ԭ�ֵ�ʧȥ��ϵ ���ڵ㲻��
	pBeChild->jn_rchild=0;

	//---------------2��Ӳ��� ��child���ڵı仯-------------------------
	if(pEnvNode->m_pLChild){
		//�ҵ���ڵ����ҵ��ֵ�
		pTemp=pEnvNode->m_pLChild->GetLastRChild();//����NULL��ʾû���ֵ�
		if(!pTemp){
			pTemp=pEnvNode->m_pLChild;
		}
		//pTemp���¼ӽڵ�ĸ���
		pBeChild->SetPointRelative(pTemp,JuggNode::JN_RChilde);
		SaveJNode2Disk(pTemp);//�޸��˸��ڵ㣬��Ҫ���±���
	}
	else{
		//�½�����ڵ�--pEnvNode���丸��
		pBeChild->SetPointRelative(pEnvNode,JuggNode::JN_LChilde);
		SaveJNode2Disk(pEnvNode);
	}
	//�����ƶ����Ľڵ�
	SaveJNode2Disk(pBeChild);
	return JNE_Correct;
}

JN_Error JuggEngine::JN_DeleteFile(JuggNode *pNode){//�ļ��к��ļ�ͳһ����
	if(!pNode){
		return JNE_NullPoint;
	}

	JuggNode *pPrnt=pNode->m_pParent;
	JuggNode *pRChld=pNode->m_pRChild;

	switch(pNode->m_ParentType){
	case JuggNode::JN_NoParent:return JNE_NotDeletable;//���ڵ㣬�޷�ɾ��
	case JuggNode::JN_LChilde://��Ϊ����
		if(pRChld){
			pPrnt->m_pLChild=pRChld;
			pPrnt->jn_lchild=pRChld->m_nodeLoc;
			pRChld->m_pParent=pPrnt;
			pRChld->m_ParentType=JuggNode::JN_LChilde;
		}
		else{
			pPrnt->m_pLChild=NULL;
			pPrnt->jn_lchild=0;
		}
		break;
	case JuggNode::JN_RChilde://�Һ���
		if(pRChld){
			pPrnt->m_pRChild=pRChld;
			pPrnt->jn_rchild=pRChld->m_nodeLoc;
			pRChld->m_pParent=pPrnt;
			pRChld->m_ParentType=JuggNode::JN_RChilde;
		}
		else{
			pPrnt->m_pRChild=NULL;
			pPrnt->jn_rchild=0;
		}
		break;
	default:break;
	}
	//���游�ڵ�--�����������Ϊ�ܶ���Ϣ��û��
	SaveJNode2Disk(pPrnt);
	//�����Һ���
	if(pRChld){
		SaveJNode2Disk(pRChld);
	}
	//�ж���ϵ���Ǳ��룩
	pNode->jn_rchild=0;
	pNode->m_pRChild=NULL;

	pNode->m_pParent=NULL;
	pNode->m_ParentType=JuggNode::JN_NoParent;
	
	//ɾ����ǰ�ڵ�
	if(pNode->jn_mark&0x01){//�ļ���--����--�ݹ�
		DeleteDiscardedFolder(pNode);
	}
	else{
		//����node bitmap
		SetNodeBitmapFree(pNode->m_nodeLoc-1);
		SaveNodeBitmap2Disk();
		//����block
		DeleteDiscardedFileBlocks(pNode);
		SaveBlockBitmap2Disk();
		delete pNode;
	}
	m_pFile->Flush();
	return JNE_Correct;
}

BOOL JuggEngine::DeleteDiscardedFileBlocks(JuggNode*pNode){//��һ���ļ�����
	//����block bitmap
	Int blcks_num=pNode->jn_blocks_cnt;
	int OneStage=m_iBlockSize/4;
	int TwoStage=OneStage*OneStage;
	int main_item_cnt,temp,del_block_Cnt=0;

	if(blcks_num<=15){//ֱ������
		for(int i=0;i<blcks_num;i++){
			SetBlockBitmapFree(pNode->jn_block[i]-1);
		}
	}
	else if(blcks_num<=OneStage){
		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);
		SetBlockBitmapFree(pNode->jn_block[0]-1);
		for(int i=0;i<blcks_num;i++){
			SetBlockBitmapFree(bbuf_main[i]-1);//����block
		}
	}
	//============��������=============
	else if(blcks_num<=TwoStage){
		temp=blcks_num%OneStage;
		main_item_cnt=(blcks_num-temp)/OneStage;
		if(temp) {main_item_cnt++;}//main_buf�еĸ���
		del_block_Cnt=0;

		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);
		SetBlockBitmapFree(pNode->jn_block[0]-1);//��blockҲɾ��
		for(int i=0;i<main_item_cnt;i++){
			DownloadStageBlockBuf(bbuf_secnd,bbuf_main[i]-1);
			SetBlockBitmapFree(bbuf_main[i]-1);//������block
			for(int j=0;j<OneStage;j++){
				SetBlockBitmapFree(bbuf_secnd[j]-1);//����block
				del_block_Cnt++;
				if(del_block_Cnt==blcks_num){//ɾ��blcks�����ѵ�
					break;
				}
			}
		}
	}
	//------------------------------
	//=======��������======
	else{
		temp=blcks_num%TwoStage;
		main_item_cnt=(blcks_num-temp)/TwoStage;
		if(temp) {main_item_cnt++;}//main_buf�еĸ���

		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);
		SetBlockBitmapFree(pNode->jn_block[0]-1);//ɾ����block

		for(int i=0;i<main_item_cnt;i++){
			DownloadStageBlockBuf(bbuf_secnd,bbuf_main[i]-1);
			SetBlockBitmapFree(bbuf_main[i]-1);//ɾ��������block
			for(int j=0;j<OneStage;j++){
				DownloadStageBlockBuf(bbuf_third,bbuf_secnd[j]-1);
				SetBlockBitmapFree(bbuf_secnd[j]-1);//ɾ��������block
				for(int k=0;k<OneStage;k++){
					SetBlockBitmapFree(bbuf_third[k]-1);//ɾ������block
					del_block_Cnt++;
					if(del_block_Cnt==blcks_num){//ɾ��blcks�����ѵ�
						j=OneStage;
						break;
					}
				}
			}
		}
	}
	//��󱣴�bitmap��disk
	return TRUE;
}

BOOL JuggEngine::DeleteDiscardedFolder(JuggNode*pNode){//ɾ��һ������Ŀ¼�����ļ���--��Ҫ�ݹ�--��node��δ����
	//��������Ŀ¼
	stack<JuggNode*> jn_stack;
	JuggNode* pJN=pNode;
	JuggNode* pDel;
	while(pJN||!jn_stack.empty()){//ջ�ǿգ���ǰ��ָ��
		if(pJN){
			jn_stack.push(pJN);
			pJN=pJN->m_pLChild;
		}
		else{
			pDel=jn_stack.top();
			jn_stack.pop();
			pJN=pDel->m_pRChild;

			SetNodeBitmapFree(pDel->m_nodeLoc-1);//ɾ��node==����bitmapλ
			if(!(pDel->jn_mark&0x01)){
				DeleteDiscardedFileBlocks(pDel);//ɾ�ļ����ļ��в���Ҫ����block
			}
			delete pDel;
		}
	}
	SaveNodeBitmap2Disk();
	SaveBlockBitmap2Disk();
	return TRUE;
}
BOOL JuggEngine::DownloadBlockByte(CFile &dFile,Int src_block0,Int cpy_byte)const{
	ASSERT(src_block0>=0);
	if(cpy_byte==0){
		cpy_byte=m_iBlockSize;
	}
	try{
		m_pFile->Seek(jr_block_table+src_block0*m_iBlockSize,CFile::begin);
		m_pFile->Read(buf,cpy_byte);//�ȶ���д
		dFile.Write(buf,cpy_byte);
		//dFile.Flush();
		return TRUE;
	}
	catch(...){
		return FALSE;
	}
}
BOOL JuggEngine::CopyBlockByte(Int dst_block0,CFile&sFile, Int cpy_byte){//Դ�ļ�����seek��
	ASSERT(dst_block0>=0);
	if(cpy_byte==0){
		cpy_byte=m_iBlockSize;
	}
	try{
		//sFile.Seek(src,CFile::begin);
		m_pFile->Seek(jr_block_table+dst_block0*m_iBlockSize,CFile::begin);
		sFile.Read(buf,cpy_byte);//�ȶ���д
		m_pFile->Write(buf,cpy_byte);
		//m_pFile->Flush();
		return TRUE;
	}
	catch(...){
		return FALSE;
	}
}
BOOL JuggEngine::DownloadStageBlockBuf(Int *bbuf,Int src_block0)const{//���Ƕ�ȡһ��block������
	ASSERT(src_block0>=0);
	try{
		m_pFile->Seek(jr_block_table+src_block0*m_iBlockSize,CFile::begin);
		m_pFile->Read(bbuf,m_iBlockSize);
		return TRUE;
	}
	catch(...){
		return FALSE;
	}

}
BOOL JuggEngine::CopyStageBlockBuf(Int dst_block0,Int *bbuf, Int cpy_num){
	ASSERT(dst_block0>=0);
	int i;
	int v512;
	v512=m_iBlockSize/4;
	for(i=cpy_num;i<v512;i++){
		bbuf[i]=0;
	}
	try{
		m_pFile->Seek(jr_block_table+dst_block0*m_iBlockSize,CFile::begin);
		m_pFile->Write(bbuf,m_iBlockSize);
		//m_pFile->Flush();
		return TRUE;
	}
	catch(...){
		return FALSE;
	}
}
JuggNode* JuggEngine::GetRootNode(){
	return m_pRootNode;
}



BOOL JuggEngine::SaveBlockBitmap2Disk(int endBitLoc0){
	int rmn;
	if(endBitLoc0==-1){
		rmn=jr_blockbitmap_size;
	}
	else{
		rmn=endBitLoc0%8;
		rmn=(endBitLoc0-rmn)/8;
		rmn++;
	}

	m_pFile->Seek(jr_block_bitmap,CFile::begin);
	m_pFile->Write(m_pBlockBitmap,rmn);
	return TRUE;
}

BOOL JuggEngine::SaveNodeBitmap2Disk(){
	m_pFile->Seek(jr_node_bitmap,CFile::begin);
	m_pFile->Write(m_pNodeBitmap,jr_nodebitmap_size);
	return TRUE;
}



void JuggEngine::RealseDynamicBuf(){//��ʵ����������һ��
	if(m_pNodeBitmap){
		delete [] m_pNodeBitmap;
		m_pNodeBitmap=NULL;
	}
	if(m_pBlockBitmap){
		delete [] m_pBlockBitmap;
		m_pBlockBitmap=NULL;
	}

	if(bbuf_main){
		delete []bbuf_main;//ÿ���СΪblock�Ĵ�С
		bbuf_main=NULL;
	}
	if(bbuf_secnd){
		delete []bbuf_secnd;
		bbuf_secnd=NULL;
	}
	if(bbuf_third){
		delete []bbuf_third;
		bbuf_third=NULL;
	}
//	if(buf){����Ͳ�ɾ�ˣ�����
//		delete []buf;
//	}
}

void JuggEngine::GetJNErrorString(CString &err,JN_Error  jnerror){
	switch(jnerror){
	case JNE_Correct: break;
	case JNE_TooLongName :err=_T("�ļ�������"); break; 
	case  JNE_NullPoint:err=_T("��ָ��"); break;
	case JNE_TooBigFile :err=_T("�ļ�����"); break; 
	case  JNE_NotAFile :err=_T("����һ���ļ�"); break; 
	case  JNE_NoSameNameFile :err=_T("�����ڸ����Ƶ��ļ�"); break; 
	case  JNE_NEnoughNode :err=_T("Ŀ¼�ڵ�������"); break; 
	case  JNE_ExistSameName :err=_T("����ͬ���ļ�/�ļ���"); break; 
	case  JNE_NEnoughBlock :err=_T("������������"); break; 
	case  JNE_NotAFolder:err=_T("����һ���ļ���"); break;
	case JNE_NotDeletable:err=_T("�޷�ɾ��"); break;
	default:break;
	}
}