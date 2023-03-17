#pragma once
#include "Character.h"
#include "Camera.h"
#include "TCollision.h"

class Player :
    public Character
{
public:
    virtual bool    Init();
    virtual bool    Frame();

    void    Attack();

public:
    T_BOX   m_AttackBox;
    bool    m_isAttack;

    Camera* m_pMainCamera;
};
