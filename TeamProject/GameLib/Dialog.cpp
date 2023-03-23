#include "Dialog.h"

bool    Dialog::Init()
{
	return true;
}

bool    Dialog::Frame()
{
	Interface::Frame();
	return true;
}

bool    Dialog::Render()
{
	for (int iSub = 0; iSub < m_rcDrawList.size(); iSub++)
	{
		m_rcDrawList[iSub]->Render();
	}
	for (int iChild = 0; iChild < m_pChildList.size(); iChild++)
	{
		m_pChildList[iChild]->Render();
	}
	return true;
}

bool    Dialog::Release()
{
	Interface::Release();
	return true;
}

