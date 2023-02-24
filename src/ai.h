/**
 * Implementation of MaxBF's chess bot algorithm.
 */
#ifndef CHESS_AI_H
#define CHESS_AI_H

#define _XOPEN_SOURCE_EXTENDED

#include <ncursesw/ncurses.h>

#include "board.h"
#include "gamelogic.h"


/**
 * Have the AI make a valid move.
 */
WinStatus ai_player_move(ChessBoard board, ChessPlayer *player);

#endif
