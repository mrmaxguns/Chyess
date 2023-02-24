/**
 * Functions for handling and rendering the chess board.
 */
#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#define _XOPEN_SOURCE_EXTENDED

#include <wchar.h>

#include <ncursesw/curses.h>


// Actual width and height of the chess board.
#define BRD_SIZE 8

// The width and height of the chess board in characters when rendered to the screen.
#define BRD_RENDER_WIDTH  20
#define BRD_RENDER_HEIGHT 11

// Chess piece characters.
#define SYMBOL_BOX_HORIZONTAL L'\u2500'
#define SYMBOL_BOX_VERTICAL   L'\u2502'
#define SYMBOL_BOX_TOP_LEFT   L'\u250C'
#define SYMBOL_BOX_TOP_RIGHT  L'\u2510'
#define SYMBOL_BOX_BOT_LEFT   L'\u2514'
#define SYMBOL_BOX_BOT_RIGHT  L'\u2518'

#define SYMBOL_WHITE_KING   L'\u2654'
#define SYMBOL_WHITE_QUEEN  L'\u2655'
#define SYMBOL_WHITE_ROOK   L'\u2656'
#define SYMBOL_WHITE_BISHOP L'\u2657'
#define SYMBOL_WHITE_KNIGHT L'\u2658'
#define SYMBOL_WHITE_PAWN   L'\u2659'

#define SYMBOL_BLACK_KING   L'\u265A'
#define SYMBOL_BLACK_QUEEN  L'\u265B'
#define SYMBOL_BLACK_ROOK   L'\u265C'
#define SYMBOL_BLACK_BISHOP L'\u265D'
#define SYMBOL_BLACK_KNIGHT L'\u265E'
#define SYMBOL_BLACK_PAWN   L'\u265F'


/** Represents the spaces on the chess board. */
typedef enum {
    PC_NULL, /** Represents no chess piece on a given square. */

    PC_WHITE_KING,
    PC_WHITE_QUEEN,
    PC_WHITE_ROOK,
    PC_WHITE_BISHOP,
    PC_WHITE_KNIGHT,
    PC_WHITE_PAWN,

    PC_BLACK_KING,
    PC_BLACK_QUEEN,
    PC_BLACK_ROOK,
    PC_BLACK_BISHOP,
    PC_BLACK_KNIGHT,
    PC_BLACK_PAWN,
} ChessPiece;

/** Represents a standard chess board. */
typedef ChessPiece ChessBoard[BRD_SIZE][BRD_SIZE];


/**
 * Initialize a chess board by overwriting all pieces. White is always at the
 * bottom.
 */
void brd_init(ChessBoard board);

/**
 * Draw a chess board to `win`. The window must be at least 11 rows by 19 columns.
 */
void brd_render(ChessBoard board, WINDOW *win);

/** Clear the board window. */
void brd_clear(WINDOW *win);

#endif
