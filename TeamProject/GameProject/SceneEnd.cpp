#include "SceneEnd.h"
#include "Input.h"

void SceneEnd::DataLoad()
{
	m_fLoadRate = 0.0f;

	m_fLoadRate = 1.0f;
}

bool SceneEnd::Init()
{
	return true;
}

bool SceneEnd::Frame()
{
	return true;
}

bool SceneEnd::Render()
{
	return true;
}

bool SceneEnd::Release()
{
	return true;
}

E_SCENE SceneEnd::NextScene()
{
	for (int i = 0; i < 256; i++)
	{
		if (I_Input.GetKey(i) == KEY_PUSH)
		{
			return E_SCENE::S_TITLE;
		}
	}
	return E_SCENE::S_END;
}