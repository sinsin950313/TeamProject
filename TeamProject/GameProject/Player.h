#pragma once
#include "Character.h"
#include "Camera.h"
class Player :
    public Character
{
public:
    virtual bool    Frame();

    void MoveChar(XMVECTOR& destinationDirection, XMMATRIX& worldMatrix);

public:
    Camera* m_pMainCamera;
};

