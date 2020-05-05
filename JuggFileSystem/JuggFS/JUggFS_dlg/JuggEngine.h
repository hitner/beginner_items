#ifndef _JUGGENGINE__
#define _JUGGENGINE__

#include "TypeDef.h"
#include "stdafx.h"
//
#include "JuggCore.h"
#include "ErrorMark.h"

//"�ļ�ϵͳ������
class JuggEngine:public JuggRule{
public:
	JuggEngine();
	~JuggEngine();
	BOOL PreHandle(CFile &cfile);
	//disk_size��MBΪ��λ��block_size��KBΪ��λ��Ĭ��Ϊ2;
	BOOL CreateDisk(Int disk_size,Int block_size);//����Disk


	BOOL ReadOneJNode(JuggNode*pJNode,Int node_loc);//��disk��ȡָ��Node

	BOOL RenameFile(LPCTSTR target,LPCTSTR newname);

	BOOL OpenDisk();
	BOOL CloseDisk();//��Ҫ��Ҫ�ͷ���Щnew�����Ŀռ�
	//void		Close();
	//BOOL CreatFolder(LPCWSTR target,LPCWSTR nm);//֧�ֵ����ַ�����ʽ �����з�ʽ
	JN_Error CreatFolder(JuggNode* pEnvNode,LPCWSTR nm);//��Ŀ¼�´���
	JN_Error AddFile(JuggNode* pEnvNode,CFile &sFile);
	JN_Error DownloadNamedFile(CFile &dFile,const JuggNode* pEnvNode,LPCWSTR nm)const;//�����ļ�����ָ���ļ������ļ�

	JN_Error DownloadNodedFile(CFile &dFile,const JuggNode* pNode)const; 
	JN_Error JN_DeleteFile(JuggNode *pNode);//�ļ��к��ļ�ͳһ����

	JN_Error MoveNode(JuggNode *pEnvNode,JuggNode* pBeChild);//�ƶ��ļ����ļ���
	

	void GetJNErrorString(CString &err,JN_Error  jnerror);
	//void PrintfBlockBitmap()const;//�������� ��Ҫcout��

	//BOOL FormatDisk();
	
	
	//
	JuggNode* GetRootNode();

private:
	static const Int FILE_BUF_LEN=10240;
	BOOL ReadAllJNode();
	int CalculateFreeEntries(const UChar *src,int bit_len);//����ʣ��block/node����
	BOOL DownloadBlockByte(CFile &dFile,Int src_block0,Int cpy_byte=0)const;//Ŀ���ļ������ƶ�ָ��
	BOOL CopyBlockByte(Int block_id,CFile&sFile,Int copy_byte=0);//Ĭ��0��ʾ����block_size�Ĵ�С

	BOOL DownloadStageBlockBuf(Int *bbuf,Int dst_block0)const;//���Ƕ�ȡһ��block������
	BOOL CopyStageBlockBuf(Int dst_block0,Int *bbuf, Int cpy_num);

	BOOL SaveBlockBtimapAndFileCont(JuggNode*pNode,CFile&sFile,Int blocks_cnt,Int remain_size);//�����ļ��ĵڶ�����ã�ֻ���ļ����ݺ�λͼ
	BOOL SaveJNode2Disk(JuggNode*pJNode);//ͬʱ����λͼ
	BOOL SaveBlockBitmap2Disk(int endBitLoc0=-1);//����block��λͼ��ע��node��λͼ������SaveJNode2Disk�и㶨,-1��ʾ�������е�bitmap
	BOOL SaveNodeBitmap2Disk();

	int GetSetFirstEmptyNode();//���ڴ���0��ʼ������
	int GetSetFirstEmptyBlock(int *begLoc=NULL);//���ڴ���0��ʼ������
	void SetNodeBitmapFree(Int loc0);//�ͷ�ָ��λ�õ�bitmap
	void SetBlockBitmapFree(Int loc0);


	BOOL DeleteDiscardedFileBlocks(JuggNode*pNode);//ɾ��һ��ʵ�ڵ��ļ�,���ļ��Ѿ���Ŀ¼�������ϵ
	BOOL DeleteDiscardedFolder(JuggNode*pNode);//ɾ��һ������Ŀ¼�����ļ���--��Ҫ�ݹ�--��node��δ����

	void RealseDynamicBuf();
private:
	//JuggRule m_JRule;
	enum ParentType{LChildP,RChildP};
	
	CFile *m_pFile;
	//CArchive *m_pStoreAr;
	//CArchive *m_pLoadAr;

	UChar *m_pNodeBitmap;//����λͼ
	UChar *m_pBlockBitmap;

	JuggNode *m_pRootNode;//��Ŀ¼
	JuggNode *m_pCurFolder;//��ǰĿ¼

	Int		m_FreeBlocksCnt;
	Int		m_FreeNodesCnt;
	Int		m_iBlockSize;

	mutable UChar *buf;
	mutable Int * bbuf_main;//ÿ���СΪblock/4�Ĵ�С
	mutable Int * bbuf_secnd;
	mutable Int * bbuf_third;



};

#endif