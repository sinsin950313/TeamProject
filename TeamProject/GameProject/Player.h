#pragma once
#include "Character.h"
#include "Camera.h"
#include "TCollision.h"

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

private:
    Player();
    ~Player();

public:
    Camera* m_pMainCamera;
};