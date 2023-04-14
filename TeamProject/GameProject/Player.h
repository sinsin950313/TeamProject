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
    virtual bool	PostRender();
    virtual bool    Release();

private:
    Player();
    ~Player();

    void    UpdateInstancingBuffer();

public:
    Camera* m_pMainCamera;

    Shader* m_pModelVS[2];
    Shader* m_pModelPS[2];
    std::vector<TMatrix>     m_matInstancing;
    TVector4    m_vInstancingColor[12];

    ID3D11Buffer* m_pInstancingBuffer;
    VS_INSTANCING_BUFFER m_cbInstancingData;

    TrailEffect* m_pTrail;

public:
    bool m_IsImmortal = false;

public:
	void Damage(int damage) override;

public:
	TVector3 m_DashDirection;
    bool m_IsDash = false;

private:
    const int m_kMaxUltimateSkillStackCount = 3;
    int m_UltimateSkillStack = 0;
    float m_stackingTimeStamp = 0.0f;

public:
    void UltimateSkillStacking(float timeStamp);
    int GetUltimateSkillStack();
    bool IsAbleToCallUltimateSkill();
    void CallUltimateSkill();
};