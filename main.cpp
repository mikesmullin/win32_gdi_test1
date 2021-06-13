#include <Windows.h>
#include <strsafe.h>

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}

const wchar_t* WINDOW_TITLE = L"TraderHUD";
const wchar_t* CLASS_NAME   = L"traderhud";
const int w = 960, h = 500;
HWND currentWindow;
HBITMAP bmp;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void log(const wchar_t* msg)
{
    int result = MessageBox(
        currentWindow,
        msg,
        WINDOW_TITLE,
        MB_ICONINFORMATION |
        MB_OK |
        MB_SETFOREGROUND
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
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,

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

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessage(&msg);
    }

    return 0;
}

WCHAR mouseCoordinates[4] = {0};

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_CREATE:
        {
            const wchar_t* path = L"..\\resources\\matrix.bmp";
            bmp = (HBITMAP) LoadImage(NULL, path, IMAGE_BITMAP, w, h, LR_LOADFROMFILE);
            if (NULL == bmp) {
                ErrorExit(L"WM_CREATE LoadImage");
                // log(L"WM_CREATE LoadImage failed.");
            }

            const BOOL outcome = sndPlaySound(L"..\\resources\\music.wav", SND_ASYNC | SND_LOOP);
            if (FALSE == outcome) {
                // ErrorExit(L"WM_CREATE sndPlaySound");
                log(L"WM_CREATE sndPlaySound failed.");
            }

            return 0;
        }

    case WM_PAINT:
        {
            // TODO: log this event to disk to verify its not called too frequently. perhaps display an FPS meter.
            BITMAP bmInfo;
            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hwnd, &ps);
            if (NULL == hdc)
            {
                log(L"WM_PAINT BeginPaint() failed.");
            }
            else {
                HDC hdcMem = CreateCompatibleDC(hdc);
                if (NULL == hdcMem) 
                {
                    log(L"WM_PAINT CreateCompatibleDC failed.");
                }
                else {
                    // HBITMAP bmpCorrectlySized = CreateCompatibleBitmap(hdc, w, h);
                    // HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, bmpCorrectlySized);
                    HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, bmp);
                    if (NULL == hbmOld)
                    {
                        log(L"WM_PAINT SelectObject failed.");
                    }
                    else {
                        // int outcome1 = GetObject(bmpCorrectlySized, sizeof(bmInfo), &bmInfo);
                        // if (0 == outcome1) {
                        //     log(L"WM_PAINT GetObject failed.");
                        // }
                        // BitBlt(hdc, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, hdcMem, 0, 0, SRCCOPY);

                        int outcome = BitBlt(hdc, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
                        if (0 == outcome) {
                            log(L"WM_PAINT BitBlt failed.");
                            // see: https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
                            // DWORD dw = GetLastError();
                            return 0;
                        }

                        SelectObject(hdcMem, hbmOld);
                    }

                    DeleteDC(hdcMem);
                }

                EndPaint(hwnd, &ps);
            }
        }
        return 0;

    // case WM_SIZE:
    //     log(L"WM_SIZE event.");
    //     return 0;

    // case WM_KEYDOWN:
        // NOTICE: must call DefWindowProc() on these or system keys will be ignored (user won't be able to alt+tab etc)
        // see: https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input
        // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate
    // case WM_KEYUP

    case WM_MOUSEMOVE:
        // TODO: switch from cl to gcc + gdb to receive cli std output this way, or just write to .log file on disk
        wsprintf(mouseCoordinates, L"X:%d Y:%d\n", HIWORD(lParam), LOWORD(lParam));
        OutputDebugString(mouseCoordinates);
        return 0;

    case WM_LBUTTONUP:
        log(L"mouse left button up");
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}