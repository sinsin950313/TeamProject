#include "SoundMgr.h"
#include <tchar.h>
#include <io.h>

std::wstring SoundMgr::GetSplitName(std::wstring fullpath)
{
	std::wstring name;
	TCHAR drive[MAX_PATH] = { 0, };
	TCHAR dir[MAX_PATH] = { 0, };
	TCHAR filename[MAX_PATH] = { 0, };
	TCHAR ext[MAX_PATH] = { 0, };
	_tsplitpath_s(fullpath.c_str(), drive, dir, filename, ext);
	name = filename;
	name += ext;
	return name;
}
void	SoundMgr::LoadDir(std::wstring path)
{
	std::wstring last = path.substr(path.length() - 1, path.length());
	if (last.compare(L"/"))
	{
		path += L"/";
	}
	std::wstring dirpath = path + L"*.*";
	intptr_t handle;
	struct _wfinddata_t fd;
	handle = _wfindfirst(dirpath.c_str(), &fd);
	if (handle == -1) return;

	do
	{
		if ((fd.attrib & _A_SUBDIR) && fd.name[0] != '.')
		{
			LoadDir(path + fd.name + L"/");
		}
		else if (fd.name[0] != '.')
		{
			m_fileList.push_back(path + fd.name);
		}
	} while (_wfindnext(handle, &fd) == 0);
	_findclose(handle);
}
void	SoundMgr::LoadAll(std::wstring path)
{
	LoadDir(path);
	for (auto& data : m_fileList)
	{
		Sound* sound;
		Load(data, &sound);
	}
}
bool	SoundMgr::Init()
{
	FMOD::System_Create(&m_pSystem);
	m_pSystem->init(32, FMOD_INIT_NORMAL, 0);
	return true;
}
bool	SoundMgr::Frame()
{
	m_pSystem->update();
	for (std::pair<std::wstring, Sound*> data : m_List)
	{
		Sound* pData = data.second;
		if (pData)pData->Frame();
	}

	return true;
}
bool	SoundMgr::Release()
{
	for (std::pair<std::wstring, Sound*> data : m_List)
	{
		Sound* pData = data.second;
		if (pData) pData->Release();
		delete pData;
	}
	m_List.clear();

	m_pSystem->close();
	m_pSystem->release();
	return true;
}
HRESULT SoundMgr::Load(std::wstring fullpath, Sound** retSound)
{
	if (retSound)
	{
		*retSound = nullptr;
	}
	else if (!retSound)
	{
		return E_INVALIDARG;
	}

	std::wstring name = GetSplitName(fullpath);
	std::map<std::wstring, Sound*>::iterator iter = m_List.find(name);
	if (iter != m_List.end())
	{
		*retSound = iter->second;
		return S_OK;
	}

	Sound* pSound = new Sound;
	pSound->m_szName = fullpath;
	if (pSound)
	{
		if (SUCCEEDED(pSound->Load(m_pSystem, fullpath)))
		{
			m_List.insert(std::make_pair(name, pSound));
			return S_OK;
		}
	}
	return E_FAIL;
}
Sound* SoundMgr::Find(std::wstring name)
{
	auto iter = m_List.find(name);
	if (iter != m_List.end())
	{
		Sound* pSound = iter->second;
		return pSound;
	}
	return nullptr;
}

SoundMgr::SoundMgr()
{

}

SoundMgr::~SoundMgr()
{
	Release();
}