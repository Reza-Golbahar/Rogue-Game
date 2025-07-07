#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "users.h"

// File handling functions
void handle_file_error(const char* operation) {
    clear();
    printw("Error: Could not %s file.\n", operation);
    printw("Press any key to continue...");
    refresh();
    getch();
}

// Password validation functions
int hasupper(const char password[]) {
    size_t len = strlen(password);
    for (size_t i = 0; i < len; i++) {
        if (password[i] >= 65 && password[i] <= 90)
            return 1;
    }
    return 0;
}

int haslower(const char password[]) {
    size_t len = strlen(password);
    for (size_t i = 0; i < len; i++) {
        if (password[i] >= 97 && password[i] <= 122)
            return 1;
    }
    return 0;
}

int hasdigit(const char password[]) {
    size_t len = strlen(password);
    for (size_t i = 0; i < len; i++) {
        if (password[i] >= 48 && password[i] <= 57)
            return 1;
    }
    return 0;
}

bool validate_password(char *password) {
    if (strlen(password) < 7) {
        printw("Password must be at least 7 characters long\n");
        return false;
    }
    if (!hasupper(password)) {
        printw("Password must contain at least one uppercase letter\n");
        return false;
    }
    if (!haslower(password)) {
        printw("Password must contain at least one lowercase letter\n");
        return false;
    }
    if (!hasdigit(password)) {
        printw("Password must contain at least one number\n");
        return false;
    }
    return true;
}

bool validate_email(const char *email) {
    const char *at = strchr(email, '@');
    if (!at) return false;
    
    const char *dot = strchr(at, '.');
    if (!dot || dot == at + 1 || *(dot + 1) == '\0') {
        printw("Invalid email format. Must be xxxx@yyy.zzz\n");
        return false;
    }
    return true;
}

// User management functions
struct UserManager* create_user_manager(void) {
    struct UserManager* manager = malloc(sizeof(struct UserManager));
    if (manager == NULL) {
        endwin();
        fprintf(stderr, "Failed to allocate memory for user manager\n");
        exit(1);
    }
    if (manager != NULL) {
        manager->user_count = 0;
        manager->current_user = NULL;
        memset(manager->users, 0, sizeof(struct User) * MAX_USERS);
        load_users_from_json(manager);
    }
    return manager;
}

void free_user_manager(struct UserManager* manager) {
    if (manager != NULL) {
        free(manager);
    }
}

void load_users_from_json(struct UserManager* manager) {
    FILE* file = fopen("users.json", "r");
    if (file == NULL) {
        manager->user_count = 0;
        return;
    }

    char line[256];
    manager->user_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "\"username\":") != NULL) {
            struct User* user = &manager->users[manager->user_count];

            // Parse username from *this* line
            sscanf(line, " \"username\": \"%[^\"]\",", user->username);

            // Next line => password
            fgets(line, sizeof(line), file);
            sscanf(line, " \"password\": \"%[^\"]\",", user->password);

            // Next line => email
            fgets(line, sizeof(line), file);
            sscanf(line, " \"email\": \"%[^\"]\",", user->email);

            // Next line => "score"
            fgets(line, sizeof(line), file);
            char score_str[16];
            sscanf(line, " \"score\": \"%[^\"]\"", score_str);
            user->score = atoi(score_str);

            // Next line => "gold_collected"
            fgets(line, sizeof(line), file);
            char gold_str[16];
            sscanf(line, " \"gold_collected\": \"%[^\"]\"", gold_str);
            user->gold = atoi(gold_str);

            // Next line => difficulty
            fgets(line, sizeof(line), file);
            sscanf(line, " \"difficulty\": %d,", &user->difficulty);

            // Next line => color
            fgets(line, sizeof(line), file);
            sscanf(line, " \"color\": \"%[^\"]\",", user->character_color);

            // Next line => song
            fgets(line, sizeof(line), file);
            sscanf(line, " \"song\": %d,", &user->song);

            // Next line => games_played
            fgets(line, sizeof(line), file);
            sscanf(line, " \"games_played\": %d,", &user->games_completed);

            fgets(line, sizeof(line), file); 
            int musicInt = 1;
            sscanf(line, " \"music_on\": %d,", &musicInt);
            user->music_on = (musicInt != 0);

            long first_t, last_t;
            fgets(line, sizeof(line), file);
            sscanf(line, " \"first_game_time\": %ld,", &first_t);
            user->first_game_time = (time_t)first_t;

            fgets(line, sizeof(line), file);
            sscanf(line, " \"last_game_time\": %ld,", &last_t);
            user->last_game_time = (time_t)last_t;

            // Next line => days_since_first_game
            // fgets(line, sizeof(line), file);
            // sscanf(line, " \"days_since_first_game\": %d", &user->days_since_first_game);

            // *Now* we've read 10 lines total. 
            // If your JSON has a trailing "}" line, you might do one more fgets, or 
            // it might be read at the top of the next iteration. It's up to you.

            // Store the username in manager->usernames
            strncpy(manager->usernames[manager->user_count], 
                    user->username, MAX_STRING_LEN - 1);
            manager->usernames[manager->user_count][MAX_STRING_LEN - 1] = '\0';

            manager->user_count++;
        }
    }

    fclose(file);
}

void save_users_to_json(struct UserManager* manager) {
    FILE* file = fopen("users.json", "w");
    if (file == NULL) {
        handle_file_error("write");
        return;
    }

    fprintf(file, "[\n");
    bool firstPrinted = false;

    for (int i = 0; i < manager->user_count; i++) {
        struct User* user = &manager->users[i];

        // If username is "Guest", skip saving
        if (strcmp(user->username, "Guest") == 0) {
            continue; 
        }

        // Print a comma + newline before each user except the very first
        if (firstPrinted) {
            fprintf(file, ",\n");
        } else {
            firstPrinted = true;
        }

        // Then save the user as normal
        fprintf(file, "  {\n");
        fprintf(file, "    \"username\": \"%s\",\n", user->username);
        fprintf(file, "    \"password\": \"%s\",\n", user->password);
        fprintf(file, "    \"email\": \"%s\",\n",    user->email);
        fprintf(file, "    \"score\": \"%d\",\n",    user->score);
        fprintf(file, "    \"gold_collected\": \"%d\",\n", user->gold);
        fprintf(file, "    \"difficulty\": %d,\n",    user->difficulty);
        fprintf(file, "    \"color\": \"%s\",\n",     user->character_color);
        fprintf(file, "    \"song\": %d,\n",          user->song);
        fprintf(file, "    \"games_played\": %d,\n",  user->games_completed);
        fprintf(file, "    \"music_on\": %d,\n",      user->music_on ? 1 : 0);
        fprintf(file, "    \"first_game_time\": %ld,\n", (long)user->first_game_time);
        fprintf(file, "    \"last_game_time\": %ld\n", (long)user->last_game_time);
        // If you want to store days_since_first_game, do so too
        fprintf(file, "  }");
    }

    fprintf(file, "\n]\n");
    fclose(file);
}


bool authenticate_user(struct UserManager* manager, int index, const char* password) {
    if (index < 0 || index >= manager->user_count) {
        return false;
    }
    
    if (strcmp(manager->users[index].password, password) == 0) {
        manager->current_user = &manager->users[index];
        return true;
    }
    return false;
}

void print_users(struct UserManager* manager) {
    if (manager->user_count == 0) {
        printw("No users found.\n");
        return;
    }

    mvprintw(2, 0, "Usernames");
    mvprintw(2, 30, "Emails");
    mvprintw(2, 60, "Score");

    for (int i = 0; i < manager->user_count; i++) {
        mvprintw(i + 3, 0, "%d-%s", i + 1, manager->users[i].username);
        mvprintw(i + 3, 30, "%s", manager->users[i].email);
        mvprintw(i + 3, 60, "%d", manager->users[i].score);
    }
}

void print_scoreboard(struct UserManager* manager) {
    if (manager->user_count == 0) {
        mvprintw(0, 0, "No users found.\n");
        refresh();
        getch();
        return;
    }

    // Create temporary array for sorting
    struct User sorted_users[MAX_USERS];
    memcpy(sorted_users, manager->users, sizeof(struct User) * manager->user_count);

    // Sort users by score in descending order
    for (int i = 0; i < manager->user_count - 1; i++) {
        for (int j = i + 1; j < manager->user_count; j++) {
            if (sorted_users[j].score > sorted_users[i].score) {
                struct User temp = sorted_users[i];
                sorted_users[i] = sorted_users[j];
                sorted_users[j] = temp;
            }
        }
    }

    // Initialize colors
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);  // Gold (1st place)
    init_pair(2, COLOR_WHITE, COLOR_BLACK);   // Silver (2nd place)
    init_pair(3, COLOR_RED, COLOR_BLACK);     // Bronze (3rd place)
    init_pair(4, COLOR_GREEN, COLOR_BLACK);   // Current user
    init_pair(5, COLOR_CYAN, COLOR_BLACK);    // Headers

    // Variables for pagination
    int current_page = 0;
    int total_pages = (manager->user_count + USERS_PER_PAGE - 1) / USERS_PER_PAGE;
    bool running = true;
    time_t current_time = time(NULL);

    while (running) {
        clear();

        // Print header with current time
        attron(COLOR_PAIR(5) | A_BOLD);
        mvprintw(0, 0, "SCOREBOARD - Page %d/%d", current_page + 1, total_pages);
        mvprintw(0, 40, "Current Time: 2025-01-04 18:07:37");
        
        // Print column headers
        mvprintw(2, 0,  "Rank");
        mvprintw(2, 8,  "Username");
        mvprintw(2, 25, "Score");
        mvprintw(2, 35, "Gold");
        mvprintw(2, 45, "Games");
        mvprintw(2, 55, "Experience");
        mvprintw(2, 70, "Title");
        attroff(COLOR_PAIR(5) | A_BOLD);

        // Print horizontal line
        mvprintw(3, 0, "--------------------------------------------------------------------------------");

        // Calculate range for current page
        int start_idx = current_page * USERS_PER_PAGE;
        int end_idx = MIN(start_idx + USERS_PER_PAGE, manager->user_count);

        // Print users
        for (int i = start_idx; i < end_idx; i++) {
            int row = i - start_idx + 4;
            struct User* user = &sorted_users[i];
            
            // Calculate experience days
            int experience_days = (int)((current_time - user->first_game_time) / (24 * 3600));

            // Set appropriate color and style
            if (i < 3) {
                // Top 3 players get special colors and medals
                attron(COLOR_PAIR(i + 1) | A_BOLD | A_ITALIC);
                const char* medals[] = {"🏆", "🥈", "🥉"};
                const char* titles[] = {"GOAT", "Legend", "Champion"};
                mvprintw(row, 0, "%s %d", medals[i], i + 1);
                mvprintw(row, 70, "%s", titles[i]);
                attroff(COLOR_PAIR(i + 1) | A_BOLD | A_ITALIC);
            } else {
                // Normal ranking
                mvprintw(row, 0, "%d", i + 1);
            }

            // Highlight current user's row
            if (manager->current_user && 
                strcmp(user->username, manager->current_user->username) == 0) {
                attron(COLOR_PAIR(4) | A_BOLD);
                mvprintw(row, 80, "You****");
            }
            
            if (i<3 && strcmp(user->username, manager->current_user->username) != 0)
                attron(COLOR_PAIR(i + 1) | A_BOLD | A_ITALIC);

            // Print user information
            mvprintw(row, 8,  "%s", user->username);
            mvprintw(row, 25, "%d", user->score);
            mvprintw(row, 35, "%d", user->gold);
            mvprintw(row, 45, "%d", user->games_completed);
            mvprintw(row, 55, "%d days", experience_days);

            // Reset attributes
            attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | COLOR_PAIR(4) | A_BOLD | A_ITALIC);
        }


        // Print navigation instructions
        mvprintw(USERS_PER_PAGE + 6, 0, "Navigation: 'n' - Next Page, 'p' - Previous Page, 'q' - Quit");
        refresh();

        // Handle input
        int ch = getch();
        switch (ch) {
            case 'n':
                if (current_page < total_pages - 1) current_page++;
                break;
            case 'p':
                if (current_page > 0) current_page--;
                break;
            case 'q':
                running = false;
                break;
        }
    }
}
