/*
Title: ClipPaste
Author: Eric Caverly
Date: 06/11/2024

Purpose: Type out text on the clipboard with fake keyboard presses. Used to fill in passwords when Paste is not available.

Sources:
- https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
- https://stackoverflow.com/questions/45531124/how-to-simulate-a-keypress
- https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
- https://stackoverflow.com/questions/14762456/getclipboarddatacf-text
- https://samulinatri.com/blog/win32-window-creation-tutorial
*/

#include "ClipPaste.h"
#include <Windows.h>
#include <minwindef.h>
#include <synchapi.h>
#include <winnt.h>
#include <winuser.h>
#include <stdio.h>

#define Assert(c) do { if (!(c)) __debugbreak(); } while (0)
#pragma comment (lib, "user32.lib")

#define	WM_USER_SHELLICON WM_USER + 1
#define WM_TASKBAR_CREATE RegisterWindowMessage(_T("TaskbarCreated"))
/*
int main() {
    // Try to register the hotkey
    if(!RegisterHotKey(NULL, 1, MOD_ALT, 'V')) {
        printf("Failed to register hotkey\n");
        return 1;
    }
    printf("Hotkey registered\n");

    MSG msg = {0};
    // Check for messages (blocking)
    while(GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY) {
            printf("Paste Keypress Detected\n");
            // Clipboard Buffer
            char text[1024];
            if(!get_clipboard_contents_ascii(text, 1024)) {
                printf(" /\\ Unable to Get Clipboard Data\n");
            }

            // Clear modifiers
            clear_modifiers();

            Sleep(20);

            // Backspace alt key
            backspace_garbage(3);

    
            // let modifiers be raised
            Sleep(20);

            // Fake keypress characters
            word_keypress(text, 1024, 60);
        }
    }

    // EOP
    return 0;
}
*/


LRESULT CALLBACK win_proc(HWND window, UINT msg, WPARAM w_param, LPARAM l_param) {
    switch(msg) {
        case WM_INITDIALOG:
            return TRUE;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default: 
            return DefWindowProcW(window, msg, w_param, l_param);
            break;
    }
}


// Main windows function (root)
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd, int cmd_show) {
    WNDCLASS wc = {
        .lpszClassName = L"ClipPaste",
        .lpfnWndProc = win_proc,
        .hInstance = instance,
        .hCursor = LoadCursor(NULL, IDC_CROSS),
    };

    ATOM atom = RegisterClassW(&wc);
    Assert(atom && "Failed to register window");

    HWND window = CreateWindowW(
        wc.lpszClassName,
        L"Clip Paste",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, // Position (X,Y)
        CW_USEDEFAULT, CW_USEDEFAULT, // Size
        NULL, // Parent
        NULL, // Menu handle
        instance, // Application instance
        NULL // Additonal data
    );

    Assert(window && "Failed to create window");
    
    ShowWindow(window, SW_HIDE);

    for(;;) {
        MSG msg;
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
    }



    return 0;
}