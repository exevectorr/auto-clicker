#include <windows.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>

#define ID_START_BUTTON 1
#define ID_STOP_BUTTON 2
#define ID_EXIT_BUTTON 3

const char CLASS_NAME[] = "AutoClickerWindowClass";
HWND hWndGlobal;

std::atomic<bool> isClicking(false);

void ClickThread() {
    while (true) {
        if (isClicking) {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_START_BUTTON:
                    isClicking = true;
                    break;
                case ID_STOP_BUTTON:
                    isClicking = false;
                    break;
                case ID_EXIT_BUTTON:
                    PostQuitMessage(0);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "AutoClicker",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;
    ShowWindow(hwnd, nCmdShow);

    CreateWindow("BUTTON", "Start AutoClicker\n(Ctrl+Alt+Space+C)",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        35, 20, 350, 30,
        hwnd, (HMENU)ID_START_BUTTON, hInstance, NULL);

    CreateWindow("BUTTON", "Stop AutoClicker\n(Ctrl+Alt+Space+S)",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        35, 60, 350, 30,
        hwnd, (HMENU)ID_STOP_BUTTON, hInstance, NULL);

    CreateWindow("BUTTON", "Exit",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        35, 100, 350, 30,
        hwnd, (HMENU)ID_EXIT_BUTTON, hInstance, NULL);

    std::thread clickerThread(ClickThread);
    clickerThread.detach();

    // Hotkeys
    RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_ALT, VK_SPACE); // You can modify to check for C or S
    RegisterHotKey(hwnd, 2, MOD_CONTROL | MOD_ALT, 0x43); // 'C'
    RegisterHotKey(hwnd, 3, MOD_CONTROL | MOD_ALT, 0x53); // 'S'

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            if (msg.wParam == 2) isClicking = true;
            if (msg.wParam == 3) isClicking = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
