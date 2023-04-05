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
    virtual bool	Render();

private:
    Player();
    ~Player();

public:
    Camera* m_pMainCamera;

    TrailEffect* m_pTrail;
};