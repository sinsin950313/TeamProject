#pragma once
#include "Sound.h"
class SoundMgr : public CSingleton<SoundMgr>
{
private:
	FMOD::System* m_pSystem = nullptr;
	std::list<std::wstring> m_fileList;
private:
	std::map<std::wstring, Sound*> m_List;

public:
	std::wstring GetSplitName(std::wstring fullpath);
	void	LoadDir(std::wstring path);
	void	LoadAll(std::wstring path);
	bool	Init();
	bool	Frame();
	bool	Release();
	HRESULT Load(std::wstring fullpath, Sound** retSound);
	Sound* Find(std::wstring name);

private:
	friend class CSingleton<SoundMgr>;
	SoundMgr();
public:
	~SoundMgr();
};

#define I_Sound SoundMgr::GetInstance()