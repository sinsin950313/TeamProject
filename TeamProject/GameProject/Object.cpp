#include "Object.h"
#include "FileIOObject.h"
#include "ModelMgr.h"
#include "CollisionMgr.h"
#define _DegreeToRadian(X) X*(XM_PI/180.0f)
bool Object::Frame()
{
	Character::Frame();
	return true;
}

bool Object::PreRender()
{
	return false;
}

bool Object::Render()
{
	Character::Render();
	return true;
}

std::wstring Object::mtw(std::string str)
{
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	//return conv.from_bytes(str);
	std::wstring ret;
	ret.assign(str.begin(), str.end());
	return ret;
}

std::string Object::wtm(std::wstring str)
{
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	//return conv.to_bytes(str);
	std::string ret;
	ret.assign(str.begin(), str.end());
	return ret;
}

std::vector<std::wstring> Object::SplitPath(std::wstring fullPath)
{
	std::vector<std::wstring> ret;

	const UINT maxLength = 1024;
	TCHAR drive[maxLength]{ 0, };
	TCHAR dir[maxLength]{ 0, };
	TCHAR fileName[maxLength]{ 0, };
	TCHAR ext[maxLength]{ 0, };

	_tsplitpath_s(fullPath.c_str(), drive, dir, fileName, ext);

	ret.push_back(std::wstring(drive));
	ret.push_back(std::wstring(dir));
	ret.push_back(std::wstring(fileName));
	ret.push_back(std::wstring(ext));

	return ret;
}


Object::Object()
{
}

Object::Object(std::string szFullPath, Transform transform, XMFLOAT3 vAxis0, XMFLOAT3 vAxis1, XMFLOAT3 vAxis2, float fExtentX, float fExtentY, float fExtentZ, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) : Character()
{
	Character::SetDevice(pDevice, pDeviceContext);
	auto splitedPath = SplitPath(mtw(szFullPath));

	XMFLOAT3 scale;
	XMStoreFloat3(&scale, transform.scale);
	XMFLOAT3 rot;
	XMStoreFloat3(&rot, transform.rotation);
	XMFLOAT3 translation;
	XMStoreFloat3(&translation, transform.position);

	m_vScale = TVector3(scale);
	m_vRotation = TVector3(_DegreeToRadian(rot.x), _DegreeToRadian(rot.y), _DegreeToRadian(rot.z));
	m_vPos = TVector3(translation);

	SSB::ObjectScriptIO io;
	auto scriptInfo = io.Read(wtm(splitedPath[2]));

	//I_Model.Load(str, "Idle", &m_pModel);
	// static object
	m_pModel = new SSB::Model;
	m_pModel->SetDevice(pDevice, pDeviceContext);
	int offset = 0;
	m_pModel->Deserialize(scriptInfo.Pointer, scriptInfo.BufferSize, offset);
	m_pModel->Init();
	
	/*m_ColliderBox.CreateOBBBox(fExtentX, fExtentY, fExtentZ, translation, vAxis0, vAxis1, vAxis2);

	I_Collision.AddStaticObjectBox(&m_ColliderBox, this);*/
}

Object::~Object()
{
}
