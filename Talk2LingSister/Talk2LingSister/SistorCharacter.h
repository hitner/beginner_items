//对话对象文件保存内容，含id/ip等


#pragma once

class SistorCharacter{
public:
	SistorCharacter();
	~SistorCharacter();
	void Enrich(CArchive &ar);
	void Enrich(const CString & name,const CString &ip);
	void Sddtore(CArchive &ar);
	CString GetDisplayStr()const;
	void GetNameIP(CString & name,CString &ip)const ;//获得属性量
protected:
	CString m_name;
	CString m_ip;
};