#pragma once
#include "std.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class Writer
{
public:
	ID2D1Factory* m_d2dFactory = nullptr;
	IDWriteFactory* m_pDWriteFactory = nullptr;
	ID2D1RenderTarget* m_d2dRT = nullptr;
	IDWriteTextFormat* m_pTextFormat = nullptr;
	ID2D1SolidColorBrush* m_pTextColor = nullptr;
	std::wstring	m_szDefaultText;

	//// 글자 고정이고 폰트와 사이즈 조절 가능한 방식 잘 안쓰임
	//IDWriteTextLayout* m_pTextLayout;

public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();

	virtual HRESULT CreateDxResource();
	virtual HRESULT DeleteDxResource();

public:
	bool	Set(IDXGISurface1* dxgiSurface);
	bool	Draw(float x, float y, std::wstring text, D2D1_COLOR_F color = { 0,0,0,1 });
};

