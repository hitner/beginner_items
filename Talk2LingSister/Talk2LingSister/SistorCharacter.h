//�Ի������ļ��������ݣ���id/ip��


#pragma once

class SistorCharacter{
public:
	SistorCharacter();
	~SistorCharacter();
	void Enrich(CArchive &ar);
	void Enrich(const CString & name,const CString &ip);
	void Sddtore(CArchive &ar);
	CString GetDisplayStr()const;
	void GetNameIP(CString & name,CString &ip)const ;//���������
protected:
	CString m_name;
	CString m_ip;
};