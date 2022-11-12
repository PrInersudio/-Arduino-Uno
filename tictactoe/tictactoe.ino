//#define DEBUG
#include "hardware.h"
#include "ScreenCoords.h"
#include "tune.h"
#include "GameMovements.h"

//цвета
#define COLOR1 0xC41E3A
#define COLOR2 0x4169E1
#define GREEN 0x008000

#define DELAY 100

// музыка
struct Note Player1WinTune[] = { {NOTE_G6, 250}, {NOTE_A6, 250}, {NOTE_B6, 250}, {NOTE_C7, 250} };
struct Note Player2WinTune[] = { {NOTE_G3, 250}, {NOTE_A3, 250}, {NOTE_B3, 250}, {NOTE_C4, 250} };
struct Note TieTune[] = { {NOTE_E3, 250}, {NOTE_D3, 250}, {NOTE_C3, 250}, {NOTE_B2, 250}};
struct Note PutTune[] = { {NOTE_C5, 125} };
struct Note StartTune[] = { {NOTE_E7, 250}, {NOTE_D7, 250}, {NOTE_C7, 250}, {NOTE_D7, 250}};

// длительность музыки
#define PLAYER1WINTUNEDUR 4
#define PLAYER2WINTUNEDUR 4
#define TIETUNEDUR 4
#define PUTTUNEDUR 1
#define STARTTUNEDUR 4

// размер позиции
int pos_size;

//создаёт сетку
void Net (uint32_t color) {
  ScreenCoords pixel;
  
  pos_size = SCREEN_LENGHT/3;
  for (int i = 0; i < SCREEN_HIGHT; i++) {
    pixel = {pos_size,i};
    pixel.DrawPixel(color);
    pixel = {SCREEN_LENGHT - 1 - pos_size,i};
    pixel.DrawPixel(color);
  }

  pos_size = SCREEN_HIGHT/3;
  for (int i = 0; i < SCREEN_LENGHT; i++) {
    pixel = {i,pos_size};
    pixel.DrawPixel(color);
    pixel = {i, SCREEN_HIGHT - 1 - pos_size};
    pixel.DrawPixel(color);
  }
  
  UpdateMatrix();
}

// матрица, в которой сохраняются занятые позиции
uint32_t positions[9] = {0};

uint32_t cur_color = COLOR1;

// флаг того, что игра начата
bool is_start = false;

class TicTacToeMovements : public GameMovements {
  protected:
    void MoveLeft() {
      do {
        cur_pos.x--;
          if (cur_pos.x == -1)
           cur_pos.x = 2;
        } while ((positions[3*cur_pos.x+cur_pos.y] != 0)&&(positions[3*cur_pos.x+((cur_pos.y+1)%3)] != 0)&&(positions[3*cur_pos.x+((cur_pos.y+2)%3)] != 0));
           if (positions[3*cur_pos.x+cur_pos.y] == 0)
         return;
        if (positions[3*cur_pos.x+((cur_pos.y+1)%3)] == 0) {
         cur_pos.y = (cur_pos.y+1)%3;
          return;
        }
        if (positions[3*cur_pos.x+((cur_pos.y+2)%3)] == 0) {
          cur_pos.y = (cur_pos.y+2)%3;
          return;
      }
    }

    void MoveRight() {
      do {
          cur_pos.x++;
         if (cur_pos.x == 3)
           cur_pos.x = 0;

         #ifdef DEBUG
         Serial.print("PlayerMove: ");
         Serial.print(cur_pos.x);
         Serial.print(' ');
          Serial.print(positions[3*cur_pos.x+cur_pos.y]);
         Serial.print('\n');
         #endif

       } while ((positions[3*cur_pos.x+cur_pos.y] != 0)&&(positions[3*cur_pos.x+((cur_pos.y+1)%3)] != 0)&&(positions[3*cur_pos.x+((cur_pos.y+2)%3)] != 0));
        if (positions[3*cur_pos.x+cur_pos.y] == 0)
         return;
        if (positions[3*cur_pos.x+((cur_pos.y+1)%3)] == 0) {
          cur_pos.y = (cur_pos.y+1)%3;
          return;
        }
        if (positions[3*cur_pos.x+((cur_pos.y+2)%3)] == 0) {
          cur_pos.y = (cur_pos.y+2)%3;
          return;
        }
    }

    void MoveDown() {
      do {
          cur_pos.y++;
          if (cur_pos.y == 3)
            cur_pos.y = 0;
        } while ((positions[3*cur_pos.x+cur_pos.y] != 0)&&(positions[3*((cur_pos.x+1)%3)+cur_pos.y] != 0)&&(positions[3*((cur_pos.x+2)%3)+cur_pos.y] != 0));
        if (positions[3*cur_pos.x+cur_pos.y] == 0)
          return;
        if (positions[3*((cur_pos.x+1)%3)+cur_pos.y] == 0) {
          cur_pos.x = (cur_pos.x+1)%3;
          return;
        }
        if (positions[3*((cur_pos.x+2)%3)+cur_pos.y] == 0) {
          cur_pos.x = (cur_pos.x+2)%3;
          return;
        }
    }

    void MoveUp() {
      do {
          cur_pos.y--;
          if (cur_pos.y == -1)
            cur_pos.y = 2;
        } while ((positions[3*cur_pos.x+cur_pos.y] != 0)&&(positions[3*((cur_pos.x+1)%3)+cur_pos.y] != 0)&&(positions[3*((cur_pos.x+2)%3)+cur_pos.y] != 0));
        if (positions[3*cur_pos.x+cur_pos.y] == 0)
          return;
        if (positions[3*((cur_pos.x+1)%3)+cur_pos.y] == 0) {
          cur_pos.x = (cur_pos.x+1)%3;
          return;
        }
        if (positions[3*((cur_pos.x+2)%3)+cur_pos.y] == 0) {
          cur_pos.x = (cur_pos.x+2)%3;
          return;
        }
    }
};

TicTacToeMovements player_pos;

void Reset () {
  is_start = false;
  player_pos.cur_pos = {0,0};
  cur_color = COLOR1;
  for (int i = 0; i < 9; i++)
    positions[i] = 0;
}

// определяет кто выиграл (если есть такой)
uint32_t WhoWins(uint32_t* board) {
  #ifdef DEBUG
  Serial.print("WhoWins: ");
  for (int i = 0; i < 9; i++) {
    Serial.print(board[i]);
    Serial.print(' ');
  }
  Serial.print('\n');
  #endif
  for (int i = 0; i < 3; i++)
    if ((board[3*i] == board[3*i+1])&&(board[3*i+1] == board[3*i+2])&&(board[3*i] != BLACK))
      return board[3*i];
  for (int i = 0; i < 3; i++)
    if ((board[i] == board[3+i])&&(board[3+i] == board[6+i])&&(board[i] != BLACK))
      return board[i];
  if ((board[0] == board[4])&&(board[4] == board[8])&&(board[0] != BLACK))
    return board[0];
  if ((board[2] == board[4])&&(board[4] == board[6])&&(board[2] != BLACK))
    return board[2];
  return 0;
}

// определяет, есть ли выигрышь
bool IsWin() {
  uint32_t who_wins = WhoWins(positions);
  if (!who_wins)
    return false;
  Reset();
  if (who_wins == COLOR1)
    PlayTune(Player1WinTune, PLAYER1WINTUNEDUR);
  else
    PlayTune(Player2WinTune, PLAYER2WINTUNEDUR);
  for (int i = 0; i < 3; i++) {
    ScreenFiller(who_wins);
    delay(DELAY);
    ScreenFiller(BLACK);
    delay(DELAY);
  }
  return true;
}

// определяет, есть ли ничья
bool Tie () {
  int filled_pos = 0;
  for (int i = 0; i < 9; i++)
    if (positions[i] != 0)
        filled_pos++;
  if (filled_pos != 9)
    return false;
  Reset();
  PlayTune(TieTune, TIETUNEDUR);
  for (int i = 0; i < 3; i++) {
    ScreenFiller(WHITE);
    delay(DELAY);
    ScreenFiller(BLACK);
    delay(DELAY);
  }
  return true;
}

// закрашивание позиции
void ColorPos (ScreenCoords pos, uint32_t color) {
  ScreenCoords pixel;
  for (int i = pos.x * (pos_size + 1); i < pos.x * (pos_size + 1) + pos_size; i++)
    for (int j = pos.y * (pos_size + 1); j < pos.y * (pos_size + 1) + pos_size; j++) {
      pixel = {i,j};
      pixel.DrawPixel(color);
    }
  UpdateMatrix();
}

// мигание текущей позиции
void PosSignal() {
  #ifdef DEBUG
  Serial.print("PosSignal: ");
  Serial.print(player_pos.cur_pos.x);
  Serial.print(' ');
  Serial.print(player_pos.cur_pos.y);
  Serial.print('\n');
  #endif
  ColorPos(player_pos.cur_pos, cur_color);
  delay(DELAY);
  ColorPos(player_pos.cur_pos, BLACK);
  delay(DELAY);
}

// устанавливает позицию
void PosSet() {
  positions[3*player_pos.cur_pos.x+player_pos.cur_pos.y] = cur_color;
    PlayTune(PutTune, PUTTUNEDUR);
    ColorPos(player_pos.cur_pos, cur_color);
    bool game_ended = IsWin();
    int i = 0;
    int j = 0;
    while (positions[3*i+j] != 0) {
      j++;
      if (j == 3) {
        i++;
        j = 0;
      }
      if (i == 3)
        if (!game_ended)
          game_ended = Tie();
    }
    if (!game_ended) {
       player_pos.cur_pos = {i,j};
      if (cur_color == COLOR1)
        cur_color = COLOR2;
      else
        cur_color = COLOR1;
    }
}

// функция, просчитывающая следущий ход ИИ по алгоритму Минимакс
int AI_Move(uint32_t* board, uint32_t player, bool is_begin, uint32_t human, uint32_t ai) {

  #ifdef DEBUG
  Serial.print("board: ");
  for (int i = 0; i < 9; i++) {
    Serial.print(board[i]);
    Serial.print(' ');
  }
  Serial.print('\n');
  #endif

  uint32_t who_wins = WhoWins(board);
  #ifdef DEBUG
  Serial.print("who_wins: ");
  Serial.print(who_wins);
  Serial.print("human: ");
  Serial.print(human);
  Serial.print("ai: ");
  Serial.print(ai);
  Serial.print('\n');
  #endif
  if (who_wins == human)
    return -1;
  if (who_wins == ai)
    return 1;

  int avaliable_pos = 9;

  int posible_moves_score[9];
  for (int i = 0; i < 9; i++)
    posible_moves_score[i] = -2;

  uint32_t new_player = human;
    if (player == human)
      new_player = ai;
  
  for (int i = 0; i < 9; i++) {
    
    if (board[i] != 0) {
      avaliable_pos--;
      continue;
    }

    uint32_t new_board[9] = {0};
    for (int j = 0; j < 9; j++)
      new_board[j] = board[j];
    new_board[i] = player;

    posible_moves_score[i] = AI_Move(new_board, new_player, false, human, ai);
  }

  #ifdef DEBUG
  Serial.print("posible_moves_score: ");
  for (int i = 0; i < 9; i++) {
    Serial.print(posible_moves_score[i]);
    Serial.print(' ');
  }
  Serial.print('\n');
  #endif

  // если заняты все позиции, то ничья, значит не даём очки
  if (!avaliable_pos)
    return 0;

  int best_score = 0;
  int best_move = 0;
  if (player == ai) {
    best_score = -2;
    for (int i = 0; i < 9; i++) {
      if (best_score <  posible_moves_score[i]) {
        best_score = posible_moves_score[i];
        best_move = i;
      }
    }
  }
  else {
    best_score = 2;
    for (int i = 0; i < 9; i++) {
      if ((best_score > posible_moves_score[i])&&(posible_moves_score[i]!=-2)) {
        best_score = posible_moves_score[i];
        best_move = i;
      }
    }
  }

  #ifdef DEBUG
  Serial.print("best_score: ");
  Serial.print(best_score);
  Serial.print(" best_move: ");
  Serial.print(best_move);
  Serial.print('\n');
  #endif
  
  if (!is_begin)
    return best_score;
  player_pos.cur_pos.x = best_move/3;
  player_pos.cur_pos.y = best_move%3;
  return 0;
}

void MenuDraw (int menu_pos) {
  ScreenFiller(BLACK);
  switch (menu_pos) {
    case 0:
      Net(WHITE);
      break;
    case 1:
      Net(GREEN);
      ColorPos ( {0,0}, COLOR1);
      break;
    case 2:
      Net(GREEN);
      ColorPos ( {0,0}, COLOR2);
      break;
    default:
      break;
  }
  while (JoystickHandler()!=not_touched) {}
}

void GameActivate (int menu_pos, bool* ai_enabled, uint32_t* ai_color, uint32_t* human_color) {
  ScreenFiller(BLACK);
  is_start = true;
  PlayTune(StartTune, STARTTUNEDUR);
  switch (menu_pos) {
    case 0:
      Net(WHITE);
      break;
    case 1:
      Net(GREEN);
      *ai_enabled = true;
      *ai_color = COLOR2;
      *human_color = COLOR1;
      break;
    case 2:
      Net(GREEN);
      *ai_enabled = true;
      *ai_color = COLOR1;
      *human_color = COLOR2;
      player_pos.cur_pos.x = 1;
      player_pos.cur_pos.y = 1;
      PosSet();
      break;
    default:
      break;
  }
 while (digitalRead(Z)) {}
}

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  HardwareInit();
  ScreenFiller(BLACK);
  Net(WHITE);
  player_pos.cur_pos = {0,0};
}

void loop() {
  
  #ifdef DEBUG
  Serial.print("loop: ");
  Serial.print(cur_color);
  Serial.print('\n');
  #endif
  
  static bool ai_enabled = false;
  static uint32_t ai_color = 0;
  static uint32_t human_color = 0;
  static int menu_pos = 0;
  // меню
  while (!is_start) {
    switch (JoystickHandler()) {
      case left:
        menu_pos--;
        if (menu_pos < 0)
          menu_pos = 2;
        MenuDraw(menu_pos);
        break;
      case right:
        menu_pos = (menu_pos + 1)%3;
        MenuDraw(menu_pos);
        break;
      case clicked:
        GameActivate(menu_pos, &ai_enabled, &ai_color, &human_color);
        break;
      default:
        break;
    }
  }
    
  if (player_pos.PlayerMove() == clicked) {
    PosSet();
    if (ai_enabled&&is_start) {
      #ifdef DEBUG
      Serial.print("ai_enabled\n");
      #endif
      AI_Move(positions, ai_color, true, human_color, ai_color);
      PosSet();
    }
  }

  if (is_start)
    PosSignal();
  else {
    ai_enabled = false;
    ai_color = 0;
    human_color = 0;
    MenuDraw(menu_pos);
  }
}
