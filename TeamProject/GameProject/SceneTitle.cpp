#include "SceneTitle.h"
#include "Input.h"

void SceneTitle::DataLoad()
{

}

E_SCENE     SceneTitle::NextScene()
{
	if (I_Input.GetKey(VK_SPACE) == KEY_UP)
	{
		return E_SCENE::S_INGAME;
	}
	return S_TITLE;
}

bool    SceneTitle::Init()
{
	I_Input.SwitchShowMouse(true);

	m_pObj = new TestObj;
	m_pInteract = new InteractiveObject();

	m_pInteract->SetSwitch(&m_pObj->m_isB);
	return true;
}

bool    SceneTitle::Frame()
{
	if (I_Input.GetKey(VK_F3) == KEY_PUSH)
		I_Input.SwitchShowMouse(!I_Input.GetShowMouse());
	if (I_Input.GetKey('Q') == KEY_PUSH)
	{
		m_pInteract->Switching();
	}
	return true;
}

bool    SceneTitle::Render()
{
	return true;
}

bool    SceneTitle::Release()
{
	delete m_pObj;
	delete m_pInteract;
	return true;
}