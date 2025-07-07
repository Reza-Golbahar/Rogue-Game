#ifndef USERS_H
#define USERS_H

#include <time.h>
#include <stdbool.h>

#define MAX_USERS 100
#define MAX_STRING_LEN 100
#define USERS_PER_PAGE 10

#define MIN(a,b) ((a) < (b) ? (a) : (b))


struct GameState {
    int level;
    int score;
    int gold;
    time_t start_time;
    time_t last_saved_time;
};


struct User {
    char username[MAX_STRING_LEN];
    char password[MAX_STRING_LEN];
    char email[MAX_STRING_LEN];
    int score;
    int gold;
    int games_completed;
    time_t first_game_time;
    time_t last_game_time;
    int days_since_first_game; // New field

    struct GameState game_state;  // Add the game state here

    // New settings attributes
    int difficulty;
    char character_color[20];
    int song;

    bool music_on;  // default: true (1)
};

struct UserManager {
    struct User users[MAX_USERS];
    char usernames[MAX_USERS][MAX_STRING_LEN];
    int user_count;
    struct User* current_user;
};


// Function declarations
struct UserManager* create_user_manager(void);
void free_user_manager(struct UserManager* manager);
void load_users_from_json(struct UserManager* manager);
void save_users_to_json(struct UserManager* manager);
bool authenticate_user(struct UserManager* manager, int index, const char* password);
void print_users(struct UserManager* manager);
void print_scoreboard(struct UserManager* manager);
void handle_file_error(const char* operation);


// Password validation functions
int hasupper(const char password[]);
int haslower(const char password[]);
int hasdigit(const char password[]);
bool validate_password(char *password);
bool validate_email(const char *email);

#endif