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
*/
#include "ClipPaste.h"
#include "Windows.h"
#include <minwindef.h>
#include <synchapi.h>
#include <winuser.h>
#include <stdio.h>


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
            fake_keypress(text, 1024, 60);
        }
    }

    // EOP
    return 0;
}
