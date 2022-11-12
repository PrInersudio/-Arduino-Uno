#ifndef __MINESWAPPER_H__
#define __MINESWAPPER_H__
//#define RECURSIVE_OPEN
//#define STRESS_TEST
#include "game_os.h"
#include "GameMovements.h"

#define MINESWAPPERLOSE_DUR 5
#define MINESWAPPERWON_DUR 8
#define MINESWAPPERMAKEMOVESOUNDDUR 1
#define MINESWAPPERSTARTTUNEDUR 5

#define BOMBS_NUM_EASY 10
#define BOMBS_NUM_MIDDLE 15
#define BOMBS_NUM_HARD 20

#define EMPTY_COLOR 0x0
#define ONE_BOMB 0x0000FF
#define TWO_BOMBS 0x00FF00
#define THREE_BOMBS 0x4B0082
#define FOUR_BOMBS 0xFFFF00
#define FIVE_BOMBS 0xFF1493
#define SIX_BOMBS 0x00FFFF
#define SEVEN_BOMBS 0xCD5C5C
#define EIGHT_BOMBS 0xB8860B
#define CLOSED_FIELD 0xFFFFFF
#define FLAG_COLOR 0xFF4500
#define MENU_COLOR 0xFF0000
#define BOMB_COLOR 0xFF0000

void Mineswapper();
void MineswapperSetup();
void MineswapperIcon();
#endif