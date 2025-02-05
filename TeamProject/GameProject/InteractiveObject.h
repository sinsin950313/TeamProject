#pragma once
#include "TestObj.h"
class InteractiveObject
{
	// 충돌체
	// 충돌 + 키 입력시 발생 이벤트 - string, enum, 
	// 상호작용시 변화가 있을 오브젝트의 리스트가 있어야할듯
	// 
	// 상호작용 오브젝트들이 프레임이 돌아야하나? - (오브젝트의 움직임이 보간되면서 움직인다면 필요할듯)
	// 프레임이 돌아야한다면 리스트를 어디선가 들고 있어야함
	// 반복 상호작용이 가능한가 / 단발성으로 사라지는가
	// 
	// 오브젝트를 연결할 방법이나 함수
	// 움직인다면 몇초동안을 움직일지
	// 
	// 방법 1)
public:
	void	SetSwitch(bool* b);
	void	Switching();

public:
	bool* m_bSwitch;

	// 방법 2)
public:
	void	SetObject(TestObj* obj);
	void	Interacted();

public:
	TestObj* m_pObj;
};

