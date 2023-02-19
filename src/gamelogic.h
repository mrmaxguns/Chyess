#ifndef CHESS_GAME_LOGIC
#define CHESS_GAME_LOGIC

#include "stdbool.h"

typedef struct {
    bool is_white; /** Whether the player is playing as white/black. */
    bool is_human; /** Whether the player is a human or bot. */
    bool has_lost; /** Whether the player has lost the game. */
} ChessPlayer;

typedef struct {

} ChessMove;

#endif
