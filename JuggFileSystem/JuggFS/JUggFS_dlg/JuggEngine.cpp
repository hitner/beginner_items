
#include "JuggEngine.h"
#include <stack>
using namespace std;
JuggEngine::JuggEngine(){
	buf=NULL;
	m_pFile=NULL;

	m_pNodeBitmap=NULL;//两个位图
	m_pBlockBitmap=NULL;

	m_pRootNode=NULL;
	bbuf_main=NULL;//每块大小为block的大小--
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
		delete []bbuf_main;//每块大小为block的大小
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
	//删除JuggNode树放在了Close里面，中根序列
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
//创建Disk
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
//block_table之前的区域都得设置为0
	
	m_pFile->SeekToBegin();
	for(i=0;i<jr_block_table/m_iBlockSize;i++){//--ggW --会不会除不断？
		m_pFile->Write(m_pBlockBitmap,m_iBlockSize);
	}
	m_pFile->Flush();
	SaveJRule2File(m_pFile);//仅在新建的时候进行这样的操作

	//创建第一个根目录
	int node_loc=0;
	node_loc=GetSetFirstEmptyNode();
	if(node_loc==-1){
		return FALSE;
	}

	m_pRootNode=new JuggNode();
	m_pRootNode->Initialize(_T("JUGG_A"),node_loc+1,-1);//根目录

	//保存到文件中
	SaveJNode2Disk(m_pRootNode);
	//分配临时block变量
	bbuf_main=new Int[m_iBlockSize/4];//每块大小为block/4的大小 可存放的数量
	bbuf_secnd=new Int[m_iBlockSize/4];
	bbuf_third=new Int[m_iBlockSize/4];

	return TRUE;
}


BOOL JuggEngine::OpenDisk(){//需要一个较大的文件缓存
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
	//bitmap 也好初始化
	m_pNodeBitmap=new UChar[jr_nodebitmap_size];
	m_pBlockBitmap=new UChar[jr_blockbitmap_size];
	m_pFile->Seek(jr_node_bitmap,CFile::begin);
	m_pFile->Read(m_pNodeBitmap,jr_nodebitmap_size);
	m_pFile->Seek(jr_block_bitmap,CFile::begin);
	m_pFile->Read(m_pBlockBitmap,jr_blockbitmap_size);
	//计数free数值
	m_FreeNodesCnt=CalculateFreeEntries(m_pNodeBitmap,jr_nodes_cnt);
	m_FreeBlocksCnt=CalculateFreeEntries(m_pBlockBitmap,jr_blocks_cnt);

	m_iBlockSize=jr_block_size*1024;
	//juggNode初始化，全部载入内存这是个很严重的问题--需要最多8M的内存空间，没问题
	ReadAllJNode();
	//分配临时block变量
	bbuf_main=new Int[m_iBlockSize/4];//每块大小为block/4的大小 可存放的数量
	bbuf_secnd=new Int[m_iBlockSize/4];
	bbuf_third=new Int[m_iBlockSize/4];
}
	catch(...){
		//有可能导致内存泄漏啊....
		RealseDynamicBuf();
		return FALSE;
	}

	return TRUE;
}
BOOL JuggEngine::CloseDisk(){//主要是要释放那些new出来的空间
	RealseDynamicBuf();
	//删除JuggNode树放在了Close里面，中根序列
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
		ReadOneJNode(m_pRootNode,1);//读取根节点
		pCur=m_pRootNode;

		while(1){
			child_tp=pCur->GetNextReadChild();
			switch(child_tp){
			case JuggNode::NoChild:
				if(JNodeStack.empty()){
					NextNodeLoc=0;
				}
				else{
					pPre=JNodeStack.top();//出帐的，访问其右孩子，因为只有bothChild才入栈
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
				//入栈
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
				//设置父子的指针连接
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
	//文件信息组
	m_pFile->Read(pJNode->jn_name,2*JuggNode::Max_Name_Len);
/*
	tmp=0;
	for(k=0;k<JuggNode::Max_Name_Len-2*wcslen(pJNode->jn_name);k++){
		m_pFile->Read(&tmp,1);
	}*/
	m_pFile->Read(&pJNode->jn_mark,1);//位指示，最低位指示是否为文件夹

	m_pFile->Read(&pJNode->jn_size,8); // // Size in bytes 8

	m_pFile->Read(&pJNode->jn_atime,4); ;//  Access time 
	m_pFile->Read(&pJNode->jn_ctime,4); ;// ULONG Creation time 
	m_pFile->Read(&pJNode->jn_mtime,4); ;// ULONG Modification time 

	m_pFile->Read(&pJNode->jn_blocks_cnt,4);// ULONG Blocks count 
	m_pFile->Read(pJNode->jn_block,60); //jn_block[15];// Pointers to blocks 

	//------------ggW 合法性判断
	return TRUE;
}

BOOL JuggEngine::SaveJNode2Disk(JuggNode*pJNode){
	int nLoc0;
	int i,j;

	UChar tmp;

	nLoc0=pJNode->m_nodeLoc-1;
	//定位node_loc在CFile中的位置
	i=nLoc0%8;//
	j=(nLoc0-i)/8;

	m_pFile->Seek(jr_node_bitmap+j,CFile::begin);
	m_pFile->Read(&tmp,1);
	tmp|=(0x80>>i);
	m_pFile->Seek(-1,CFile::current);
	m_pFile->Write(&tmp,1);

	m_pFile->Seek(jr_node_table+nLoc0*jr_node_size*64,CFile::begin);//定位从0开始

	m_pFile->Write(&pJNode->jn_lchild,4);
	m_pFile->Write(&pJNode->jn_rchild,4);;
	//m_pFile->Write(&pJNode->jn_parent,4); ;
	//文件信息组
	m_pFile->Write(pJNode->jn_name,2*JuggNode::Max_Name_Len);
/*
	tmp=0;
	for(k=0;k<JuggNode::Max_Name_Len-2*wcslen(pJNode->jn_name);k++){
		m_pFile->Write(&tmp,1);
	}*/
	m_pFile->Write(&pJNode->jn_mark,1);//位指示，最低位指示是否为文件夹

	m_pFile->Write(&pJNode->jn_size,8); // // Size in bytes

	m_pFile->Write(&pJNode->jn_atime,4); ;//  Access time 
	m_pFile->Write(&pJNode->jn_ctime,4); ;// ULONG Creation time 
	m_pFile->Write(&pJNode->jn_mtime,4); ;// ULONG Modification time 

	m_pFile->Write(&pJNode->jn_blocks_cnt,4);// ULONG Blocks count 
	m_pFile->Write(pJNode->jn_block,60); //jn_block[15];// Pointers to blocks 

	m_pFile->Flush();

	//JuggRule中总是不变的，明白了吧

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
				//设置该位为1
				m_pNodeBitmap[i]|=mask;
				m_FreeNodesCnt--;
				return 8*i+j;
			}
			mask=mask>>1;
		}
	}
	return -1;
}
int JuggEngine::GetSetFirstEmptyBlock(int *begLoc){//是内存中0开始的序列
	int i,j;
	UChar mask;
	if(begLoc==NULL){
		i=0;
	}
	else{
		j=*begLoc%8;
		i=(*begLoc-j)/8;//加快搜索速度,并且指示保存到最大位图位置
	}
	for(;i<jr_blockbitmap_size;i++){
		mask=0x80;
		for(j=0;j<8;j++){
			if(!(m_pBlockBitmap[i]&mask)){
				//设置该位为1
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
void JuggEngine::SetNodeBitmapFree(Int loc0){//释放指定位置的bitmap
	int i,j;
	UChar mask(0x80);
	ASSERT(loc0>=0);
	j=loc0%8;
	i=(loc0-j)/8;

	mask=mask>>j;
	m_pNodeBitmap[i]&=(~mask);//取反
	m_FreeNodesCnt++;
}
void JuggEngine::SetBlockBitmapFree(Int loc0){
	int i,j;
	UChar mask(0x80);
	ASSERT(loc0>=0);
	j=loc0%8;
	i=(loc0-j)/8;

	mask=mask>>j;
	m_pBlockBitmap[i]&=(~mask);//取反
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
	//最后可能不足byte的项
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
//编辑操作
//============================


JN_Error JuggEngine::CreatFolder(JuggNode* pEnvNode,LPCWSTR nm){//该目录下创建
	int node_loc;
	//第一步，判断名字是否合法
	JuggNode* pTemp ,*pPrt;
	if(!pEnvNode){
		return JNE_NullPoint;
	}
	if(wcslen(nm)>JuggNode::Max_Name_Len){
		return JNE_TooLongName;
	}
	//第二步，1,判断是否有重名文件夹--调用一个统一函数
	pTemp=pEnvNode->SameNamerInFolder(nm,1);
	if(pTemp){
		return JNE_ExistSameName;
	}
	//--------2,找到一个node 地址
	node_loc=GetSetFirstEmptyNode();
	if(node_loc==-1){
		return JNE_NEnoughNode;
	}
	//第三步，创建一个JuggNode
	pTemp=new JuggNode();
	pTemp->Initialize(nm,node_loc+1,1);//目录


	if(pEnvNode->m_pLChild){
		//找到左节点最右的兄弟
		pPrt=pEnvNode->m_pLChild->GetLastRChild();//返回NULL表示没有兄弟
		if(!pPrt){
			pPrt=pEnvNode->m_pLChild;
		}
		pTemp->SetPointRelative(pPrt,JuggNode::JN_RChilde);
		SaveJNode2Disk(pPrt);//修改了父节点，故要重新保存
	}
	else{
		//新建的左节点
		pTemp->SetPointRelative(pEnvNode,JuggNode::JN_LChilde);
		SaveJNode2Disk(pEnvNode);
	}
	//保存到file
	SaveJNode2Disk(pTemp);

	return JNE_Correct;
}
JN_Error JuggEngine::AddFile(JuggNode* pEnvNode,CFile &sFile){
	//第一步，判断参数是否合法
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
	//第二步，预判断
	if(!(pEnvNode->jn_mark&0x01)){
		return JNE_NotAFolder;//非文件夹
	}
	pTemp=pEnvNode->SameNamerInFolder(cst_nm,0);
	if(pTemp){
		return JNE_ExistSameName;
	}
	//          2找到一个node 地址
	nLoc0=GetSetFirstEmptyNode();
	if(nLoc0==-1){
		return JNE_NEnoughNode;
	}
	//          3,文件大小判断
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

	//第三步，创建一个JuggNode
	pTemp=new JuggNode();
	pTemp->Initialize(cst_nm,nLoc0+1);
	pTemp->jn_size=filesize;
	pTemp->jn_blocks_cnt=blocks_of_file;


	if(pEnvNode->m_pLChild){
		//找到左节点最右的兄弟
		pPrt=pEnvNode->m_pLChild->GetLastRChild();//返回NULL表示没有兄弟
		if(!pPrt){
			pPrt=pEnvNode->m_pLChild;
		}
		pTemp->SetPointRelative(pPrt,JuggNode::JN_RChilde);
		SaveJNode2Disk(pPrt);//修改了父节点，故要重新保存
	}
	else{
		//新建的左节点
		pTemp->SetPointRelative(pEnvNode,JuggNode::JN_LChilde);
		SaveJNode2Disk(pEnvNode);
	}
	//第四步,复制文件内容

	if(remain==0&&blocks_of_file==0){//空文件
		SaveJNode2Disk(pTemp);
	}
	else{
		//来一个循环
		SaveBlockBtimapAndFileCont(pTemp,sFile,blocks_of_file,remain);
		SaveJNode2Disk(pTemp);
	}

	return JNE_Correct;

}
BOOL JuggEngine::SaveBlockBtimapAndFileCont(JuggNode*pNode,CFile&sFile,Int blocks_cnt,Int remain_size){
	int m512,d512_rmn;//一级buf 数
	int beginSrchLoc0=0,blockLoc0;//开始搜索指针位置
	int end_flag=0;
	sFile.SeekToBegin();
	if(blocks_cnt<=15){//全部直接索引
		pNode->jn_mark|=0x02;//说明15个地址都是直接地址
		for(int i=0;i<blocks_cnt;i++){
			blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);
			pNode->jn_block[i]=blockLoc0+1;
			if(i==blocks_cnt-1){//最后一项，如果remain_size 0则复制整个block
				CopyBlockByte(blockLoc0,sFile,remain_size);
			}
			else{
				CopyBlockByte(blockLoc0,sFile);
			}
		}
	}
	else{
		//涉及到多类地址--其实直接进行就可以了--分段生死
		int OneStage=m_iBlockSize/4;
		int TwoStage=OneStage*OneStage;
		blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);
		pNode->jn_block[0]=blockLoc0+1;

		if(blocks_cnt<=OneStage){//一级中间搞定
			for(int i=0;i<blocks_cnt;i++){//最后一项拿出来
				blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);
				bbuf_main[i]=blockLoc0+1;//只有此处于15类型不一致
				if(i==blocks_cnt-1){//最后一项，如果remain_size 0则复制整个block
					CopyBlockByte(blockLoc0,sFile,remain_size);
				}
				else{
					CopyBlockByte(blockLoc0,sFile);
				}
			}
			//复制bbuf_main 缓存
			CopyStageBlockBuf(pNode->jn_block[0]-1,bbuf_main,blocks_cnt);
		}
		//===================
		//---------中间二级--------------
		else if(blocks_cnt<=TwoStage){
			d512_rmn=blocks_cnt%OneStage;
			m512=(blocks_cnt-d512_rmn)/OneStage;
			if(d512_rmn){
				m512++;//不为0，还需一个不完整的
			}
			int saved_block_cnts=0;
			for(int j=0;j<m512;j++){
				blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);				 
				bbuf_main[j]=blockLoc0+1;
				int k;
				for(k=0;k<OneStage;k++){//进入第二层block，但要判断i是否超了
					blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);					 
					bbuf_secnd[k]=blockLoc0+1;
					if(saved_block_cnts==blocks_cnt-1){//最后一项，如果remain_size 0则复制整个block
						CopyBlockByte(blockLoc0,sFile,remain_size);
						k++;
						break;
					}
					else{
						CopyBlockByte(blockLoc0,sFile);
					}
					saved_block_cnts++;//指示文件中的值总是自增
				}
				//保存中间buf
				CopyStageBlockBuf(bbuf_main[j]-1,bbuf_secnd,k);
			}
			CopyStageBlockBuf(pNode->jn_block[0]-1,bbuf_main,m512);//保存总bbuf
		}
		//===================
		//---------中间三级--------------
		else {
			int dD512_rmn=blocks_cnt%TwoStage;
			int mD512=(blocks_cnt-dD512_rmn)/TwoStage;
			if(dD512_rmn){
				mD512++;
			}
			int saved_block_cnts=0;
			for(int j=0;j<mD512;j++){//j指示最外层
				blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);				 
				bbuf_main[j]=blockLoc0+1;
				int k=0;
				for(;k<OneStage;k++){//进入第二层block，但要判断i是否超了
					blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);					 
					bbuf_secnd[k]=blockLoc0+1;
					int z;
					for(z=0;z<OneStage;z++){
						blockLoc0=GetSetFirstEmptyBlock(&beginSrchLoc0);
						bbuf_third[z]=blockLoc0+1;
						if(saved_block_cnts==blocks_cnt-1){
							CopyBlockByte(blockLoc0,sFile,remain_size);//最后一项，如果remain_size 0则复制整个block
							z++;
							end_flag=1;
							break;
						}
						else{
							CopyBlockByte(blockLoc0,sFile);
						}
						saved_block_cnts++;//指示文件中的值总是自增
					}//end z
					//保存最尾端亮
					CopyStageBlockBuf(bbuf_secnd[k]-1,bbuf_third,z);

					if(end_flag==1){//指示结束
						k++;
						break;
					}
				}//end k
				//保存中间buf
				CopyStageBlockBuf(bbuf_main[j]-1,bbuf_secnd,k);
			}
			CopyStageBlockBuf(pNode->jn_block[0]-1,bbuf_main,mD512);//保存总bbuf
		}
	}
	//保存block位图
	return SaveBlockBitmap2Disk(beginSrchLoc0);

}
JN_Error JuggEngine::DownloadNodedFile(CFile &dFile,const JuggNode* pNode)const{
	Int bcks_num;
	int d512_rmn;//没有填满整个block，填充的字节数
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
	//开始下载了，同学们，分4类进行
	bcks_num=pNode->jn_blocks_cnt;
	d512_rmn=bcks_num*m_iBlockSize-pNode->jn_size;
	if(d512_rmn){
		d512_rmn=m_iBlockSize-d512_rmn;
	}
	//15个直接地址
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
	//<=512个地址，一个中间--只是索引地址段位置发生变化
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
	//<=512*512,二个中间
	else if(bcks_num<=TwoStage){
		int last_s_item_cnt,main_item_cnt;
		last_s_item_cnt=bcks_num%OneStage;//最后一个次block中条目的数量，如果为0，其实对于满条目
		main_item_cnt=(bcks_num-last_s_item_cnt)/OneStage;
		if(last_s_item_cnt){
			main_item_cnt++;//主buf中条目的数量
		}
		readed_cnt=0;
		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);//载入主block
		for(int i=0;i<main_item_cnt;i++){//遍历主block
			DownloadStageBlockBuf(bbuf_secnd,bbuf_main[i]-1);//载入次block
			for(int j=0;j<OneStage;j++){//遍历整个次block
				if(readed_cnt==bcks_num-1){
					DownloadBlockByte(dFile,bbuf_secnd[j]-1,d512_rmn);
					break;//结束标志
				}
				else{
					DownloadBlockByte(dFile,bbuf_secnd[j]-1);
					readed_cnt++;
				}
			}
		}
	}
	//>512*512,三个中间
	else{
int tmp,main_item_cnt;//;
	
tmp=bcks_num%TwoStage;
main_item_cnt=(bcks_num-tmp)/TwoStage;//主block数量
if(tmp){
	main_item_cnt++;
}
readed_cnt=0;
DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);//载入主block
for(int i=0;i<main_item_cnt;i++){//遍历主block
	DownloadStageBlockBuf(bbuf_secnd,bbuf_main[i]-1);//载入次block
	for(int j=0;j<OneStage;j++){//遍历整个次block
		DownloadStageBlockBuf(bbuf_third,bbuf_secnd[j]-1);//载入第三block
		for(int k=0;k<OneStage;k++){//遍历第三block
			if(readed_cnt==bcks_num-1){
				DownloadBlockByte(dFile,bbuf_third[k]-1,d512_rmn);//读取数据
				return JNE_Correct;//结束标志
			}
			else{
				DownloadBlockByte(dFile,bbuf_third[k]-1);//读取数据
				readed_cnt++;
			}
		}//end 第三
	}//end次
}//end 主
	}
	return  JNE_Correct;
}
JN_Error JuggEngine::DownloadNamedFile(CFile &dFile,const JuggNode* pEnvNode,LPCWSTR nm)const{
	//第1步，预判断	
	if(!pEnvNode){
		return JNE_NullPoint;
	}
	if(!(pEnvNode->jn_mark&0x01)){
		return JNE_NotAFolder;//非文件夹
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

JN_Error JuggEngine::MoveNode(JuggNode *pEnvNode,JuggNode* pBeChild){//移动文件或文件夹
	if(!pEnvNode||! pBeChild){
		return JNE_NullPoint;
	}
	if(!(pEnvNode->jn_mark&0x01)){
		return JNE_NotAFolder;//非文件夹
	}
	int folder_flag=0;
	if(pBeChild->IsFolder()){
		folder_flag=1;
	}
	JuggNode*pTemp=pEnvNode->SameNamerInFolder(pBeChild->jn_name,folder_flag);
	if(pTemp){
		return JNE_ExistSameName;
	}

	//1，-----------------原child所在的变化------------------
	JuggNode *pPrnt=pBeChild->m_pParent;
	JuggNode *pRChld=pBeChild->m_pRChild;
	switch(pBeChild->m_ParentType){
	case JuggNode::JN_NoParent:return JNE_NotDeletable;//根节点，无法删除/移动
	case JuggNode::JN_LChilde://作为左孩子
		if(pRChld){//还有并列兄弟，设置连接
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
	case JuggNode::JN_RChilde://右孩子
		if(pRChld){//还有并列兄弟
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
	//保存父节点--冗余操作，因为很多信息都没变
	SaveJNode2Disk(pPrnt);
	//保存右孩子
	if(pRChld){
		SaveJNode2Disk(pRChld);
	}

	pBeChild->m_pRChild=NULL;//原兄弟失去联系 作节点不变
	pBeChild->jn_rchild=0;

	//---------------2添加操作 新child所在的变化-------------------------
	if(pEnvNode->m_pLChild){
		//找到左节点最右的兄弟
		pTemp=pEnvNode->m_pLChild->GetLastRChild();//返回NULL表示没有兄弟
		if(!pTemp){
			pTemp=pEnvNode->m_pLChild;
		}
		//pTemp是新加节点的父亲
		pBeChild->SetPointRelative(pTemp,JuggNode::JN_RChilde);
		SaveJNode2Disk(pTemp);//修改了父节点，故要重新保存
	}
	else{
		//新建的左节点--pEnvNode是其父亲
		pBeChild->SetPointRelative(pEnvNode,JuggNode::JN_LChilde);
		SaveJNode2Disk(pEnvNode);
	}
	//保存移动过的节点
	SaveJNode2Disk(pBeChild);
	return JNE_Correct;
}

JN_Error JuggEngine::JN_DeleteFile(JuggNode *pNode){//文件夹和文件统一操作
	if(!pNode){
		return JNE_NullPoint;
	}

	JuggNode *pPrnt=pNode->m_pParent;
	JuggNode *pRChld=pNode->m_pRChild;

	switch(pNode->m_ParentType){
	case JuggNode::JN_NoParent:return JNE_NotDeletable;//根节点，无法删除
	case JuggNode::JN_LChilde://作为左孩子
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
	case JuggNode::JN_RChilde://右孩子
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
	//保存父节点--冗余操作，因为很多信息都没变
	SaveJNode2Disk(pPrnt);
	//保存右孩子
	if(pRChld){
		SaveJNode2Disk(pRChld);
	}
	//切断联系（非必须）
	pNode->jn_rchild=0;
	pNode->m_pRChild=NULL;

	pNode->m_pParent=NULL;
	pNode->m_ParentType=JuggNode::JN_NoParent;
	
	//删除当前节点
	if(pNode->jn_mark&0x01){//文件夹--操作--递归
		DeleteDiscardedFolder(pNode);
	}
	else{
		//回收node bitmap
		SetNodeBitmapFree(pNode->m_nodeLoc-1);
		SaveNodeBitmap2Disk();
		//回收block
		DeleteDiscardedFileBlocks(pNode);
		SaveBlockBitmap2Disk();
		delete pNode;
	}
	m_pFile->Flush();
	return JNE_Correct;
}

BOOL JuggEngine::DeleteDiscardedFileBlocks(JuggNode*pNode){//是一个文件来着
	//回收block bitmap
	Int blcks_num=pNode->jn_blocks_cnt;
	int OneStage=m_iBlockSize/4;
	int TwoStage=OneStage*OneStage;
	int main_item_cnt,temp,del_block_Cnt=0;

	if(blcks_num<=15){//直接索引
		for(int i=0;i<blcks_num;i++){
			SetBlockBitmapFree(pNode->jn_block[i]-1);
		}
	}
	else if(blcks_num<=OneStage){
		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);
		SetBlockBitmapFree(pNode->jn_block[0]-1);
		for(int i=0;i<blcks_num;i++){
			SetBlockBitmapFree(bbuf_main[i]-1);//数据block
		}
	}
	//============二级索引=============
	else if(blcks_num<=TwoStage){
		temp=blcks_num%OneStage;
		main_item_cnt=(blcks_num-temp)/OneStage;
		if(temp) {main_item_cnt++;}//main_buf中的个数
		del_block_Cnt=0;

		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);
		SetBlockBitmapFree(pNode->jn_block[0]-1);//主block也删除
		for(int i=0;i<main_item_cnt;i++){
			DownloadStageBlockBuf(bbuf_secnd,bbuf_main[i]-1);
			SetBlockBitmapFree(bbuf_main[i]-1);//次索引block
			for(int j=0;j<OneStage;j++){
				SetBlockBitmapFree(bbuf_secnd[j]-1);//数据block
				del_block_Cnt++;
				if(del_block_Cnt==blcks_num){//删除blcks数量已到
					break;
				}
			}
		}
	}
	//------------------------------
	//=======三级索引======
	else{
		temp=blcks_num%TwoStage;
		main_item_cnt=(blcks_num-temp)/TwoStage;
		if(temp) {main_item_cnt++;}//main_buf中的个数

		DownloadStageBlockBuf(bbuf_main,pNode->jn_block[0]-1);
		SetBlockBitmapFree(pNode->jn_block[0]-1);//删除主block

		for(int i=0;i<main_item_cnt;i++){
			DownloadStageBlockBuf(bbuf_secnd,bbuf_main[i]-1);
			SetBlockBitmapFree(bbuf_main[i]-1);//删除次索引block
			for(int j=0;j<OneStage;j++){
				DownloadStageBlockBuf(bbuf_third,bbuf_secnd[j]-1);
				SetBlockBitmapFree(bbuf_secnd[j]-1);//删除次索引block
				for(int k=0;k<OneStage;k++){
					SetBlockBitmapFree(bbuf_third[k]-1);//删除数据block
					del_block_Cnt++;
					if(del_block_Cnt==blcks_num){//删除blcks数量已到
						j=OneStage;
						break;
					}
				}
			}
		}
	}
	//最后保存bitmap到disk
	return TRUE;
}

BOOL JuggEngine::DeleteDiscardedFolder(JuggNode*pNode){//删除一个脱离目录树的文件夹--需要递归--但node善未回收
	//遍历整个目录
	stack<JuggNode*> jn_stack;
	JuggNode* pJN=pNode;
	JuggNode* pDel;
	while(pJN||!jn_stack.empty()){//栈非空，当前有指向
		if(pJN){
			jn_stack.push(pJN);
			pJN=pJN->m_pLChild;
		}
		else{
			pDel=jn_stack.top();
			jn_stack.pop();
			pJN=pDel->m_pRChild;

			SetNodeBitmapFree(pDel->m_nodeLoc-1);//删除node==设置bitmap位
			if(!(pDel->jn_mark&0x01)){
				DeleteDiscardedFileBlocks(pDel);//删文件，文件夹不需要回收block
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
		m_pFile->Read(buf,cpy_byte);//先读再写
		dFile.Write(buf,cpy_byte);
		//dFile.Flush();
		return TRUE;
	}
	catch(...){
		return FALSE;
	}
}
BOOL JuggEngine::CopyBlockByte(Int dst_block0,CFile&sFile, Int cpy_byte){//源文件不在seek了
	ASSERT(dst_block0>=0);
	if(cpy_byte==0){
		cpy_byte=m_iBlockSize;
	}
	try{
		//sFile.Seek(src,CFile::begin);
		m_pFile->Seek(jr_block_table+dst_block0*m_iBlockSize,CFile::begin);
		sFile.Read(buf,cpy_byte);//先读再写
		m_pFile->Write(buf,cpy_byte);
		//m_pFile->Flush();
		return TRUE;
	}
	catch(...){
		return FALSE;
	}
}
BOOL JuggEngine::DownloadStageBlockBuf(Int *bbuf,Int src_block0)const{//总是读取一个block的内容
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



void JuggEngine::RealseDynamicBuf(){//其实与析构函数一样
	if(m_pNodeBitmap){
		delete [] m_pNodeBitmap;
		m_pNodeBitmap=NULL;
	}
	if(m_pBlockBitmap){
		delete [] m_pBlockBitmap;
		m_pBlockBitmap=NULL;
	}

	if(bbuf_main){
		delete []bbuf_main;//每块大小为block的大小
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
//	if(buf){这个就不删了，常用
//		delete []buf;
//	}
}

void JuggEngine::GetJNErrorString(CString &err,JN_Error  jnerror){
	switch(jnerror){
	case JNE_Correct: break;
	case JNE_TooLongName :err=_T("文件名过长"); break; 
	case  JNE_NullPoint:err=_T("空指针"); break;
	case JNE_TooBigFile :err=_T("文件过大"); break; 
	case  JNE_NotAFile :err=_T("不是一个文件"); break; 
	case  JNE_NoSameNameFile :err=_T("不存在该名称的文件"); break; 
	case  JNE_NEnoughNode :err=_T("目录节点数不足"); break; 
	case  JNE_ExistSameName :err=_T("存在同名文件/文件夹"); break; 
	case  JNE_NEnoughBlock :err=_T("磁盘容量不足"); break; 
	case  JNE_NotAFolder:err=_T("不是一个文件夹"); break;
	case JNE_NotDeletable:err=_T("无法删除"); break;
	default:break;
	}
}