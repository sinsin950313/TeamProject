#pragma once
#include "Character.h"
#include "Camera.h"
#include "TCollision.h"
#include "TrailEffect.h"

class Player :
    public Character
{
public:
    static Player& GetInstance()
    {
        static Player instance;
        return instance;
    }

public:
    virtual bool    Init();
    virtual bool    Frame();

    void    Attack();

private:
    Player();
public:
    ~Player();

public:
    T_BOX   m_AttackBox;
    bool    m_isAttack;

    Camera* m_pMainCamera;

    TrailEffect* m_pTrail;
};