#include"JuggCore.h"
#include"string.h"

Bool JuggRule::InitRule(Int disksz,Int blocksz){//�����������ɱ�
	int atmp,btmp=0;
	//jr_modetype;
	if(disksz<=0){
		return False;
	}

	if(blocksz!=1&&blocksz!=2&&blocksz!=4){
		return False;
	}

	jr_modetype=MODE_TYPE_VALUE;
	jr_disk_size=disksz;//��λ��MB

	jr_block_size=blocksz;//��λ��KB


	jr_node_size=4;//��λ��64�ı���
	jr_nodes_cnt=32768;//��Ҫ����Ӧ���õģ��ȹ̶���32K�� ��Ӧ4K��bitmap
	//jr_free_nodes_cnt=32768;
	jr_node_bitmap=1024;
	jr_node_table=jr_node_bitmap+jr_nodes_cnt/8;//�Կ�Ϊ��λ����

	jr_block_bitmap=jr_node_table+jr_nodes_cnt*jr_node_size*64;


	if(jr_block_bitmap%(jr_block_size*1024)){//���þ�������
		atmp=(disksz*1024)/jr_block_size;//����
		btmp=(jr_block_bitmap/1024)/jr_block_size;//����
		atmp=atmp-btmp;//ʣ��

		btmp=atmp%(8192*jr_block_size+1);
		btmp=1+(atmp-btmp)/(8192*jr_block_size+1);//λͼʹ�ÿ���

		jr_blocks_cnt=atmp-btmp;
		//jr_free_blocks_cnt=jr_blocks_cnt;
		jr_block_table=jr_block_bitmap+btmp*1024*jr_block_size;
	}
	else return False;
	

	atmp=jr_nodes_cnt%8;
	jr_nodebitmap_size=(jr_nodes_cnt-atmp)/8;
	if(atmp) jr_nodebitmap_size++;

	atmp=jr_blocks_cnt%8;
	jr_blockbitmap_size=(jr_blocks_cnt-atmp)/8;
	if(atmp) jr_blockbitmap_size++;

	return True;
}

Bool JuggRule::SaveJRule2File(CFile *pf){
	/*
	(*pAr)<<jr_modetype<<jr_disk_size;//��λ��MB

	(*pAr)<<jr_block_size<<jr_blocks_cnt<<jr_free_blocks_cnt<<jr_block_bitmap<<jr_block_table;

	(*pAr)<<jr_node_size<<jr_nodes_cnt<<jr_free_nodes_cnt<<jr_node_bitmap<<jr_node_table;//�Կ�Ϊ��λ����
	*/
	pf->SeekToBegin();
	pf->Write(this,sizeof(JuggRule)-8);
	pf->Flush();
	return True;
}

Bool JuggRule::LoadJRule(CFile *pf){
	/*
	(*pAr)>>jr_modetype>>jr_disk_size;//��λ��MB

	(*pAr)>>jr_block_size>>jr_blocks_cnt>>jr_free_blocks_cnt>>jr_block_bitmap>>jr_block_table;

	(*pAr)>>jr_node_size>>jr_nodes_cnt>>jr_free_nodes_cnt>>jr_node_bitmap>>jr_node_table;//�Կ�Ϊ��λ����
	*/
	pf->SeekToBegin();
	pf->Read(this,sizeof(JuggRule)-8);
	//���м��
	if(pf->GetLength()!=jr_disk_size*MB_SIZE){//�ļ���С��ƥ��
		return False;
	}
	if(jr_modetype!=MODE_TYPE_VALUE){
		return False;
	}
	if(jr_block_bitmap>=jr_block_table||jr_node_bitmap>=jr_node_table){
		return False;
	}

	//����bitmap����
	int atmp=jr_nodes_cnt%8;
	jr_nodebitmap_size=(jr_nodes_cnt-atmp)/8;
	if(atmp) jr_nodebitmap_size++;

	atmp=jr_blocks_cnt%8;
	jr_blockbitmap_size=(jr_blocks_cnt-atmp)/8;
	if(atmp) jr_blockbitmap_size++;
	
	return True;
}

Bool JuggNode::Initialize(JPCWStr filename,Int nLoc1,Int direc_flag){
	if(wcslen(filename)>JuggNode::Max_Name_Len){
		return False;//�ļ�������
	}
	wcscpy(jn_name,filename);
	m_nodeLoc=nLoc1;
	if(direc_flag==0){
		jn_mark=0;
	}
	else{
		jn_mark=1;
		jn_size=0;
		jn_blocks_cnt=0;//�ļ�����Щ��û��
	}

	//����ʱ���ʼ��----����
	jn_atime=time(NULL);//  Access time 
	jn_ctime=time(NULL);// ULONG Creation time 
	jn_mtime=time(NULL);// ULONG Modification time

	return True;
}
JuggNode::ChildType JuggNode::GetNextReadChild(){
	ChildType ctp=NoChild;
	if(jn_lchild==0){
		if(jn_rchild!=0){
			ctp=OnlyRChild;
		}
	}
	else{
		if(jn_rchild!=0){
			ctp=BothChilds;
		}
		else{
			ctp=OnlyLChild;
		}

	}
	return ctp;
}

JuggNode::JuggNode(){
	m_pLChild=NULL;
	m_pRChild=NULL;
	m_pParent=NULL;
	m_ParentType=JN_NoParent;//ָʾ˫�����ļ��л��ǲ����ļ�
	m_nodeLoc=0;//��fs�е�λ��
	//=============

	//fs�йؼ���Ϣ��
	//=============
	jn_lchild=0;
	jn_rchild=0;
	//jn_parent=0;
		//�ļ���Ϣ��
	memset(jn_name,0,2*Max_Name_Len+2);

	jn_mark=0;//λָʾ�����λָʾ�Ƿ�Ϊ�ļ���

	jn_size=0;// // Size in bytes

	jn_atime=0;//  Access time 
	jn_ctime=0;// ULONG Creation time 
	jn_mtime=0;// ULONG Modification time 

	jn_blocks_cnt=0;// ULONG Blocks count 
	memset(jn_block,0,60);
	//jn_block[15];// Pointers to blocks
}

//�ļ�������Ѱ��ͬ���ļ��л��ļ� �����������ļ��У�
JuggNode* JuggNode::SameNamerInFolder(JPCWStr nm,int direct_flag){
	return const_cast<JuggNode *>(static_cast<const JuggNode&>(*this).SameNamerInFolder(nm,direct_flag));
}
 const JuggNode* JuggNode::SameNamerInFolder(JPCWStr nm,int direct_flag)const{
	JuggNode *pNode;
	pNode=m_pLChild;//����ڵ�
	for(;pNode;pNode=pNode->m_pRChild){
		if(direct_flag){
			if(pNode->jn_mark&0x01){//�ļ���
				if(0==wcscmp(nm,pNode->jn_name)){
					return pNode;
				}
			}
		}
		else {
			if(!(pNode->jn_mark&0x01)){
				if(0==wcscmp(nm,pNode->jn_name)){
					return pNode;
				}
			}
		}


	}
	return NULL;

}

//�ڴ���ָ�� �� �ļ���ָ�붼������
void JuggNode::SetPointRelative(JuggNode*pParent,JNParentType parent_tp){
	m_pParent=pParent;
	m_ParentType=parent_tp;
	//thisָ����Ϊ����
	if(parent_tp==JN_LChilde){
		pParent->m_pLChild=this;
		pParent->jn_lchild=m_nodeLoc;//����Ŀ�ı�����д���ļ���
	}
	else if(parent_tp==JN_RChilde){
		pParent->m_pRChild=this;
		pParent->jn_rchild=m_nodeLoc;//��ҲҪд���ļ���
	}
}

//������ұߵĺ��ӣ���ʵ��Ѱ��һ���ļ��������һ��Ԫ��
JuggNode* JuggNode::GetLastRChild(){//���û���Һ��ӣ�����NULL
	JuggNode* pTmp;
	pTmp=m_pRChild;
	while(pTmp){
		if(pTmp->m_pRChild){
			pTmp=pTmp->m_pRChild;
		}
		else {
			return pTmp;
		}
	}
	return NULL;
}
JuggNode* JuggNode::GetUpFolderNode(){//�ϲ�Ŀ¼�ڵ�
	JuggNode* pTmp=this;
	while(pTmp->m_pParent){
		if(pTmp->m_ParentType==JN_LChilde){
			return pTmp->m_pParent;
		}
		else{
			pTmp=pTmp->m_pParent;
		}
	}
	return NULL;

}
Bool JuggNode::IsFolder()const{

	if(jn_mark&0x01){
		return True;
	}
	else{
		return False;
	}

}