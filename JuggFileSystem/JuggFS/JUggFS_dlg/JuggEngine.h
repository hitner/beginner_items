#ifndef _JUGGENGINE__
#define _JUGGENGINE__

#include "TypeDef.h"
#include "stdafx.h"
//
#include "JuggCore.h"
#include "ErrorMark.h"

//"文件系统启动器
class JuggEngine:public JuggRule{
public:
	JuggEngine();
	~JuggEngine();
	BOOL PreHandle(CFile &cfile);
	//disk_size以MB为单位；block_size以KB为单位，默认为2;
	BOOL CreateDisk(Int disk_size,Int block_size);//创建Disk


	BOOL ReadOneJNode(JuggNode*pJNode,Int node_loc);//从disk读取指定Node

	BOOL RenameFile(LPCTSTR target,LPCTSTR newname);

	BOOL OpenDisk();
	BOOL CloseDisk();//主要是要释放那些new出来的空间
	//void		Close();
	//BOOL CreatFolder(LPCWSTR target,LPCWSTR nm);//支持的是字符串形式 命令行方式
	JN_Error CreatFolder(JuggNode* pEnvNode,LPCWSTR nm);//该目录下创建
	JN_Error AddFile(JuggNode* pEnvNode,CFile &sFile);
	JN_Error DownloadNamedFile(CFile &dFile,const JuggNode* pEnvNode,LPCWSTR nm)const;//下载文件夹下指定文件名的文件

	JN_Error DownloadNodedFile(CFile &dFile,const JuggNode* pNode)const; 
	JN_Error JN_DeleteFile(JuggNode *pNode);//文件夹和文件统一操作

	JN_Error MoveNode(JuggNode *pEnvNode,JuggNode* pBeChild);//移动文件或文件夹
	

	void GetJNErrorString(CString &err,JN_Error  jnerror);
	//void PrintfBlockBitmap()const;//仅调试用 需要cout了

	//BOOL FormatDisk();
	
	
	//
	JuggNode* GetRootNode();

private:
	static const Int FILE_BUF_LEN=10240;
	BOOL ReadAllJNode();
	int CalculateFreeEntries(const UChar *src,int bit_len);//计算剩余block/node数量
	BOOL DownloadBlockByte(CFile &dFile,Int src_block0,Int cpy_byte=0)const;//目标文件不在移动指针
	BOOL CopyBlockByte(Int block_id,CFile&sFile,Int copy_byte=0);//默认0表示复制block_size的大小

	BOOL DownloadStageBlockBuf(Int *bbuf,Int dst_block0)const;//总是读取一个block的内容
	BOOL CopyStageBlockBuf(Int dst_block0,Int *bbuf, Int cpy_num);

	BOOL SaveBlockBtimapAndFileCont(JuggNode*pNode,CFile&sFile,Int blocks_cnt,Int remain_size);//保存文件的第二层调用，只是文件内容和位图
	BOOL SaveJNode2Disk(JuggNode*pJNode);//同时保存位图
	BOOL SaveBlockBitmap2Disk(int endBitLoc0=-1);//保存block的位图，注意node的位图总是在SaveJNode2Disk中搞定,-1表示保存所有的bitmap
	BOOL SaveNodeBitmap2Disk();

	int GetSetFirstEmptyNode();//是内存中0开始的序列
	int GetSetFirstEmptyBlock(int *begLoc=NULL);//是内存中0开始的序列
	void SetNodeBitmapFree(Int loc0);//释放指定位置的bitmap
	void SetBlockBitmapFree(Int loc0);


	BOOL DeleteDiscardedFileBlocks(JuggNode*pNode);//删除一个实在的文件,该文件已经与目录树脱离关系
	BOOL DeleteDiscardedFolder(JuggNode*pNode);//删除一个脱离目录树的文件夹--需要递归--但node善未回收

	void RealseDynamicBuf();
private:
	//JuggRule m_JRule;
	enum ParentType{LChildP,RChildP};
	
	CFile *m_pFile;
	//CArchive *m_pStoreAr;
	//CArchive *m_pLoadAr;

	UChar *m_pNodeBitmap;//两个位图
	UChar *m_pBlockBitmap;

	JuggNode *m_pRootNode;//根目录
	JuggNode *m_pCurFolder;//当前目录

	Int		m_FreeBlocksCnt;
	Int		m_FreeNodesCnt;
	Int		m_iBlockSize;

	mutable UChar *buf;
	mutable Int * bbuf_main;//每块大小为block/4的大小
	mutable Int * bbuf_secnd;
	mutable Int * bbuf_third;



};

#endif