#pragma once
#include "Frustum.h"
#include "ArcBall.h"

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08

class Camera
{
public:
	ArcBall			m_WorldArcBall;
	ArcBall			m_ViewArcBall;
	//--------------------------------------------------------------------------------------
	// 멤버 변수
	//--------------------------------------------------------------------------------------
	TVector3			m_vModelCenter;
	TMatrix			m_mModelLastRot;        // Last arcball rotation matrix for model 
	TMatrix			m_mModelRot;            // Rotation matrix of model
	TMatrix			m_mCameraRotLast;
public:
	Frustum m_vFrustum;

	//--------------------------------------------------------------------------------------
	// 카메라 방향벡터 및 위치 / 타켓 벡터	
	//--------------------------------------------------------------------------------------
	TVector3		m_vPos;
	TVector3		m_vLook; // Z
	TVector3		m_vUp = { 0, 1, 0 }; // Y
	TVector3		m_vRight; // X
	TVector3		m_vTarget = { 0, 0, 0 };
	TVector3			m_vDefaultEye;          // Default camera eye position
	TVector3			m_vDefaultLookAt;       // Default LookAt position

	//--------------------------------------------------------------------------------------
	// 뷰 회전에 대한 쿼터니언
	//--------------------------------------------------------------------------------------
	TQuaternion		m_qRotation;
	//--------------------------------------------------------------------------------------
	// 각 축에 대한 변위값 w=반지름( 원점으로부터의 거리 )
	//--------------------------------------------------------------------------------------
	TVector3			m_vPosDelta;
	//--------------------------------------------------------------------------------------
	// 뷰 행렬 및 투영 행렬
	//--------------------------------------------------------------------------------------
	TMatrix		m_matWorld;
	TMatrix		m_matView;
	TMatrix		m_matProj;

	TVector3		m_vRotation = { 0, 0, 0 };
	float			m_fDistance = 0.0f;
	float			m_fSpeed = 10.0f;

	float	m_fNear;
	float	m_fFar;
	float	m_fFovY;
	float	m_fAspectRatio;

	// 추가 : 오일러 각 성분 	
	float				m_fCameraYawAngle;
	float				m_fCameraPitchAngle;
	float				m_fCameraRollAngle;
	// 추가 : 반지름( 초기 원점으로부터의 거리) 에 대한 변수 	
	float				m_fRadius;
	float				m_fDefaultRadius;       // Distance from the camera to model 
	float				m_fMinRadius;           // Min radius
	float				m_fMaxRadius;           // Max radius
	//--------------------------------------------------------------------------------------
	// 마우스 및 버튼의 사용여부를 판단하는 마스크 값과 마우스 휠 변위값
	//--------------------------------------------------------------------------------------	
	int					m_nMouseWheelDelta;     // Amount of middle wheel scroll (+/-) 
	int					m_nRotateModelButtonMask;
	int					m_nZoomButtonMask;
	int					m_nRotateCameraButtonMask;

public:
	virtual void CreateViewMatrix(TVector3 vEye, TVector3 vAt, TVector3 vUp);
	virtual void CreateProjMatrix(float fNear, float fFar, float fFovY, float fAspectRatio);
	virtual bool Frame();
	virtual bool Release();
	virtual void UpdateVector();

	virtual void Update(TVector4 vDirValue);

	void	SetRadius(float fDefaultRadius = 5.0f, float fMinRadius = 1.0f, float fMaxRadius = FLT_MAX);

	void	SetObjectView(TVector3 vMax, TVector3 vMin);


	virtual void				MoveLook(float fValue);
	virtual void				MoveSide(float fValue);
	virtual void				MoveUp(float fValue);


	// 회전 및 이동에 적용할 버튼 세팅 
	virtual void				SetButtonMasks(int nRotateModelButtonMask, int nZoomButtonMask, int nRotateCameraButtonMask);
	//--------------------------------------------------------------------------------------
	// 윈도우 사이즈가 변경을 반영한다. 
	//--------------------------------------------------------------------------------------
	virtual void				SetWindow(int nWidth, int nHeight, float fArcballRadius = 0.9f);
	//--------------------------------------------------------------------------------------
	// 윈도우 메세지를 사용하여 카메라 제어 
	//--------------------------------------------------------------------------------------
	virtual int					WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	//--------------------------------------------------------------------------------------
	// 모델 회전 중심위치를 지정한다.
	//--------------------------------------------------------------------------------------
	void						SetModelCenter(TVector3 vModelCenter);

public:
	Camera();
	virtual ~Camera();
};

