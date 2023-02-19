#define _XOPEN_SOURCE_EXTENDED

#include <ncursesw/curses.h>

#include "board.h"


/** Draw a horizontal line to `data`, given a total `width` and optional left
    padding with spaces. The first and last characters of the line of the line
    will be `start_char` and `end_char`, respectively. The `data` buffer must
    be at least width + 1 (for the NUL terminator). */
static void create_horizontal_line(wchar_t data[], int width, int left_padding,
                            wchar_t start_char, wchar_t end_char);

/** Given a ChessPiece, return the corresponding wchar_t. */
static wchar_t get_symbol_for_piece(ChessPiece piece);


void brd_init(ChessBoard board)
{
    // Initialize empty squares and pawns.
    for (int row = 0; row < BRD_SIZE; row++) {
        for (int col = 0; col < BRD_SIZE; col++) {
            if (row == 1) {
                board[row][col] = PC_BLACK_PAWN;
            } else if (row == BRD_SIZE - 2) {
                board[row][col] = PC_WHITE_PAWN;
            } else {
                board[row][col] = PC_NULL;
            }
        }
    }

    // Initialize all other pieces manually.
    board[0][0] = PC_BLACK_ROOK;
    board[0][1] = PC_BLACK_KNIGHT;
    board[0][2] = PC_BLACK_BISHOP;
    board[0][3] = PC_BLACK_QUEEN;
    board[0][4] = PC_BLACK_KING;
    board[0][5] = PC_BLACK_BISHOP;
    board[0][6] = PC_BLACK_KNIGHT;
    board[0][7] = PC_BLACK_ROOK;

    board[7][0] = PC_WHITE_ROOK;
    board[7][1] = PC_WHITE_KNIGHT;
    board[7][2] = PC_WHITE_BISHOP;
    board[7][3] = PC_WHITE_QUEEN;
    board[7][4] = PC_WHITE_KING;
    board[7][5] = PC_WHITE_BISHOP;
    board[7][6] = PC_WHITE_KNIGHT;
    board[7][7] = PC_WHITE_ROOK;
}

void brd_render(ChessBoard board, WINDOW *win)
{
    wmove(win, 0, 0);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    wchar_t top_line[20] = {0};
    create_horizontal_line(top_line, 20, 1, SYMBOL_BOX_TOP_LEFT, SYMBOL_BOX_TOP_RIGHT);
    waddwstr(win, top_line);

    for (int row = 0; row < BRD_SIZE; row++) {
        // Print the left side of each row. Ex: 2|
        wchar_t left_side[3] = { 0 };
        swprintf(left_side, 3, L"%d%lc", row, SYMBOL_BOX_VERTICAL);
        waddwstr(win, left_side);

        // Print 8 positions in the row. Each position is two characters wide
        // to compensate for character width/height ratio.
        for (int col = 0; col < BRD_SIZE; col++) {
            cchar_t first_char, second_char;
            int color_pair_num = (row % 2 == col % 2) ? 1 : 2;
            wchar_t chess_piece = get_symbol_for_piece(board[row][col]);

            setcchar(&first_char, &chess_piece, WA_NORMAL, color_pair_num, NULL);
            setcchar(&second_char, L" ", WA_NORMAL, color_pair_num, NULL);

            wadd_wch(win, &first_char);
            wadd_wch(win, &second_char);
        }

        // Print a vertical line and newline at the end of the row.
        wchar_t right_side[3] = { SYMBOL_BOX_VERTICAL, L'\n', 0 };
        waddwstr(win, right_side);
    }

    wchar_t bottom_line[20] = {0};
    create_horizontal_line(bottom_line, 20, 1, SYMBOL_BOX_BOT_LEFT, SYMBOL_BOX_BOT_RIGHT);
    waddwstr(win, bottom_line);

    // Letters at the bottom of the board.
    waddwstr(win, L"  ");
    for (int col = 0; col < BRD_SIZE; col++) {
        wchar_t letter[3] = { L'A' + col, L' ', 0 };
        waddwstr(win, letter);
    }
}

void brd_clear(WINDOW *win)
{
    werase(win);
}

static void create_horizontal_line(wchar_t data[], int width, int left_padding,
                            wchar_t start_char, wchar_t end_char)
{
    int i = 0;

    for (; i < left_padding; i++) {
        data[i] = L' ';
    }

    data[i] = start_char;
    for (i++; i < width - 2; i++) {
        data[i] = SYMBOL_BOX_HORIZONTAL;
    }
    data[i] = end_char;
    data[++i] = L'\n';
    data[++i] = L'\0';
}

static wchar_t get_symbol_for_piece(ChessPiece piece)
{
    switch (piece) {
        case PC_WHITE_KING: return SYMBOL_WHITE_KING;
        case PC_WHITE_QUEEN: return SYMBOL_WHITE_QUEEN;
        case PC_WHITE_ROOK: return SYMBOL_WHITE_ROOK;
        case PC_WHITE_BISHOP: return SYMBOL_WHITE_BISHOP;
        case PC_WHITE_KNIGHT: return SYMBOL_WHITE_KNIGHT;
        case PC_WHITE_PAWN: return SYMBOL_WHITE_PAWN;

        case PC_BLACK_KING: return SYMBOL_BLACK_KING;
        case PC_BLACK_QUEEN: return SYMBOL_BLACK_QUEEN;
        case PC_BLACK_ROOK: return SYMBOL_BLACK_ROOK;
        case PC_BLACK_BISHOP: return SYMBOL_BLACK_BISHOP;
        case PC_BLACK_KNIGHT: return SYMBOL_BLACK_KNIGHT;
        case PC_BLACK_PAWN: return SYMBOL_BLACK_PAWN;

        default: return L' ';
    }
}
