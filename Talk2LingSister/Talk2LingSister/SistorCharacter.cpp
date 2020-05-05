#include"stdafx.h"
#include"SistorCharacter.h"


SistorCharacter::SistorCharacter(){
}

SistorCharacter::~SistorCharacter(){
}

void SistorCharacter::Enrich(CArchive &ar){
	ar>>m_name>>m_ip;
	ar.Flush();
}

void SistorCharacter::Enrich(const CString & name,const CString &ip){
	m_name=name;
	m_ip=ip;
}

void SistorCharacter::Sddtore(CArchive &ar){
	ar<<m_name<<m_ip;
	ar.Flush();
}


CString SistorCharacter:: GetDisplayStr()const{
	return m_name+_T("@")+m_ip;
}



void SistorCharacter::GetNameIP(CString & name,CString &ip)const {//获得属性量
	name=m_name;
	ip=m_ip;
}