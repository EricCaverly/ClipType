
#ifndef ClipPaste_h   /* Include guard */
#define ClipPaste_h

int get_clipboard_contents_ascii(char output[], int size);
void uppercase_press(char key);
void lowercase_press(char key);
void word_keypress(char text[], int size, int delay_ms);
void clear_modifiers();
void backspace_garbage(int count);

#endif