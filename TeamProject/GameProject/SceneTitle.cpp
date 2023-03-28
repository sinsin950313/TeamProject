#include "SceneTitle.h"
#include "Input.h"

void SceneTitle::DataLoad()
{
	m_fLoadRate = 0.0f;

	m_fLoadRate = 1.0f;
}

bool    SceneTitle::Init()
{
	return true;
}

E_SCENE     SceneTitle::NextScene()
{
	for (int i = 0; i < 256; i++)
	{
		if (I_Input.GetKey(i) == KEY_UP)
		{
			return E_SCENE::S_INGAME;
		}
	}
	return S_TITLE;
}

bool    SceneTitle::Frame()
{
	return true;
}

bool    SceneTitle::Render()
{
	return true;
}

bool    SceneTitle::Release()
{
	return true;
}