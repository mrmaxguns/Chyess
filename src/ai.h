#ifndef CHESS_AI_H
#define CHESS_AI_H

#define _XOPEN_SOURCE_EXTENDED

#include <ncursesw/ncurses.h>

#include "board.h"
#include "gamelogic.h"


/**
 * Have the AI make a valid move. If VALIDATE_AI is enabled, check the AI's move
 * for validity, at the tradeoff of performance. Return value signifies whether
 * the game should end (AI has lost).
 */
bool ai_player_move(WINDOW *prompt_win, ChessBoard *board, ChessPlayer *player);

#endif
