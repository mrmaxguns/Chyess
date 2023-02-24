/**
 * Implementation of chess move validation, board information, and algebraic
 * notation parsing utilities.
 */
#ifndef CHESS_GAME_LOGIC
#define CHESS_GAME_LOGIC

#define _XOPEN_SOURCE_EXTENDED

#include <stdbool.h>

#include "board.h"


/** Represents a person or computer that can play chess. */
typedef struct {
    bool is_white; /** Whether the player is playing as white/black. */
    bool is_human; /** Whether the player is a human or bot. */
    bool has_lost; /** Whether the player has lost the game. */
} ChessPlayer;

/** An enumeration of all possible move types. */
typedef enum {
    SPECIAL_MOVE_NULL,                /** A regular move that doesn't involve a capture or special rule. */
    SPECIAL_MOVE_CAPTURE,
    SPECIAL_MOVE_EN_PASSANT,
    SPECIAL_MOVE_PROMOTION,
    SPECIAL_MOVE_DRAW_OFFER,
    SPECIAL_MOVE_CASTLING,
    SPECIAL_MOVE_QUEEN_SIDE_CASTLING,
    SPECIAL_MOVE_CHECK,
    SPECIAL_MOVE_CHECKMATE,
} SpecialMoveType;

/**
 * Represents a chess move made by a certain player. Does not store the board on
 * which the move was made.
 */
typedef struct {
    ChessPlayer *player;        /** The player making the move. */
    ChessPiece piece;           /** The piece being moved. */

    int from_position[2];       /** The row, col position of the piece to be moved. */
    int to_position[2];         /** The row, col position of the location to be moved to.*/

    SpecialMoveType move_type;  /** The type of move. Regular move is SPECIAL_MOVE_NULL. */
    ChessPiece promotion_piece; /** The piece to which to promote on SPECIAL_MOVE_PROMOTION. */
} ChessMove;

/** Represents the state of the game. */
typedef enum {
    WS_CONTINUE, /** The game should continue; nobody has won or lost. */
    WS_WHITE,
    WS_BLACK,
    WS_DRAW,
} WinStatus;


/** Parse algebraic notation defined in the string `notation` and store it in
    `chess_move` if valid. Return true only if the notation is valid. */
bool parse_algebraic_notation(wchar_t *notation, ChessMove chess_move);

/** Apply `chess_move` to `board`. Return true only if the move can be made. */
bool make_move(ChessBoard board, ChessMove chess_move);

/** Return a win status depending on the current state of the game. */
WinStatus should_game_end(ChessBoard board);

#endif
