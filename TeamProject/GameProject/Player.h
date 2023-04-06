#pragma once
#include "Character.h"
#include "Camera.h"
#include "TCollision.h"
#include "TrailEffect.h"

struct VS_INSTANCING_BUFFER
{
    TMatrix matWorld[12];
    TVector4 vColor[12];
};

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
    virtual bool    Release();

private:
    Player();
    ~Player();

    void    UpdateInstancingBuffer();

public:
    Camera* m_pMainCamera;

    std::vector<TMatrix>     m_matInstancing;
    TVector4    m_vInstancingColor[12];

    ID3D11Buffer* m_pInstancingBuffer;
    VS_INSTANCING_BUFFER m_cbInstancingData;

    TrailEffect* m_pTrail;
};