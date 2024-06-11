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

#include "Windows.h"
#include <minwindef.h>
#include <synchapi.h>
#include <winuser.h>
#include <stdio.h>


int get_clipboard_contents_ascii(char output[], int size) {
    HGLOBAL handler;
    char * text_pointer;

    // Check if text is in the clipboard
    if(!IsClipboardFormatAvailable(CF_TEXT)) {
        printf("Format not available\n");
        return 0;
    }

    // Open the clipboard, no window to set as the owner so NULL
    if(!OpenClipboard(NULL)) {
        printf("Unable to open\n");
        return 0;
    }

    // Get a handler for the clipboard
    handler = GetClipboardData(CF_TEXT);
    if(handler == NULL) {
        printf("Unable to get handle\n");
        return 0;
    }

    // Get a char pointer to the clipboard contents. Also lock so no other processes can mobify the clipboard
    text_pointer = (GlobalLock(handler));
    if(text_pointer == NULL) {
        printf("Unable to get contents\n");
        return 0;
    }

    // Copy the data starting at the pointer to the output buffer up to size specified
    strcpy_s(output, size, text_pointer);

    // Unlock and close the clipboard
    GlobalUnlock(handler);
    CloseClipboard();

    // Return true
    return 1;
}


void uppercase_press(char key) {

    // input[0] -> shift down
    // input[1] -> key down
    // input[2] -> key up
    // input[3] -> shift up

    INPUT u_input[4] = {0};
    u_input[0].type = u_input[1].type = u_input[2].type = u_input[3].type = INPUT_KEYBOARD;
    u_input[2].ki.dwFlags = u_input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    u_input[0].ki.wVk = u_input[3].ki.wVk = VK_SHIFT;

    u_input[1].ki.wVk = u_input[2].ki.wVk = key;
            
    SendInput(4, u_input, sizeof(INPUT));
}


void lowercase_press(char key) {

    // input[0] -> key down
    // input[1] -> key up

    INPUT l_input[2] = {0};
    l_input[0].type = l_input[1].type = INPUT_KEYBOARD; 
    l_input[1].ki.dwFlags = KEYEVENTF_KEYUP;            

    l_input[0].ki.wVk = l_input[1].ki.wVk = key;

    SendInput(2, l_input, sizeof(INPUT));
}


void fake_keypress(char text[], int size, int delay_ms) {
    // Itterate through each character in text
    for(int i=0; i<size; i++) {
        // End once we reached string terminator
        if(text[i] == '\0') {
            break;
        }
        
        // find scan code for character
        short key_code = VkKeyScan(text[i]);

        // Get Uppercase/Lowercase and virtual key code
        UINT case_status = HIBYTE(key_code);
        UINT virtual_key_code = LOBYTE(key_code);

        //printf("%d - %x\n", case_status, virtual_key_code);

        // Type key in correct case
        if (case_status) {
            uppercase_press(virtual_key_code);
        } else {
            lowercase_press(virtual_key_code);
        }

        // Delay to seem human
        Sleep(delay_ms);
    }
}


void clear_modifiers() {
    // Number of keys to reset
    const int len = 10;
    INPUT input[len];

    // Set type and key up flag
    for(int i=0; i<len; i++) {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    // Keys to reset
    input[0].ki.wVk = VK_BACK;
    input[1].ki.wVk = VK_TAB;
    input[2].ki.wVk = VK_RETURN;
    input[3].ki.wVk = VK_SHIFT;
    input[4].ki.wVk = VK_MENU;
    input[5].ki.wVk = VK_CONTROL;
    input[6].ki.wVk = VK_CAPITAL;
    input[7].ki.wVk = 'V';
    input[8].ki.wVk = VK_LWIN;
    input[9].ki.wVk = VK_RWIN;
    
    // Send key-up messages
    SendInput(len, input, sizeof(INPUT));
}


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
            // Clipboard Buffer
            char text[1024];
            if(!get_clipboard_contents_ascii(text, 1024)) {
                printf(" /\\ Unable to Get Clipboard Data\n");
            }

            // Clear modifiers
            clear_modifiers();

            // Fake keypress characters
            fake_keypress(text, 1024, 100);
        }
    }

    // EOP
    return 0;
}
