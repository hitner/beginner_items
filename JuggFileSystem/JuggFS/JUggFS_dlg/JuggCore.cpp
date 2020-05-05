#include"JuggCore.h"
#include"string.h"

Bool JuggRule::InitRule(Int disksz,Int blocksz){//其它参数不可变
	int atmp,btmp=0;
	//jr_modetype;
	if(disksz<=0){
		return False;
	}

	if(blocksz!=1&&blocksz!=2&&blocksz!=4){
		return False;
	}

	jr_modetype=MODE_TYPE_VALUE;
	jr_disk_size=disksz;//单位是MB

	jr_block_size=blocksz;//单位是KB


	jr_node_size=4;//单位是64的倍数
	jr_nodes_cnt=32768;//需要自适应设置的，先固定化32K个 对应4K的bitmap
	//jr_free_nodes_cnt=32768;
	jr_node_bitmap=1024;
	jr_node_table=jr_node_bitmap+jr_nodes_cnt/8;//以块为单位计算

	jr_block_bitmap=jr_node_table+jr_nodes_cnt*jr_node_size*64;


	if(jr_block_bitmap%(jr_block_size*1024)){//除得尽的情形
		atmp=(disksz*1024)/jr_block_size;//总数
		btmp=(jr_block_bitmap/1024)/jr_block_size;//已用
		atmp=atmp-btmp;//剩余

		btmp=atmp%(8192*jr_block_size+1);
		btmp=1+(atmp-btmp)/(8192*jr_block_size+1);//位图使用块数

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
	(*pAr)<<jr_modetype<<jr_disk_size;//单位是MB

	(*pAr)<<jr_block_size<<jr_blocks_cnt<<jr_free_blocks_cnt<<jr_block_bitmap<<jr_block_table;

	(*pAr)<<jr_node_size<<jr_nodes_cnt<<jr_free_nodes_cnt<<jr_node_bitmap<<jr_node_table;//以块为单位计算
	*/
	pf->SeekToBegin();
	pf->Write(this,sizeof(JuggRule)-8);
	pf->Flush();
	return True;
}

Bool JuggRule::LoadJRule(CFile *pf){
	/*
	(*pAr)>>jr_modetype>>jr_disk_size;//单位是MB

	(*pAr)>>jr_block_size>>jr_blocks_cnt>>jr_free_blocks_cnt>>jr_block_bitmap>>jr_block_table;

	(*pAr)>>jr_node_size>>jr_nodes_cnt>>jr_free_nodes_cnt>>jr_node_bitmap>>jr_node_table;//以块为单位计算
	*/
	pf->SeekToBegin();
	pf->Read(this,sizeof(JuggRule)-8);
	//进行检查
	if(pf->GetLength()!=jr_disk_size*MB_SIZE){//文件大小不匹配
		return False;
	}
	if(jr_modetype!=MODE_TYPE_VALUE){
		return False;
	}
	if(jr_block_bitmap>=jr_block_table||jr_node_bitmap>=jr_node_table){
		return False;
	}

	//计数bitmap长度
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
		return False;//文件名过长
	}
	wcscpy(jn_name,filename);
	m_nodeLoc=nLoc1;
	if(direc_flag==0){
		jn_mark=0;
	}
	else{
		jn_mark=1;
		jn_size=0;
		jn_blocks_cnt=0;//文件夹这些都没有
	}

	//各类时间初始化----暂略
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
	m_ParentType=JN_NoParent;//指示双亲是文件夹还是并列文件
	m_nodeLoc=0;//在fs中的位置
	//=============

	//fs中关键信息类
	//=============
	jn_lchild=0;
	jn_rchild=0;
	//jn_parent=0;
		//文件信息组
	memset(jn_name,0,2*Max_Name_Len+2);

	jn_mark=0;//位指示，最低位指示是否为文件夹

	jn_size=0;// // Size in bytes

	jn_atime=0;//  Access time 
	jn_ctime=0;// ULONG Creation time 
	jn_mtime=0;// ULONG Modification time 

	jn_blocks_cnt=0;// ULONG Blocks count 
	memset(jn_block,0,60);
	//jn_block[15];// Pointers to blocks
}

//文件夹下面寻找同名文件夹或文件 （不深入子文件夹）
JuggNode* JuggNode::SameNamerInFolder(JPCWStr nm,int direct_flag){
	return const_cast<JuggNode *>(static_cast<const JuggNode&>(*this).SameNamerInFolder(nm,direct_flag));
}
 const JuggNode* JuggNode::SameNamerInFolder(JPCWStr nm,int direct_flag)const{
	JuggNode *pNode;
	pNode=m_pLChild;//到左节点
	for(;pNode;pNode=pNode->m_pRChild){
		if(direct_flag){
			if(pNode->jn_mark&0x01){//文件夹
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

//内存中指针 和 文件中指针都设置了
void JuggNode::SetPointRelative(JuggNode*pParent,JNParentType parent_tp){
	m_pParent=pParent;
	m_ParentType=parent_tp;
	//this指针作为孩子
	if(parent_tp==JN_LChilde){
		pParent->m_pLChild=this;
		pParent->jn_lchild=m_nodeLoc;//此项目改变必须得写入文件中
	}
	else if(parent_tp==JN_RChilde){
		pParent->m_pRChild=this;
		pParent->jn_rchild=m_nodeLoc;//这也要写入文件中
	}
}

//获得最右边的孩子，其实是寻找一个文件夹中最后一个元素
JuggNode* JuggNode::GetLastRChild(){//如果没有右孩子，返回NULL
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
JuggNode* JuggNode::GetUpFolderNode(){//上层目录节点
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