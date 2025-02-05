#pragma once
#include "Character.h"
#include "Camera.h"
#include "TCollision.h"
#include "TrailEffect.h"
#include "DebugBox.h"
#include "EffectMgr.h"

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
    bool Render() override;
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

    Effect* m_pAura = nullptr;

    ID3D11Buffer* m_pInstancingBuffer;
    VS_INSTANCING_BUFFER m_cbInstancingData;

    TrailEffect* m_pTrail;
    Interface* m_pInterSkillQ;
    Interface* m_pInterSkillDash;
    Interface* m_pInterSkillE;
    Interface* m_pInterSkillR;
    Interface* m_pInterSkillPassive;
    Interface* m_pInterDamageBlood;
    Interface* m_pInterBlur;

public:
    bool m_IsImmortal = false;
    bool m_isCheat = false;

public:
	void Damage(int damage) override;

public:
	TVector3 m_DashDirection;
    bool m_IsDash = false;

public:
    bool IsDash();

private:
    const int m_kMaxUltimateSkillStackCount = 3;
    bool m_IsCallUltimateSkill = false;
    int m_UltimateSkillStack = 0;
    float m_stackingTimeStamp = 0.0f;

public:
    void UltimateSkillStacking(float timeStamp);
    int GetUltimateSkillStack();
    bool IsAbleToCallUltimateSkill();
    bool IsUltimateSkill();
    void CallUltimateSkill();
    void ResetUltimateSkill();

private:
    class Tornado : public Character
    {
    private:
        Character* m_Owner;
        std::set<Character*> m_DamagedCharacters;
        T_BOX m_collideBox;
        TVector3 m_Position;
        TVector3 m_Direction;
        float m_TimeStamp;
        float m_Livetime = 1.5f;
        bool m_IsAlive = false;
        float m_Speed = 20;
        float m_Damage = 20.0f;

    public:
        Tornado(Character* owner);

    private:
        std::vector<Character*> GetHitList();

    public:
        void Initialize_SetPosition(TVector3 position);
        void Initialize_SetDirection(TVector3 direction);
        void Initialize_SetTimestamp(float timestamp);
        bool IsHit();
        bool IsFinished();
        std::vector<Character*> GetAirborneList();
        void Clear();

    public:
        bool Init() override;
        bool Frame() override;
        bool Render() override;
        bool Release() override;

    private:
		DebugBox* m_pDebugBox = nullptr;
    };
    Tornado m_Tornado;

public:
	void	SetMap(MeshMap* pMap) override;
    void ShotTornado(float timestamp);
    bool IsUlitmateSkillCallable();
    std::vector<Character*> GetUltimateSkillTargetList();

private:
    bool _isBegin = false;

public:
    void SetBegin();
    bool IsBegin();

private:
    bool _isVictory = false;

public:
    void SetVictory();
    bool IsVictory();

public:
    void SceneChange();
};