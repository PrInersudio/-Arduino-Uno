#ifndef __GAME_OS_H__
#define __GAME_OS_H__
//#define DEBUG
#include "hardware.h"
#include "ScreenCoords.h"
#include "tune.h"
#include "tictactoe.h"
#include "mineswapper.h"

#define OSWELCOMEDUR 6
#define CLICKINMENUSOUNDDUR 1
#define OSWELCOMECOLOR 0xF75E25

#define GAMES_QUANTITY 2
#define SOUNDICONCOLOR 0xFF0000
#define EXITSIGNALCOLOR 0xFF0000
#define DELAY 100

void ExitSignal(uint32_t* bottom_edge_pixels);
void DrawExit();
extern bool is_start;
extern struct Note ClickInMenuSound[];
#endif