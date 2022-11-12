#ifndef __TICTACTOE_H__
#define __TICTACTOE_H__
#include "game_os.h"
#include "GameMovements.h"

//цвета
#define COLOR1 0xC41E3A
#define COLOR2 0x4169E1
#define GREEN 0x008000

// длительность музыки
#define PLAYER1WINTUNEDUR 4
#define PLAYER2WINTUNEDUR 4
#define TIETUNEDUR 4
#define PUTTUNEDUR 1
#define STARTTUNEDUR 4

void TicTacToe();
void TicTacToeIcon();
void TicTacToeSetup();
#endif