#include "InteractiveObject.h"

void	InteractiveObject::SetSwitch(bool* b)
{
	m_bSwitch = b;
}

void	InteractiveObject::Switching()
{
	(*m_bSwitch) = !(*m_bSwitch);
}

void	InteractiveObject::SetObject(TestObj* obj)
{
	m_pObj = obj;
}
void	InteractiveObject::Interacted()
{
	//m_pObj->Inter();
}