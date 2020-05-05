#ifndef _JUGGCORE__
#define _JUGGCORE__ 

#include "TypeDef.h"
#include "stdafx.h"
/**��������ɷ�ʽ��˳�������������
*/

//============
//�൱��ext2�е�superblock
//============
class JuggRule{
public:
	Int		jr_modetype;
	Int		jr_disk_size;//��λ��MB

	UChar	jr_block_size;//��λ��KB
	Int		jr_blocks_cnt;//���2G
	
	UInt		jr_block_bitmap;
	UInt		jr_block_table;

	UChar	jr_node_size;//��λ��64�ı���
	Int		jr_nodes_cnt;
	

	UInt		jr_node_bitmap;
	UInt		jr_node_table;//�Կ�Ϊ��λ����

	//Char		jr_pad[982];//����1024--�ڴ��в���Ҫ�����������
//������Ϣ��

	Int		jr_nodebitmap_size;
	Int		jr_blockbitmap_size;
public:
#define MODE_TYPE_VALUE 0x0000F038
	Bool InitRule(Int disksz,Int blocksz);//�����������ɱ�,�൱�ڸ�ʽ��
	Bool SaveJRule2File(CFile *pAr);
	Bool LoadJRule(CFile*pAr);
	
};

//Ŀ¼���ļ����ڵ�
class JuggNode{
public:
	JuggNode();
	Bool Initialize(JPCWStr filename,Int nLoc1,Int direc_flag=0);
	enum ChildType{NoChild,OnlyLChild,OnlyRChild,BothChilds};
	ChildType GetNextReadChild();
	enum JNParentType{JN_NoParent,JN_LChilde,JN_RChilde};

	const JuggNode* SameNamerInFolder(JPCWStr nm,int direct_flag=0)const;
	JuggNode* SameNamerInFolder(JPCWStr nm,int direct_flag=0);
	void SetPointRelative(JuggNode*pParent,JNParentType parent_tp);//�ɺ��ӵ���
	JuggNode* GetLastRChild();//���û���Һ��ӣ�����NULL
	JuggNode* GetUpFolderNode();//�ϲ�Ŀ¼�ڵ�
	Bool IsFolder()const;
//
//private:
	//=============
	//������������
	JuggNode* m_pLChild;
	JuggNode* m_pRChild;
	JuggNode* m_pParent;
	JNParentType m_ParentType;//ָʾ˫�����ļ��л��ǲ����ļ�
	Int m_nodeLoc;//��fs�е�λ��--1Ϊ��Чֵ
	//=============
	//=============
	static const  Int Max_Name_Len=80;
	//fs�йؼ���Ϣ��
	//=============
	Int jn_lchild;
	Int jn_rchild;
	//Int jn_parent;
		//�ļ���Ϣ��
	WChar jn_name[Max_Name_Len+1];

	UChar jn_mark;//λָʾ�����λָʾ�Ƿ�Ϊ�ļ���

	Int64 jn_size;// // Size in bytes  8λ

	UInt jn_atime;//  Access time 
	UInt jn_ctime;// ULONG Creation time 
	UInt jn_mtime;// ULONG Modification time 

	Int jn_blocks_cnt;// ULONG Blocks count 
	Int jn_block[15];// Pointers to blocks 
};

const Int64 MB_SIZE=1048576;
#endif
