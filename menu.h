#ifndef MENU_H
#define MENU_H

#include "users.h"
#include <stdbool.h>
#include "game.h"

#define MAX_USERS 100
#define MAX_STRING_LEN 100
#define PASSWORD_LENGTH 12
#define SPECIAL_CHARACTERS "!@#$%^&*()-_=+[]{}|;:,.<>?/"

struct UserSettings {
    char character_color[20]; // e.g., "red", "green", etc.
    int song;
    int difficulty;
};


void generate_password(char *password, int length);
bool init_ncurses(void);
void first_menu(struct UserManager* manager);
void pre_game_menu(struct UserManager* manager);
bool entering_menu(struct UserManager* manager, int selected_index);
int users_menu(struct UserManager* manager);
void settings(struct UserManager* manager);
void adding_new_user(struct UserManager* manager);
void login_menu(struct UserManager* manager);
void register_menu(struct UserManager* manager);
void game_menu(struct UserManager* manager);
void print_user_profile(struct UserManager *manager);
void start_new_game(struct UserManager* manager);
void continue_game(struct UserManager* manager);
void stop_music();
void play_music(struct User* user);
void initialize_guest(struct UserManager* manager);


#endif