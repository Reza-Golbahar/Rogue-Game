// unicode_display.c
#include <locale.h>    // For setlocale
#include <ncurses.h>   // Ncurses library
#include <wchar.h>     // For wchar_t

int main() {
    // 1. Initialize locale to support Unicode
    setlocale(LC_ALL, "");

    // 2. Initialize ncurses
    initscr();              // Start curses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo pressed keys
    keypad(stdscr, TRUE);   // Enable function and arrow keys
    curs_set(0);            // Hide the cursor

    // 3. Optional: Initialize colors if supported
    if (has_colors()) {
        start_color();                              // Start color functionality
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);   // Define color pair 1
        attron(COLOR_PAIR(1));                      // Activate color pair 1
    }

    // 4. Define row and col where the symbol will be printed
    int row = 10; // Y-coordinate
    int col = 30; // X-coordinate

    // 5. Define the Unicode symbol ⚒️ (Hammer and Pick)
    wchar_t symbol[] = L"⚒️";

    // 6. Move to (row, col) and add the wide string
    mvaddwstr(row, col, symbol);

    // 7. Refresh to update the screen
    refresh();

    // 8. Wait for user input before exiting
    getch();

    // 9. End ncurses mode
    endwin();

    return 0;
}
