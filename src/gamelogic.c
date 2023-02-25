#define _XOPEN_SOURCE_EXTENDED

#include <string.h>
#include <wctype.h>

#include "gamelogic.h"


#define ALGEBRAIC_KING   L'K'
#define ALGEBRAIC_QUEEN  L'Q'
#define ALGEBRAIC_ROOK   L'R'
#define ALGEBRAIC_BISHOP L'B'
#define ALGEBRAIC_KNIGHT L'N'

const wchar_t files[] = { L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h' };
const int num_files = 8;


/** Parse a move in algebraic notation that is not castling/draw offer. */
static bool parse_algebraic_move(wchar_t *notation, ChessMove *chess_move);

/** Parse a castling move in algebraic notation. */
static bool parse_algebraic_castling(wchar_t *notation, ChessMove *chess_move);

/** Given a letter, such as Q, and a ChessMove, return the correct ChessPiece object. */
static ChessPiece get_piece_from_algebraic(wchar_t letter, ChessMove *chess_move);


bool parse_algebraic_notation(wchar_t *notation, ChessPlayer *player, ChessMove *chess_move)
{
    chess_move->player = player;
    chess_move->piece = PC_NULL;
    chess_move->from_position[0] = -1;
    chess_move->from_position[1] = -1;
    chess_move->to_position[0] = -1;
    chess_move->to_position[1] = -1;
    chess_move->move_type = SPECIAL_MOVE_NULL;
    chess_move->promotion_piece = PC_NULL;

    switch (notation[0]) {
        case L'0':
            return parse_algebraic_castling(notation, chess_move);
        case L'=':
            chess_move->move_type = SPECIAL_MOVE_DRAW_OFFER;
            return true;
        default:
            return parse_algebraic_move(notation, chess_move);
    }
}

static bool parse_algebraic_move(wchar_t *notation, ChessMove *chess_move)
{
    const unsigned char not_chosen = 0, chosen_empty = 1, chosen = 2;

    bool letter_chosen = false;

    unsigned char file_start_chosen = not_chosen;
    unsigned char rank_start_chosen = not_chosen;

    bool file_end_chosen  = false;
    bool rank_end_chosen  = false;
    bool promotion_chosen = false;

    for (wchar_t *letter = notation; *letter; letter++) {
        if (*letter == L'x' || *letter == L'+' || *letter == L'=' || *letter == L'#') {
            continue;
        } else if (!letter_chosen) {
            chess_move->piece = get_piece_from_algebraic(*letter, chess_move);
            if (chess_move->piece == PC_NULL) {
                chess_move->piece = chess_move->player->is_white ? PC_WHITE_PAWN : PC_BLACK_PAWN;
                letter--;
            }
            letter_chosen = true;
        } else if (file_start_chosen == not_chosen) {
            wchar_t file_lower = towlower(*letter);
            for (int i = 0; i < num_files; i++) {
                if (file_lower == files[i]) {
                    chess_move->from_position[1] = i;
                    file_start_chosen = chosen;
                    break;
                }
            }

            if (file_start_chosen == not_chosen) {
                file_start_chosen = chosen_empty;
                letter--;
            }
        } else if (rank_start_chosen == not_chosen) {
            wint_t rank = *letter - '0';
            if (rank < 1 || rank > 8) {
                rank_start_chosen = chosen_empty;
                letter--;
            } else {
                chess_move->from_position[0] = 8 - rank;
                rank_start_chosen = chosen;
            }
        } else if (!file_end_chosen) {
            wchar_t file_lower = towlower(*letter);
            for (int i = 0; i < num_files; i++) {
                if (file_lower == files[i]) {
                    chess_move->to_position[1] = i;
                    file_end_chosen = true;
                    break;
                }
            }

            // If there isn't another rank/file present, that means that the
            // rank and file we collected earlier are the end rank and file,
            // not the start rank and file.
            if (!file_end_chosen) {
                if (file_start_chosen == chosen_empty || rank_start_chosen == chosen_empty) {
                    return false;
                }
                chess_move->to_position[0] = chess_move->from_position[0];
                chess_move->to_position[1] = chess_move->from_position[1];
                chess_move->from_position[0] = -1;
                chess_move->from_position[1] = -1;

                file_end_chosen = true;
                rank_end_chosen = true;

                letter--;
            }
        } else if (!rank_end_chosen) {
            wint_t rank = *letter - '0';
            if (rank < 1 || rank > 8) {
                return false;
            } else {
                chess_move->to_position[0] = 8 - rank;
                rank_end_chosen = true;
            }
        } else if (!promotion_chosen) {
            chess_move->promotion_piece = get_piece_from_algebraic(*letter, chess_move);
            if (chess_move->promotion_piece == PC_NULL) {
                return false;
            }
            chess_move->move_type = SPECIAL_MOVE_PROMOTION;
            promotion_chosen = true;
        } else {
            return false; // Invalid char.
        }
    }

    // For a move such as Be5, the position shift would not have happened because
    // we would have run out of characters before that moment, so it will be done
    // here.
    if (chess_move->to_position[0] == -1 && chess_move->to_position[1] == -1) {
        chess_move->to_position[0] = chess_move->from_position[0];
        chess_move->to_position[1] = chess_move->from_position[1];
        chess_move->from_position[0] = -1;
        chess_move->from_position[1] = -1;
    }

    if (chess_move->piece == PC_NULL || chess_move->to_position[0] == -1 || chess_move->to_position[1] == -1) {
        return false;
    }

    return true;
}

static bool parse_algebraic_castling(wchar_t *notation, ChessMove *chess_move)
{
    if (wcscmp(notation, L"0-0")) {
        chess_move->move_type = SPECIAL_MOVE_CASTLING;
        return true;
    } else if (wcscmp(notation, L"0-0-0")) {
        chess_move->move_type = SPECIAL_MOVE_QUEEN_SIDE_CASTLING;
        return true;
    } else {
        return false;
    }
}

static ChessPiece get_piece_from_algebraic(wchar_t letter, ChessMove *chess_move)
{
    switch (towupper(letter)) {
        case ALGEBRAIC_KING:
            return chess_move->player->is_white ? PC_WHITE_KING : PC_BLACK_KING;
        case ALGEBRAIC_QUEEN:
            return chess_move->player->is_white ? PC_WHITE_QUEEN : PC_BLACK_QUEEN;
        case ALGEBRAIC_ROOK:
            return chess_move->player->is_white ? PC_WHITE_ROOK : PC_BLACK_ROOK;
        case ALGEBRAIC_BISHOP:
            return chess_move->player->is_white ? PC_WHITE_BISHOP : PC_BLACK_BISHOP;
        case ALGEBRAIC_KNIGHT:
            return chess_move->player->is_white ? PC_WHITE_KNIGHT : PC_BLACK_KNIGHT;
        default:
            return PC_NULL;
    }
}

bool make_move(ChessBoard board, ChessMove *chess_move)
{
    return false;
}

WinStatus should_game_end(ChessBoard board)
{
    return WS_DRAW;
}
