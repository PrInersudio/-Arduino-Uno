#include "mineswapper.h"
static struct Note MineswapperLose[] = { {NOTE_F2, 250}, {NOTE_G2, 250}, {NOTE_A2, 500}, {NOTE_G2, 250}, {NOTE_F2, 250} };
static struct Note MineswapperWon[] = { {NOTE_E4, 250}, {NOTE_F4, 250}, {NOTE_G4, 250}, {NOTE_A4, 500}, {NOTE_G4, 250}, {NOTE_F4, 250}, {NOTE_E4, 250}, {NOTE_D4, 500}};
static struct Note MineswapperMakeMoveSound[] = { {NOTE_CS4, 125} };
static struct Note MineswapperStartTune[] = {{NOTE_C4, 250}, {NOTE_CS4, 250}, {NOTE_D4, 250}, {NOTE_DS4, 250}, {NOTE_E4, 250}};

static uint32_t num_colors[] = {
                            EMPTY_COLOR,
                            ONE_BOMB,
                            TWO_BOMBS,
                            THREE_BOMBS,
                            FOUR_BOMBS,
                            FIVE_BOMBS,
                            SIX_BOMBS,
                            SEVEN_BOMBS,
                            EIGHT_BOMBS
                        };



static bool mineswapper_start = false;

static int bombs_num = BOMBS_NUM_EASY;

// имеет значение -1, если в клетке бомба, или значение количества бомб вокруг клетки, если клетка свободана
static int bombs_pos[SCREEN_LENGHT*SCREEN_HIGHT] = {0};

// имеет значение 1, если поле закрыто, 0, если открыто, -1 если на поле выставлен флаг
static int filled_fields[SCREEN_LENGHT*SCREEN_HIGHT];

static bool exit_signal = false;

class MineswapperMovements : public GameMovements {
  protected:
    void MoveLeft() {
      cur_pos.x -=1;
      if (cur_pos.x < 0)
        cur_pos.x = SCREEN_LENGHT + cur_pos.x;
    }
    void MoveRight() {
      cur_pos.x = (cur_pos.x + 1) % SCREEN_LENGHT;
    }
    void MoveUp() {
      cur_pos.y--;
      if (cur_pos.y < 0)
        cur_pos.y = SCREEN_HIGHT + cur_pos.y;
    }
    void MoveDown() {
      cur_pos.y++;
      if (cur_pos.y == SCREEN_HIGHT) {
        exit_signal = true;
        return;
      }
      if (cur_pos.y == SCREEN_HIGHT + 1) {
        cur_pos.y = 0;
        exit_signal = false;
        return;
      }
      cur_pos.y %= SCREEN_HIGHT;
    }
};

static MineswapperMovements player_pos;

enum field_pos {undefined_pos, not_touching_edges, left_edge, upper_edge, right_edge, bottom_edge, upper_left_corner, upper_right_corner, bottom_left_corner, bottom_right_corner};
//  00  0!0  !00  !0!0  M0  M!0  MM  !0M  0M
//      **   * *  ***   *   **   **  ***  **
//   *   *   ***  * *   **  *    *   * *   *
//  **  **        ***       **
static field_pos FieldPosCheck (ScreenCoords pos_for_checking) {
  
  // !0!0
  if (
    (pos_for_checking.x > 0) &&
    (pos_for_checking.x < SCREEN_LENGHT - 1) &&
    (pos_for_checking.y > 0) &&
    (pos_for_checking.y < SCREEN_HIGHT - 1)
  )
    return not_touching_edges;

  // 0!0
  if (
    (pos_for_checking.x == 0) &&
    (pos_for_checking.y > 0) &&
    (pos_for_checking.y < SCREEN_HIGHT - 1)
  )
    return left_edge;

  //!00
  if (
    (pos_for_checking.x > 0) &&
    (pos_for_checking.x < SCREEN_LENGHT - 1) &&
    (pos_for_checking.y == 0)
  )
    return upper_edge;

  //M!0
  if (
    (pos_for_checking.x == SCREEN_LENGHT - 1) &&
    (pos_for_checking.y > 0) &&
    (pos_for_checking.y < SCREEN_HIGHT - 1)
  )
    return right_edge;

  //!0M
  if (
    (pos_for_checking.x > 0) &&
    (pos_for_checking.x < SCREEN_LENGHT - 1) &&
    (pos_for_checking.y == SCREEN_HIGHT - 1)
  )
    return bottom_edge;

  // 00
  if ((pos_for_checking.x == 0) && (pos_for_checking.y == 0))
    return upper_left_corner;

  //M0
  if ((pos_for_checking.x == SCREEN_LENGHT - 1) && (pos_for_checking.y == 0))
    return upper_right_corner;

  //0M
  if ((pos_for_checking.x == 0) && (pos_for_checking.y == SCREEN_HIGHT - 1))
    return bottom_left_corner;

  // MM
  if ((pos_for_checking.x == SCREEN_LENGHT - 1) && (pos_for_checking.y == SCREEN_HIGHT - 1))
    return bottom_right_corner;

  return undefined_pos;
}

static int BombsCount(ScreenCoords start, ScreenCoords end, int *pos_arr) {
  int bombs_count = 0;
  for (int i = start.x; i <= end.x; i++)
    for (int j = start.y; j <= end.y; j++)
      if (pos_arr[j * SCREEN_LENGHT + i] == -1)
        bombs_count ++;
  return bombs_count;
}

static int BombsCounter(int pos, int* pos_arr) {
  ScreenCoords pos_for_checking = {pos%SCREEN_LENGHT, pos/SCREEN_LENGHT};
  // #ifdef DEBUG
  // Serial.print("BombsCounter: ");
  // Serial.print(pos_for_checking.x);
  // Serial.print(' ');
  // Serial.println(pos_for_checking.y);
  // #endif

  switch (FieldPosCheck(pos_for_checking)) {
  
    // !0!0
    case not_touching_edges:
      return BombsCount(
        {pos_for_checking.x - 1, pos_for_checking.y - 1},
        {pos_for_checking.x + 1, pos_for_checking.y + 1},
        pos_arr
      );
      break;

    // 0!0
    case left_edge:
      return BombsCount(
        {pos_for_checking.x, pos_for_checking.y - 1},
        {pos_for_checking.x + 1, pos_for_checking.y + 1},
        pos_arr
      );
      break;

    //!00
    case upper_edge:
      return BombsCount(
        {pos_for_checking.x - 1, pos_for_checking.y},
        {pos_for_checking.x + 1, pos_for_checking.y + 1},
        pos_arr
      );
      break;

    //M!0
    case right_edge:
      return BombsCount(
        {pos_for_checking.x - 1, pos_for_checking.y - 1},
        {pos_for_checking.x, pos_for_checking.y + 1},
        pos_arr
      );
      break;

    //!0M
    case bottom_edge:
      return BombsCount(
        {pos_for_checking.x - 1, pos_for_checking.y - 1},
        {pos_for_checking.x + 1, pos_for_checking.y},
        pos_arr
      );
      break;

    // 00
    case upper_left_corner:
      return BombsCount(
        {pos_for_checking.x, pos_for_checking.y},
        {pos_for_checking.x + 1, pos_for_checking.y + 1},
        pos_arr
      );
      break;

    //M0
    case upper_right_corner:
      return BombsCount(
        {pos_for_checking.x - 1, pos_for_checking.y},
        {pos_for_checking.x, pos_for_checking.y + 1},
        pos_arr
      );
      break;

    //0M
    case bottom_left_corner:
      return BombsCount(
        {pos_for_checking.x, pos_for_checking.y - 1},
        {pos_for_checking.x + 1, pos_for_checking.y},
        pos_arr
      );
      break;

    // MM
    case bottom_right_corner:
      return BombsCount(
        {pos_for_checking.x - 1, pos_for_checking.y - 1},
        {pos_for_checking.x, pos_for_checking.y},
        pos_arr
      );
      break;

    default:
      break;

    }

  return -2;
}

static void PlaceBombs() {
  // ининциализурем ГСЧ
  randomSeed(analogRead(ANALOG_PIN_FOR_RND));
  int i = 0;
  //выставляем бомбы
  while(i != bombs_num) {
    int new_bomb_pos = (int)random(0, SCREEN_LENGHT*SCREEN_HIGHT);
    if (bombs_pos[new_bomb_pos] != -1) {
      bombs_pos[new_bomb_pos] = -1;
      i++;
    }
  }

  //выставляем количество бомб в свободных клетках
  for (i = 0; i < SCREEN_LENGHT*SCREEN_HIGHT; i++) {
    if (bombs_pos[i] != -1) {
      bombs_pos[i] = BombsCounter(i, bombs_pos);
      if (bombs_pos[i] < 0) {
        #ifdef DEBUG
        Serial.print("Error when counting bombs around point ");
        Serial.println(i);
        #else
        exit(__LINE__);
        #endif
      }
    }
  }
}

static bool AllFieldsOpened() {
  int opened_fields_count = 0;
  for (int i = 0; i < SCREEN_HIGHT * SCREEN_LENGHT; i++)
    if (bombs_pos[i] == -1)
      opened_fields_count++;
    else if (filled_fields[i] == 0)
      opened_fields_count++;
  if (opened_fields_count == SCREEN_HIGHT * SCREEN_LENGHT)
    return true;
  return false;
}

#ifdef RECURSIVE_OPEN

static void MultiOpenField(ScreenCoords start, ScreenCoords end) {
  for (int i = start.x; i <= end.x; i++)
    for (int j = start.y; j <= end.y; j++) {
        if (bombs_pos[j*SCREEN_LENGHT+i] == -1) // доп проверка на случай ошибок
          continue;
        OpenField({i,j});
    }
}

static void OpenField(ScreenCoords field) {

  // #ifdef DEBUG
  // Serial.print("OpenField: ");
  // Serial.print(field.x);
  // Serial.print(' ');
  // Serial.print(field.y);
  // Serial.print(' ');
  // Serial.print(bombs_pos[field.y * SCREEN_LENGHT + field.x]);
  // Serial.print(' ');
  // Serial.println(filled_fields[field.y * SCREEN_LENGHT + field.x]);
  // #endif

  if (filled_fields[field.y * SCREEN_LENGHT + field.x] == 0)
    return;
  filled_fields[field.y * SCREEN_LENGHT + field.x] = 0;
  field.DrawPixel(num_colors[bombs_pos[field.y * SCREEN_LENGHT + field.x]]);
  if (bombs_pos[field.y * SCREEN_LENGHT + field.x] != 0)
    return;
  
  switch (FieldPosCheck(field)) {
  
    // !0!0
    case not_touching_edges:
      MultiOpenField(
        {field.x - 1, field.y - 1},
        {field.x + 1, field.y + 1}
      );
      break;

    // 0!0
    case left_edge:
      MultiOpenField(
        {field.x, field.y - 1},
        {field.x + 1, field.y + 1}
      );
      break;

    //!00
    case upper_edge:
      MultiOpenField(
        {field.x - 1, field.y},
        {field.x + 1, field.y + 1}
      );
      break;

    //M!0
    case right_edge:
      MultiOpenField(
        {field.x - 1, field.y - 1},
        {field.x, field.y + 1}
      );
      break;

    //!0M
    case bottom_edge:
      MultiOpenField(
        {field.x - 1, field.y - 1},
        {field.x + 1, field.y}
      );
      break;

    // 00
    case upper_left_corner:
      MultiOpenField(
        {field.x, field.y},
        {field.x + 1, field.y + 1}
      );
      break;

    //M0
    case upper_right_corner:
      MultiOpenField(
        {field.x - 1, field.y},
        {field.x, field.y + 1}
      );
      break;

    //0M
    case bottom_left_corner:
      MultiOpenField(
        {field.x, field.y - 1},
        {field.x + 1, field.y}
      );
      break;

    // MM
    case bottom_right_corner:
      MultiOpenField(
        {field.x - 1, field.y - 1},
        {field.x, field.y}
      );
      break;

    default:
      break;

    }
}

static void MultiMove(ScreenCoords start, ScreenCoords end) {
  for (int i = start.x; i <= end.x; i++)
    for (int j = start.y; j <= end.y; j++)
      if (filled_fields[j * SCREEN_LENGHT + i] == 1)
        if (MakeMove({i,j}, false))
          break;
}

static void MoveAround(ScreenCoords field) {
  int num_of_flags = BombsCounter(field.y * SCREEN_LENGHT + field.x, filled_fields);
  if (num_of_flags != bombs_pos[field.y * SCREEN_LENGHT + field.x])
    return;
  
  switch (FieldPosCheck(field)) {
  
    // !0!0
    case not_touching_edges:
      MultiMove(
        {field.x - 1, field.y - 1},
        {field.x + 1, field.y + 1}
      );
      break;

    // 0!0
    case left_edge:
      MultiMove(
        {field.x, field.y - 1},
        {field.x + 1, field.y + 1}
      );
      break;

    //!00
    case upper_edge:
      MultiMove(
        {field.x - 1, field.y},
        {field.x + 1, field.y + 1}
      );
      break;

    //M!0
    case right_edge:
      MultiMove(
        {field.x - 1, field.y - 1},
        {field.x, field.y + 1}
      );
      break;

    //!0M
    case bottom_edge:
      MultiMove(
        {field.x - 1, field.y - 1},
        {field.x + 1, field.y}
      );
      break;

    // 00
    case upper_left_corner:
      MultiMove(
        {field.x, field.y},
        {field.x + 1, field.y + 1}
      );
      break;

    //M0
    case upper_right_corner:
      MultiMove(
        {field.x - 1, field.y},
        {field.x, field.y + 1}
      );
      break;

    //0M
    case bottom_left_corner:
      MultiMove(
        {field.x, field.y - 1},
        {field.x + 1, field.y}
      );
      break;

    // MM
    case bottom_right_corner:
      MultiMove(
        {field.x - 1, field.y - 1},
        {field.x, field.y}
      );
      break;

    default:
      break;

    }
}

#else

class NeighborsInfo {
  
  private:
    unsigned* neighbors_indexes;
    unsigned neighbors_quantity;
    size_t allocated_memory;
  
  public:

    NeighborsInfo(ScreenCoords field) {
      neighbors_quantity = 0;

      switch (FieldPosCheck(field)) {
        case not_touching_edges:
          allocated_memory = 8 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + field.x);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + (field.x + 1));
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x + 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + field.x);
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + (field.x + 1));
          break;
        case left_edge:
          allocated_memory = 5 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + field.x);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + (field.x + 1));
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x + 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + field.x);
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + (field.x + 1));
          break;
        case upper_edge:
          allocated_memory = 5 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x + 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + field.x);
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + (field.x + 1));
          break;
        case right_edge:
          allocated_memory = 5 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + field.x);
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + field.x);
          break;
        case bottom_edge:
          allocated_memory = 5 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + field.x);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + (field.x + 1));
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x + 1));
          break;
        case upper_left_corner:
          allocated_memory = 3 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x + 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + field.x);
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + (field.x + 1));
          break;
        case upper_right_corner:
          allocated_memory = 3 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y + 1) * SCREEN_LENGHT + field.x);
          break;
        case bottom_left_corner:
          allocated_memory = 3 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + field.x);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + (field.x + 1));
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x + 1));
          break;
        case bottom_right_corner:
          allocated_memory = 3 * sizeof(unsigned);
          neighbors_indexes = (unsigned*)malloc(allocated_memory);
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + (field.x - 1));
          pushNeighbor((field.y - 1) * SCREEN_LENGHT + field.x);
          pushNeighbor(field.y * SCREEN_LENGHT + (field.x - 1));
          break;
        default:
          break;
      }
    }

    unsigned popNeighbor() {
      if (neighbors_quantity == 0) {
        #ifdef DEBUG
        Serial.println("popNeighbor error: all neighbors have already poped");
        #endif
        return 0;
      }
      neighbors_quantity--;
      unsigned poped_neighbor = neighbors_indexes[neighbors_quantity];
      neighbors_indexes[neighbors_quantity] = 0;
      return poped_neighbor;
    }

    void pushNeighbor(unsigned new_neighbor) {
      new_neighbor %= (SCREEN_LENGHT * SCREEN_HIGHT); // защита на случай ошибки
      neighbors_quantity++;
      if (allocated_memory > neighbors_quantity * sizeof(unsigned)) {
        neighbors_indexes[neighbors_quantity - 1] = new_neighbor;
        return;
      }
      allocated_memory *= 2;
      neighbors_indexes = (unsigned*)realloc(neighbors_indexes, allocated_memory);
      neighbors_indexes[neighbors_quantity - 1] = new_neighbor;
    }

    unsigned getNeighborsQauntity() {
      return neighbors_quantity;
    }

    bool HaveNeighbor(unsigned new_neighbor) {
      for (unsigned i = 0; i < neighbors_quantity; i++)
       if (new_neighbor == neighbors_indexes[i])
        return true;
      return false;
   }
   
    void addNeighbors(NeighborsInfo* new_neighbors) {
      while (new_neighbors->getNeighborsQauntity() > 0) {
        unsigned next_neighbor = new_neighbors->popNeighbor();
        if (filled_fields[next_neighbor] == 1)
          if (!HaveNeighbor(next_neighbor))
            pushNeighbor(next_neighbor);
      }
    }

    #ifdef DEBUG
    void printNeighbors() {
      Serial.print("printNeighbors: ");
      for (unsigned i = 0; i < neighbors_quantity; i++) {
        Serial.print(neighbors_indexes[i]);
        Serial.print(' ');
      }
      Serial.print('\n');
    }
    #endif

    ~NeighborsInfo() {
      allocated_memory = 0;
      neighbors_quantity = 0;
      free(neighbors_indexes);
      neighbors_indexes = NULL;
    }
};

static void OpenField (ScreenCoords field) {
  unsigned field_index = field.y * SCREEN_LENGHT + field.x;
  field_index %= (SCREEN_LENGHT * SCREEN_HIGHT); // чтобы не вылезти за пределы массива в случае ошибки
  filled_fields[field_index] = 0;
  field.DrawPixel(num_colors[bombs_pos[field_index]]);
  if (bombs_pos[field_index] != 0)
    return;

  #ifdef DEBUG
  Serial.println("OpenField stared multiopening");
  #endif

  NeighborsInfo neighbors = NeighborsInfo(field);
  #ifdef DEBUG
  Serial.print("OpenField NeighborsQauntity before cycle: ");
  Serial.println(neighbors.getNeighborsQauntity());
  neighbors.printNeighbors();
  #endif
  while (neighbors.getNeighborsQauntity() > 0) {
    unsigned next_neighbor = neighbors.popNeighbor();
    filled_fields[next_neighbor] = 0;
    field = {(int)next_neighbor % SCREEN_LENGHT, (int)next_neighbor / SCREEN_LENGHT};
    field.DrawPixel(num_colors[bombs_pos[next_neighbor]]);
    #ifdef DEBUG
    Serial.print("OpenField NeighborsQauntity and next_neighbor: ");
    Serial.print(neighbors.getNeighborsQauntity());
    Serial.print(' ');
    Serial.println(next_neighbor);
    #endif
    if (bombs_pos[next_neighbor] == 0) {
      NeighborsInfo new_neighbors = NeighborsInfo(field);
      neighbors.addNeighbors(&new_neighbors);
    }
  }
}

static bool MakeMove(ScreenCoords field, bool maid_by_player);

static void MoveAround(ScreenCoords field) {
  int num_of_flags = BombsCounter(field.y * SCREEN_LENGHT + field.x, filled_fields);
  if (num_of_flags != bombs_pos[field.y * SCREEN_LENGHT + field.x])
    return;
  NeighborsInfo neighbors = NeighborsInfo(field);
  while (neighbors.getNeighborsQauntity() > 0) {
    unsigned next_neighbor = neighbors.popNeighbor();
    if (MakeMove({(int)next_neighbor % SCREEN_LENGHT, (int)next_neighbor / SCREEN_LENGHT}, false))
      break;
  }
} 

#endif

static void GameEnded() {
  for (int i = 0; i < SCREEN_LENGHT * SCREEN_HIGHT; i++)
    if(bombs_pos[i] == -1) {
      ScreenCoords field = {i%SCREEN_LENGHT, i/SCREEN_LENGHT};
      field.DrawPixel(BOMB_COLOR);
    }
  UpdateMatrix();
  while(JoystickHandler() != clicked) {}
  mineswapper_start = false;
}

static void UpdateField() {
  UpdateMatrix();
  if (AllFieldsOpened()) {
    PlayTune(MineswapperWon, MINESWAPPERWON_DUR);
    GameEnded();
  }
}

// возвращает true, если игра закончена, false иначе
static bool MakeMove(ScreenCoords field, bool maid_by_player) {
  #ifdef DEBUG
  Serial.print("MakeMove field: ");
  Serial.print(field.x);
  Serial.print(' ');
  Serial.println(field.y);
  #endif
  if (filled_fields[field.y * SCREEN_LENGHT + field.x] == -1)
    return false;
  if (bombs_pos[field.y * SCREEN_LENGHT + field.x] == -1) {
    PlayTune(MineswapperLose, MINESWAPPERLOSE_DUR);
    GameEnded();
    return true;
  }
  if (filled_fields[field.y * SCREEN_LENGHT + field.x] == 0) {
    if (maid_by_player) {
      MoveAround(field);
      UpdateField();
    }
    return false;
  }
  OpenField(field);

  // #ifdef DEBUG
  // Serial.println("MakeMove:");
  // for (int i = 0; i < SCREEN_HIGHT; i++) {
  //   for (int j = 0; j < SCREEN_LENGHT; j++) {
  //     Serial.print(filled_fields[i * SCREEN_LENGHT + j]);
  //     Serial.print(' ');
  //   }
  //   Serial.print('\n');
  // }
  // #endif

  if (maid_by_player)
    UpdateField();
  return false;
}

static void PutFlag() {
  if (filled_fields[player_pos.cur_pos.y * SCREEN_LENGHT + player_pos.cur_pos.x] == 1) {
    filled_fields[player_pos.cur_pos.y * SCREEN_LENGHT + player_pos.cur_pos.x] = -1;
    player_pos.cur_pos.DrawPixel(FLAG_COLOR);
    UpdateMatrix();
  }
  else if (filled_fields[player_pos.cur_pos.y * SCREEN_LENGHT + player_pos.cur_pos.x] == -1) {
    filled_fields[player_pos.cur_pos.y * SCREEN_LENGHT + player_pos.cur_pos.x] = 1;
    player_pos.cur_pos.DrawPixel(CLOSED_FIELD);
    UpdateMatrix();
  }
  while (JoystickHandler() != not_touched) {}
}

static void PosSignal() {
  uint32_t cur_pos_color;
  uint32_t signal_color;
  if (filled_fields[player_pos.cur_pos.y * SCREEN_LENGHT + player_pos.cur_pos.x] == 1) {
    cur_pos_color = CLOSED_FIELD;
    signal_color = EMPTY_COLOR;
  }
  else if (filled_fields[player_pos.cur_pos.y * SCREEN_LENGHT + player_pos.cur_pos.x] == -1) {
    cur_pos_color = FLAG_COLOR;
    signal_color = CLOSED_FIELD;
  }
  else if (bombs_pos[player_pos.cur_pos.y * SCREEN_LENGHT + player_pos.cur_pos.x] == 0) {
    cur_pos_color = EMPTY_COLOR;
    signal_color = CLOSED_FIELD;
  }
  else {
    cur_pos_color = num_colors[bombs_pos[player_pos.cur_pos.y * SCREEN_LENGHT + player_pos.cur_pos.x]];
    signal_color = EMPTY_COLOR;
  }

  player_pos.cur_pos.DrawPixel(signal_color);
  UpdateMatrix();
  delay(DELAY);
  player_pos.cur_pos.DrawPixel(cur_pos_color);
  UpdateMatrix();
  delay(DELAY);
}

static void DrawEasy() {
  ScreenCoords pixel;
  for (int i = 0; i < SCREEN_LENGHT; i++) {
    pixel = {i, 0};
    pixel.DrawPixel(MENU_COLOR);
    pixel.y = SCREEN_HIGHT/2;
    pixel.DrawPixel(MENU_COLOR);
    pixel.y = SCREEN_HIGHT - 1;
    pixel.DrawPixel(MENU_COLOR);    
  }
  pixel.x = 0;
  for (int i = 0; i < SCREEN_HIGHT; i++) {
    pixel.y = i;
    pixel.DrawPixel(MENU_COLOR);
  }
}

static void DrawMiddle() {
  ScreenCoords pixel;
  for (int i = 0; i < SCREEN_HIGHT; i++) {
    pixel = {0, i};
    pixel.DrawPixel(MENU_COLOR);
    pixel.x = SCREEN_LENGHT - 1;
    pixel.DrawPixel(MENU_COLOR);
  }
  for (int i = 0; i < SCREEN_LENGHT/2; i++) {
      pixel = {i, i};
      pixel.DrawPixel(MENU_COLOR);
      pixel.x = SCREEN_LENGHT - 1 - i;
      pixel.DrawPixel(MENU_COLOR);
  }
}

static void DrawHard() {
  ScreenCoords pixel;
  for (int i = 0; i < SCREEN_HIGHT; i++) {
    pixel = {0, i};
    pixel.DrawPixel(MENU_COLOR);
    pixel.x = SCREEN_LENGHT - 1;
    pixel.DrawPixel(MENU_COLOR);
  }
  pixel.y = SCREEN_HIGHT/2;
  for (int i = 0; i < SCREEN_LENGHT; i++) {
    pixel.x = i;
    pixel.DrawPixel(MENU_COLOR);
  }
}

static void MenuDraw (int menu_pos) {
  ScreenFiller(CLOSED_FIELD);
  switch (menu_pos) {
    case 0:
      DrawEasy();
      break;
    case 1:
      DrawMiddle();
      break;
    case 2:
      DrawHard();
      break;
    case 3:
      DrawExit();
      break;
    default:
      break;
  }
  UpdateMatrix();
  while (JoystickHandler() != not_touched) {}
}

static void QuitGame() {
  PlayTune(ClickInMenuSound, CLICKINMENUSOUNDDUR);
  mineswapper_start = false;
  bombs_num = BOMBS_NUM_EASY;
  for (int i = 0; i < SCREEN_LENGHT*SCREEN_HIGHT; i++) {
    bombs_pos[i] = 0;
    filled_fields[i] = 0;
  }
  exit_signal = false;

}

static void ExitGame(int* menu_pos) {
  QuitGame();
  *menu_pos = 0;
  is_start = false;
}

static void GameActivate(int* menu_pos) {

  switch (*menu_pos) {
    case 0:
      PlayTune(MineswapperStartTune, MINESWAPPERSTARTTUNEDUR);
      bombs_num = BOMBS_NUM_EASY;
      break;
    case 1:
      PlayTune(MineswapperStartTune, MINESWAPPERSTARTTUNEDUR);
      bombs_num = BOMBS_NUM_MIDDLE;
      break;
    case 2:
      PlayTune(MineswapperStartTune, MINESWAPPERSTARTTUNEDUR);
      bombs_num = BOMBS_NUM_HARD;
      break;
    case 3:
      ExitGame(menu_pos);
      return;
      break;
    default:
      break;
  }

  ScreenFiller(CLOSED_FIELD);
  for (int i = 0; i < SCREEN_LENGHT * SCREEN_HIGHT; i++) {
    bombs_pos[i] = 0;
    filled_fields[i] = 1;
  }
  player_pos.cur_pos = {SCREEN_LENGHT/2,SCREEN_HIGHT/2};
  #ifndef STRESS_TEST
  PlaceBombs();
  #endif
  #ifdef DEBUG
  for (int i = 0; i < SCREEN_HIGHT; i++) {
    for (int j = 0; j < SCREEN_LENGHT; j++) {
      Serial.print(bombs_pos[i * SCREEN_LENGHT + j]);
      Serial.print(' ');
    }
    Serial.print('\n');
  }
  #endif
  mineswapper_start = true;
}

static uint32_t* BottomEdgeCopy() {
  uint32_t* bottom_edge = (uint32_t*)malloc(SCREEN_LENGHT * sizeof(uint32_t));
  for (int i = 0; i < SCREEN_LENGHT; i++)
    if (filled_fields[SCREEN_LENGHT * (SCREEN_HIGHT - 1) + i] == 1)
      bottom_edge[i] = CLOSED_FIELD;
    else if (filled_fields[SCREEN_LENGHT * (SCREEN_HIGHT - 1) + i] == -1)
      bottom_edge[i] = FLAG_COLOR;
    else
      bottom_edge[i]  = num_colors[bombs_pos[SCREEN_LENGHT * (SCREEN_HIGHT - 1) + i]];
  return bottom_edge;
}

void Mineswapper() {

  static int menu_pos = 0;
  while(!mineswapper_start) {
      switch (JoystickHandler()) {
        case left:
          menu_pos--;
          if (menu_pos < 0)
            menu_pos = 3;
          MenuDraw(menu_pos);
          break;
        case right:
          menu_pos = (menu_pos + 1)%4;
          MenuDraw(menu_pos);
          break;
        case clicked:
          GameActivate(&menu_pos);
          break;
        default:
          break;
      }
      if (!is_start) return;
  }

  joystick_pos is_clicked_or_held = player_pos.PlayerMove();
  if (is_clicked_or_held == clicked) {
    if (exit_signal) {
      QuitGame();
      MenuDraw(menu_pos);
      return;
    }
    PlayTune(MineswapperMakeMoveSound, MINESWAPPERMAKEMOVESOUNDDUR);
    delay(10); // чтобы звук нажатия и звук выигрыша/проигрыша не сливались
    MakeMove(player_pos.cur_pos, true);
  }
  else if ((is_clicked_or_held == held) && (!exit_signal)) {
    PlayTune(MineswapperMakeMoveSound, MINESWAPPERMAKEMOVESOUNDDUR);
    PutFlag();
  }
  
  if (exit_signal) {
    uint32_t* bottom_edge = BottomEdgeCopy();
    ExitSignal(bottom_edge);
    free(bottom_edge);
    return;
  }
  
  if (mineswapper_start)
    PosSignal();
  else
    MenuDraw(menu_pos);
}

void MineswapperSetup() {
  MenuDraw(0);
  // для теста цветов
  // ScreenCoords pixel;
  // ScreenFiller(EMPTY_COLOR);
  // for (int i = 0; i < 9; i++) {
  //   pixel = {i%8, i/8};
  //   pixel.DrawPixel(num_colors[i]);
  // }
  // pixel = {1, 1};
  // pixel.DrawPixel(FLAG_COLOR);
  // pixel = {2, 1};
  // pixel.DrawPixel(BOMB_COLOR);
  // UpdateMatrix();
}

void MineswapperIcon() {
  ScreenFiller(BLACK);
  PlaceBombs();
  ScreenCoords pixel;
  for (int i = 0; i < (SCREEN_LENGHT * SCREEN_HIGHT) / 2; i++) {
    pixel = {i % SCREEN_LENGHT, i / SCREEN_HIGHT};
    if (bombs_pos[i] == -1)
      pixel.DrawPixel(FLAG_COLOR);
    else
      pixel.DrawPixel(num_colors[bombs_pos[i]]);
  }
  for (int i = (SCREEN_LENGHT * SCREEN_HIGHT) / 2; i < SCREEN_LENGHT * SCREEN_HIGHT; i++) {
    pixel = {i % SCREEN_LENGHT, i / SCREEN_HIGHT};
    pixel.DrawPixel(CLOSED_FIELD);
  }
  for (int i = 0; i < SCREEN_LENGHT * SCREEN_HIGHT; i++)
    bombs_pos[i] = 0;
  UpdateMatrix();
}