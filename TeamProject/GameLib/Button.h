#pragma once
#include "Interface.h"
class Button :
    public Interface
{
public:
    Button()
    {
        m_CurrentState = UI_NORMAL;
    }
};

