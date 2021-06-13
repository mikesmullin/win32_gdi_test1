#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

const wchar_t* WINDOW_TITLE = L"TraderHUD";
const wchar_t* CLASS_NAME   = L"traderhud";
HWND currentWindow;

void log(wchar_t* msg)
{
    int result = MessageBox(
        currentWindow,
        msg,
        WINDOW_TITLE,
        MB_ICONINFORMATION |
        MB_OK |
        MB_SETFOREGROUND |
        MB_TOPMOST
    );

}

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    int nCmdShow
) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
        0,                      // Optional window styles
        CLASS_NAME,             // Window class
        WINDOW_TITLE,           // Window text
        WS_OVERLAPPEDWINDOW,    // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,   // Parent window
        NULL,   // Menu
        hInstance, // Instance handle
        NULL
    );

    if (NULL == hwnd) {
        return 1;
    }

    currentWindow = hwnd;

    HDC fakeDC = GetDC(hwnd);

    ShowWindow(hwnd, nCmdShow);

    Sleep(2000);

    log(L"Hello world!");

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // All painting occurs here, between BeginPaint and EndPaint.
            // FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(hwnd, &ps);

            // BITMAP bmInfo;
            // PAINTSTRUCT ps;

            // HDC hdc = BeginPaint(hwnd, &ps);

            // HDC hdcMem = CreateCompatibleDC(hdc);
            // HBITMAP hbmOld = SelectObject(hdcMem, g_hbmBall);

            // GetObject(g_hbmBall, sizeof(bmInfo), &bmInfo);

            // BitBlt(hdc, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, hdcMem, 0, 0, SRCCOPY);

            // SelectObject(hdcMem, hbmOld);
            // DeleteDC(hdcMem);

            // EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}