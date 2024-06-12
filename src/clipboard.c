#include "ClipPaste.h"
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