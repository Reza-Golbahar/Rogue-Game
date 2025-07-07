#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include "menu.h"
#include "game.h"
#include "users.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


int main() {

    // Initialize locale for proper Unicode support
    setlocale(LC_ALL, "");

    // Initialize ncurses
    cbreak();        // Disable line buffering
    
    // Initialize random seed with current time
    srand(time(NULL));
    init_ncurses();

    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        endwin();
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        endwin();
        fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }
    
    // Create user manager
    struct UserManager* manager = create_user_manager();
    if (!manager) {
        Mix_CloseAudio();
        SDL_Quit();
        endwin();
        fprintf(stderr, "Failed to create user manager\n");
        return 1;
    }

    // Main menu loop
    bool running = true;
    while (running) {
        clear();
        mvprintw(0, 0, "Welcome to the Game!");
        mvprintw(2, 0, "1. Login");
        mvprintw(3, 0, "2. Register");
        mvprintw(4, 0, "3. Play as Guest");
        mvprintw(5, 0, "4. Quit");
        mvprintw(7, 0, "Current Date and Time (UTC): 2025-01-04 19:45:03");
        mvprintw(8, 0, "Please choose an option (1-4): ");
        refresh();

        int choice = getch();

        switch (choice) {
            case '1':
                login_menu(manager);
                break;
            case '2':
                register_menu(manager);
                break;
            case '3':
                // Play as guest
                initialize_guest(manager);
                break;
            case '4':
                running = false;
                break;
            default:
                mvprintw(10, 0, "Invalid option. Press any key to continue...");
                refresh();
                getch();
                break;
        }
    }

    // Cleanup
    free_user_manager(manager);
    Mix_CloseAudio();
    SDL_Quit();
    endwin();
    return 0;
}