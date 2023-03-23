#include "MyWindows.h"
MyWindows* g_pWindows = nullptr;

HWND		g_hWnd;
RECT		    g_rcClient;
RECT         g_rcWindow;
UINT        g_iClientWidth;
UINT        g_iClientHeight;

LRESULT CALLBACK    StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    _ASSERT(g_pWindows);
    return g_pWindows->MsgProc(hWnd, message, wParam, lParam);
}

int		MyWindows::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return -1;
}
LRESULT	MyWindows::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int iReturn = WndProc(hWnd, message, wParam, lParam);
    if (iReturn >= 0)
    {
        return iReturn;
    }
    switch (message)
    {
        //case WM_COMMAND:
        //{
        //    int wmId = LOWORD(wParam);
        //    // 메뉴 선택을 구문 분석합니다:
        //    switch (wmId)
        //    {
        //    case IDM_ABOUT:
        //        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
        //        break;
        //    case IDM_EXIT:
        //        DestroyWindow(hWnd);
        //        break;
        //    default:
        //        return DefWindowProc(hWnd, message, wParam, lParam);
        //    }
        //}
        //break;
        //case WM_PAINT:
        //{
        //    PAINTSTRUCT ps;
        //    HDC hdc = BeginPaint(hWnd, &ps);
        //    // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        //    EndPaint(hWnd, &ps);
        //}
        //break;
    case WM_SIZE:
        if (SIZE_MINIMIZED != wParam) // 최소화
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            GetWindowRect(hWnd, &g_rcWindow);
            GetClientRect(hWnd, &g_rcClient);
            g_iClientWidth = g_rcClient.right - g_rcClient.left;
            g_iClientHeight = g_rcClient.bottom - g_rcClient.top;
            if (FAILED(ResizeDevice(width, height)))
            {

            }

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT		MyWindows::ResizeDevice(UINT width, UINT height)
{
    return S_OK;
}

MyWindows::MyWindows()
{
    g_pWindows = this;
}

void		MyWindows::SethWnd(HWND hWnd)
{
    g_hWnd = hWnd;
    GetWindowRect(hWnd, &g_rcWindow);
    GetClientRect(hWnd, &g_rcClient);
}

bool		MyWindows::SetWindows(HINSTANCE hInstance, const WCHAR* szTitle, UINT iWidth, UINT iHeight)
{
    m_hInstance = hInstance;
    WORD ret = MyRegisterClass();
    //if(InitInstance(szTitle, iWidth, iHeight))
    if (InitInstance(szTitle, iWidth, iHeight))
    {
        return false;
    }
    return true;
}

void   MyWindows::CenterWindow()
{
    GetWindowRect(g_hWnd, &g_rcWindow);
    UINT iScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
    UINT iScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
    UINT cx, cy;
    cx = (iScreenWidth - (g_rcWindow.right - g_rcWindow.left)) * 0.5;
    cy = (iScreenHeight - (g_rcWindow.bottom - g_rcWindow.top)) * 0.5;
    MoveWindow(g_hWnd, cx, cy,
        g_rcWindow.right - g_rcWindow.left,
        g_rcWindow.bottom - g_rcWindow.top,
        true);
}

ATOM MyWindows::MyRegisterClass()
{
    WNDCLASSEXW wcex;
    ZeroMemory(&wcex, sizeof(WNDCLASSEXW));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = StaticWndProc;
    //wcex.cbClsExtra = 0;
    //wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 그냥 사전에 정의된 색깔들인것 같음 검색해보니까 *winuser.h*
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszClassName = L"CustomWindow";
    //LPCWSTR;
    //wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CREATEWINDOW));
    //wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CREATEWINDOW);
    //wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL MyWindows::InitInstance(const WCHAR* szTitle, UINT width, UINT height)
{
    m_csStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME;
    g_rcClient.right = width;
    g_rcClient.bottom = height;
    AdjustWindowRect(&g_rcClient, m_csStyle, FALSE);

    g_hWnd = CreateWindowW(L"CustomWindow", szTitle,
        m_csStyle,
        0, 0,
        g_rcClient.right - g_rcClient.left, g_rcClient.bottom - g_rcClient.top,
        nullptr, nullptr, 
        m_hInstance, nullptr);

    if (!g_hWnd)
    {
        return FALSE;
    }

    ShowWindow(g_hWnd, SW_SHOW);
    ShowCursor(FALSE);
    SetFocus(g_hWnd);
    //UpdateWindow(g_hWnd);
    CenterWindow();
    return TRUE;
}

bool		MyWindows::Run()
{
    MSG msg = { 0, };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
#ifdef DXCORE
        else
        {
            return true;
        }
#endif
    }

    return false;
}