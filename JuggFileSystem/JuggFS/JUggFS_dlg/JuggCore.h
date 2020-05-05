#ifndef _JUGGCORE__
#define _JUGGCORE__ 

#include "TypeDef.h"
#include "stdafx.h"
/**按照盘组成方式的顺序来定义各个类
*/

//============
//相当于ext2中的superblock
//============
class JuggRule{
public:
	Int		jr_modetype;
	Int		jr_disk_size;//单位是MB

	UChar	jr_block_size;//单位是KB
	Int		jr_blocks_cnt;//最多2G
	
	UInt		jr_block_bitmap;
	UInt		jr_block_table;

	UChar	jr_node_size;//单位是64的倍数
	Int		jr_nodes_cnt;
	

	UInt		jr_node_bitmap;
	UInt		jr_node_table;//以块为单位计算

	//Char		jr_pad[982];//凑齐1024--内存中不需要载入这个东西
//多余信息组

	Int		jr_nodebitmap_size;
	Int		jr_blockbitmap_size;
public:
#define MODE_TYPE_VALUE 0x0000F038
	Bool InitRule(Int disksz,Int blocksz);//其它参数不可变,相当于格式化
	Bool SaveJRule2File(CFile *pAr);
	Bool LoadJRule(CFile*pAr);
	
};

//目录（文件）节点
class JuggNode{
public:
	JuggNode();
	Bool Initialize(JPCWStr filename,Int nLoc1,Int direc_flag=0);
	enum ChildType{NoChild,OnlyLChild,OnlyRChild,BothChilds};
	ChildType GetNextReadChild();
	enum JNParentType{JN_NoParent,JN_LChilde,JN_RChilde};

	const JuggNode* SameNamerInFolder(JPCWStr nm,int direct_flag=0)const;
	JuggNode* SameNamerInFolder(JPCWStr nm,int direct_flag=0);
	void SetPointRelative(JuggNode*pParent,JNParentType parent_tp);//由孩子调用
	JuggNode* GetLastRChild();//如果没有右孩子，返回NULL
	JuggNode* GetUpFolderNode();//上层目录节点
	Bool IsFolder()const;
//
//private:
	//=============
	//新增处理数据
	JuggNode* m_pLChild;
	JuggNode* m_pRChild;
	JuggNode* m_pParent;
	JNParentType m_ParentType;//指示双亲是文件夹还是并列文件
	Int m_nodeLoc;//在fs中的位置--1为有效值
	//=============
	//=============
	static const  Int Max_Name_Len=80;
	//fs中关键信息类
	//=============
	Int jn_lchild;
	Int jn_rchild;
	//Int jn_parent;
		//文件信息组
	WChar jn_name[Max_Name_Len+1];

	UChar jn_mark;//位指示，最低位指示是否为文件夹

	Int64 jn_size;// // Size in bytes  8位

	UInt jn_atime;//  Access time 
	UInt jn_ctime;// ULONG Creation time 
	UInt jn_mtime;// ULONG Modification time 

	Int jn_blocks_cnt;// ULONG Blocks count 
	Int jn_block[15];// Pointers to blocks 
};

const Int64 MB_SIZE=1048576;
#endif
