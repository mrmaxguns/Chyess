/*
 * Entrypoint for Chyess.
 *
 * Implements UI and user input handling functions.
 */
#define _XOPEN_SOURCE_EXTENDED

#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wctype.h>

#include <ncursesw/curses.h>

#include "ai.h"
#include "board.h"
#include "gamelogic.h"


#define INPUT_BUF_SIZE 20 /** The size of the input buffer used by prompt_win. */


/** Play a game of chess again and again until the user decides to quit. */
static void interactive_session(WINDOW *game_win, WINDOW *prompt_win);

/** Play a single chess game. */
static void play_game(WINDOW *game_win, WINDOW *prompt_win);

/** Initialize a player's type by prompting the user. */
static void init_player_type(WINDOW *prompt_win, ChessPlayer *player);

/**
 * Ask the player for a move. The return value signifies whether the game should
 * end. Only return a valid move, by prompting the user repeatedly if it is invalid.
 */
static WinStatus human_player_move(WINDOW *prompt_win, ChessBoard board, ChessPlayer *player);

/********** Prompt window utilities **********/

/** Clear the prompt window. Do not refresh. */
static void prompt_win_clear(WINDOW *win);

/** Display a prompt to the prompt window and refresh it. */
static void prompt_win_message(WINDOW *win, const wchar_t *message);

/**
 * Prompt the user for an answer up to `INPUT_BUF_SIZE` characters long. Refresh
 * the window as needed, but do not refresh once the user has submitted their
 * answer.
 */
static void prompt_win_input(WINDOW *win, const wchar_t *prompt, wchar_t response[INPUT_BUF_SIZE + 1]);

/**
 * Prompt the user and parse the input like wscanf.
 */
static int prompt_win_wscanf(WINDOW *win, const wchar_t *prompt, const wchar_t *format, ...);


int main(void)
{
    setlocale(LC_ALL, "");

    initscr();

    if (has_colors() == FALSE) {
        endwin();
        fwprintf(stderr, L"Your terminal does not support color.\n");
        return EXIT_FAILURE;
    }
    start_color();

    cbreak();
    noecho();
    curs_set(0);
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);

    int rows, cols;
    (void)rows; // Unused variable
    getmaxyx(stdscr, rows, cols);

    // Title
    static const wchar_t title_msg[] = L"Chyess by Maxim Rebguns";
    attr_set(A_BOLD, 0, NULL);
    mvaddwstr(0, (cols - (sizeof title_msg / sizeof(wchar_t))) / 2, title_msg);
    attr_set(A_NORMAL, 0, NULL);
    refresh();

    // Game board
    WINDOW *game_win = newwin(BRD_RENDER_HEIGHT, BRD_RENDER_WIDTH, 1, (cols - BRD_RENDER_WIDTH) / 2);

    // Prompt window
    WINDOW *prompt_win = newwin(1, cols, BRD_RENDER_HEIGHT + 1, 0);

    // Play the game!
    interactive_session(game_win, prompt_win);

    endwin();
    return EXIT_SUCCESS;
}

static void interactive_session(WINDOW *game_win, WINDOW *prompt_win)
{
    while (true) {
        wclear(game_win);
        wrefresh(game_win);

        play_game(game_win, prompt_win);

        wchar_t play_again;
        if (prompt_win_wscanf(prompt_win, L"Do you want to play again? [y/N] ", L"%lc", &play_again) == 1) {
            if (towupper(play_again) == 'Y') {
                continue;
            }
        }
        break;
    }
}

static void play_game(WINDOW *game_win, WINDOW *prompt_win)
{
    ChessPlayer white_player, black_player;

    white_player.is_white = true;
    prompt_win_message(prompt_win, L"Configure player (white)...");
    sleep(1);
    init_player_type(prompt_win, &white_player);

    black_player.is_white = false;
    prompt_win_message(prompt_win, L"Configure player (black)...");
    sleep(1);
    init_player_type(prompt_win, &black_player);

    ChessBoard board;
    brd_init(board);

    bool current_player_is_white = true;

    // Game loop.
    WinStatus game_status = WS_CONTINUE;
    while (game_status == WS_CONTINUE) {
        ChessPlayer *current_player;
        if (current_player_is_white) {
            current_player = &white_player;
        } else {
            current_player = &black_player;
        }

        brd_render(board, game_win);
        wrefresh(game_win);

        if (current_player->is_human) {
            game_status = human_player_move(prompt_win, board, current_player);
        } else {
            game_status = ai_player_move(board, current_player);
        }

        current_player_is_white = !current_player_is_white;
    }

    if (game_status == WS_WHITE) {
        prompt_win_message(prompt_win, L"Player 2 has won!");
    } else if (game_status == WS_BLACK) {
        prompt_win_message(prompt_win, L"Player 1 has won!");
    } else {
        prompt_win_message(prompt_win, L"A draw ocurred.");
    }
    sleep(2);
}

static void init_player_type(WINDOW *prompt_win, ChessPlayer *player)
{
    while (true) {
        wchar_t player_type;
        if (prompt_win_wscanf(prompt_win, L"PLayer type [(h)uman/(b)ot]: ", L"%lc", &player_type) == 1) {
            if (towupper(player_type) == 'H') {
                player->is_human = true;
                break;
            } else if (towupper(player_type) == 'B') {
                player->is_human = false;
                break;
            }
        }
        // Invalid type.
        prompt_win_message(prompt_win, L"Invalid player type.");
    }
}

static WinStatus human_player_move(WINDOW *prompt_win, ChessBoard board, ChessPlayer *player)
{
    if (player->is_white) {
        prompt_win_message(prompt_win, L"White's turn...");
    } else {
        prompt_win_message(prompt_win, L"Black's turn...");
    }
    sleep(1);

    while (true) {
        wchar_t move_instruction[INPUT_BUF_SIZE + 1];
        prompt_win_input(prompt_win, L"Move (algebraic notation): ", move_instruction);

        ChessMove user_move;
        if (!parse_algebraic_notation(move_instruction, player, &user_move)) {
            prompt_win_message(prompt_win, L"Invalid move, incorrect use of algebraic notation.");
            sleep(1);
            continue;
        }

        if (!make_move(board, &user_move)) {
            prompt_win_message(prompt_win, L"Invalid move.");
            sleep(1);
            continue;
        } else {
            break;
        }
    }

    return should_game_end(board);
}

/* Prompt window utilities. */
static void prompt_win_clear(WINDOW *win)
{
    werase(win);
}

static void prompt_win_message(WINDOW *win, const wchar_t *message)
{
    prompt_win_clear(win);
    mvwaddwstr(win, 0, 0, message);
    wrefresh(win);
}

static void prompt_win_input(WINDOW *win, const wchar_t *prompt, wchar_t response[INPUT_BUF_SIZE + 1])
{
    prompt_win_message(win, prompt);

    // Show the cursor.
    curs_set(1);

    // Track where the prompt ends so that the user cannot modify the prompt.
    int prompt_end_row, prompt_end_col;
    (void)prompt_end_row;
    getyx(win, prompt_end_row, prompt_end_col);

    int i = 0;
    while (true) {
        wint_t ch;
        int status = get_wch(&ch);

        if (status == OK) {
            // Handle submission.
            if (ch == '\n' || ch == WEOF) {
                break;
            }

            // Do not read in any more input after buffer size is reached. Continuing
            // allows the user to delete/modify characters before submitting.
            if (i >= INPUT_BUF_SIZE) {
                continue;
            }

            response[i] = (wchar_t)ch;

            // Echo the input.
            cchar_t input_char;
            setcchar(&input_char, &response[i], WA_NORMAL, 0, NULL);
            wadd_wch(win, &input_char);
            wrefresh(win);

            i++;
        } else if (status == KEY_CODE_YES) {
            int rows, cols;
            (void)rows;
            getyx(win, rows, cols);

            // TODO: Implement arrow keys.
            if (ch == KEY_BACKSPACE && cols > prompt_end_col) {
                // Handle backspace.
                wmove(win, rows, cols - 1);
                waddwstr(win, L" ");
                wmove(win, rows, cols - 1);
                wrefresh(win);

                i--;
            } else if (ch == KEY_ENTER) {
                // Handle submission.
                break;
            } else {
                // Ignore other special keys.
                continue;
            }
        } else {
            // Break if an error reading input ocurred.
            break;
        }
    }
    response[i] = L'\0';

    // Re-hide the cursor.
    curs_set(0);
}

static int prompt_win_wscanf(WINDOW *win, const wchar_t *prompt, const wchar_t *format, ...)
{
    wchar_t input[INPUT_BUF_SIZE + 1];
    prompt_win_input(win, prompt, input);

    int return_value;

    va_list args;
    va_start(args, format);
    return_value = vswscanf(input, format, args);
    va_end(args);

    return return_value;
}
