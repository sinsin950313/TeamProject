#include "Writer.h"

HRESULT Writer::CreateDxResource()
{
	HRESULT hr = m_d2dRT->CreateSolidColorBrush({ 0, 0, 0, 1 }, &m_pTextColor);
	return S_OK;
}

HRESULT Writer::DeleteDxResource()
{
	if (m_pTextColor) 
	{
		m_pTextColor->Release();
		m_pTextColor = nullptr;
	}
	if (m_d2dRT)
	{
		m_d2dRT->Release();
		m_d2dRT = nullptr;
	}
	return S_OK;
}

bool Writer::Init()
{
	Release(); // 개체를 삭제하지 않고 재사용을 위한 Init 호출을 사용할 수 있기 때문에 앞에 Release를 호출
	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2dFactory);
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pDWriteFactory);
	hr = m_pDWriteFactory->CreateTextFormat(
		L"고딕",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		30,
		L"ko-kr",
		&m_pTextFormat);

	//m_szDefaultText = L"DefaultText";
	//hr = m_pDWriteFactory->CreateTextLayout(
	//	m_szDefaultText.c_str(),
	//	m_szDefaultText.size(),
	//	m_pTextFormat,
	//	g_rcClient.right,
	//	g_rcClient.bottom,
	//	&m_pTextLayout);

	return true;
}

bool Writer::Frame()
{
	return true;
}

bool Writer::Render()
{
	return true;
}

bool Writer::Release()
{
	//if (m_pTextLayout)m_pTextLayout->Release();
	if (m_pTextColor)m_pTextColor->Release();
	if (m_pTextFormat) m_pTextFormat->Release();
	if (m_d2dRT) m_d2dRT->Release();
	if (m_pDWriteFactory) m_pDWriteFactory->Release();
	if (m_d2dFactory) m_d2dFactory->Release();
	return true;
}

bool	Writer::Set(IDXGISurface1* dxgiSurface)
{
	D2D1_RENDER_TARGET_PROPERTIES props;
	ZeroMemory(&props, sizeof(props));

	props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	props.pixelFormat = { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED };
	props.dpiX = 96;
	props.dpiY = 96;
	props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

	HRESULT hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(
		dxgiSurface,
		&props,
		&m_d2dRT);

	CreateDxResource();
	return true;
}

bool	Writer::Draw(float x, float y, std::wstring text, D2D1_COLOR_F color)
{
	m_d2dRT->BeginDraw();

	D2D1_RECT_F rt = { x, y, g_rcClient.right, g_rcClient.bottom };
	m_pTextColor->SetColor(color);
	m_pTextColor->SetOpacity(1.0f);
	m_d2dRT->DrawText(text.c_str(), text.size(), m_pTextFormat, rt, m_pTextColor);

	m_d2dRT->EndDraw();

	return true;
}