#include "game_os.h"
static void ((*games_cycles[GAMES_QUANTITY]))() = {TicTacToe, Mineswapper};
static void ((*games_setups[GAMES_QUANTITY]))() = {TicTacToeSetup, MineswapperSetup};
static void ((*games_icons[GAMES_QUANTITY]))() ={TicTacToeIcon, MineswapperIcon};

static struct Note OSWelcome[] = { {NOTE_E3 + NOTE_B3, 100}, {NOTE_B3 + NOTE_E4, 100}, {0, 100}, {NOTE_B3 + NOTE_FS4, 100}, 
                            {NOTE_FS2 + NOTE_A2 + NOTE_CS3 + NOTE_E3 + NOTE_AS3 + NOTE_B3 + NOTE_E4 + NOTE_AS4 + NOTE_B4 + NOTE_E5, 100},
                            {NOTE_FS2 + NOTE_A2 + NOTE_CS3 + NOTE_E3 + NOTE_AS3 + NOTE_B3 + NOTE_AS4 + NOTE_B4 + NOTE_E5, 100}
                          };
struct Note ClickInMenuSound[] = {{NOTE_D4, 125}};

bool is_start = false;

static void DrawSoundOnIcon() {
  ScreenFiller(BLACK);
  ScreenCoords pixel = {0, 3};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {0, 4};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {1, 2};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {1, 3};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {1, 4};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {1, 5};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {3, 2};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {3, 5};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {4, 3};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {4, 4};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {4, 0};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {4, 7};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {5, 1};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {5, 6};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {6, 2};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {6, 3};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {6, 4};
  pixel.DrawPixel(SOUNDICONCOLOR);
  pixel = {6, 5};
  pixel.DrawPixel(SOUNDICONCOLOR);
  UpdateMatrix();
}

static void DrawSoundOffIcon() {
  DrawSoundOnIcon();
  unsigned min_screen_size = min(SCREEN_HIGHT, SCREEN_LENGHT);
  ScreenCoords pixel;
  for (unsigned i = 0; i < min_screen_size; i++) {
    pixel = {(int)i, (int)i};
    pixel.DrawPixel(SOUNDICONCOLOR);
  }
  UpdateMatrix();
}

static void MenuDraw(int menu_pos) {
  if (menu_pos < 0)
    return;
  if (menu_pos < GAMES_QUANTITY) {
    games_icons[menu_pos]();
    while (JoystickHandler()!=not_touched) {};
    return;
  }
  if (sound_on)
    DrawSoundOnIcon();
  else
    DrawSoundOffIcon();

  while (JoystickHandler()!=not_touched) {};
}

static void ClickInMenu(int menu_pos) {
  PlayTune(ClickInMenuSound, CLICKINMENUSOUNDDUR);
  if (menu_pos < 0)
    return;
  if (menu_pos < GAMES_QUANTITY) {
    is_start = true;
    games_setups[menu_pos]();
    while (is_start)
      games_cycles[menu_pos]();  
    games_icons[menu_pos]();
    return;
  }
  if (sound_on) {
    sound_on = false;
    DrawSoundOffIcon();
  }
  else {
    sound_on = true;
    DrawSoundOnIcon();
  }
}

void ExitSignal(uint32_t* bottom_edge_pixels) {
  ScreenCoords pixel;
  for (int i = 0; i < SCREEN_LENGHT; i++) {
    pixel = {i, SCREEN_HIGHT - 1};
    pixel.DrawPixel(EXITSIGNALCOLOR);
  }
  UpdateMatrix();
  delay(DELAY);
  for (int i = 0; i < SCREEN_LENGHT; i++) {
    pixel = {i, SCREEN_HIGHT - 1};
    pixel.DrawPixel(bottom_edge_pixels[i]);
  }
  UpdateMatrix();
  delay(DELAY);
}

void DrawExit() {
  ScreenCoords pixel;
  for (int i = 0; i < min(SCREEN_LENGHT, SCREEN_HIGHT); i++) {
    pixel = {i, i};
    pixel.DrawPixel(EXITSIGNALCOLOR);
    pixel = {(SCREEN_LENGHT - 1) * (i+1) % SCREEN_LENGHT, (SCREEN_LENGHT - 1) * (i+1) / SCREEN_LENGHT};
    pixel.DrawPixel(EXITSIGNALCOLOR);
  }
  UpdateMatrix();
}

static void DrawSoundIcon(int menu_pos) {
  switch (menu_pos) {
      case 0:
        DrawSoundOnIcon();
        break;
      case 1:
        DrawSoundOffIcon();
        break;
      default:
        break;
    }
    while (JoystickHandler()!=not_touched) {};
}

static void ChooseSoundMode() {
  DrawSoundOnIcon();
  int menu_pos = 0;
  while(true) {
    switch (JoystickHandler()) {
        case left:
          menu_pos--;
          if (menu_pos < 0)
            menu_pos = 1;
          DrawSoundIcon(menu_pos);
          break;
        case right:
          menu_pos = (menu_pos + 1) % 2;
          DrawSoundIcon(menu_pos);
          break;
        case clicked:
          switch (menu_pos) {
            case 0:
              sound_on = true;
              break;
            case 1:
              sound_on = false;
              break;
            default:
              break;
          }
          return;
          break;
        default:
          break;
      }
  }
}

static void OSWelcomeScreen() {
  ScreenFiller(BLACK);
  ScreenCoords pixel;
  for (int i = 0; i < SCREEN_LENGHT; i++) {
    pixel = {i, 0};
    pixel.DrawPixel(OSWELCOMECOLOR);
    pixel = {i, SCREEN_HIGHT -1};
    pixel.DrawPixel(OSWELCOMECOLOR);
  }
  for (int i = 0; i < SCREEN_HIGHT; i++) {
    pixel = {SCREEN_LENGHT / 2 - 1, i};
    pixel.DrawPixel(OSWELCOMECOLOR);
    pixel = {SCREEN_LENGHT / 2, i};
    pixel.DrawPixel(OSWELCOMECOLOR);
  }
  UpdateMatrix();
}

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  HardwareInit();
  ChooseSoundMode();
  OSWelcomeScreen();
  if (sound_on)
    PlayTune(OSWelcome, OSWELCOMEDUR);
  else
    delay(500);
  games_icons[0]();
}
void loop() {
  static int menu_pos = 0;
  switch (JoystickHandler()) {
    case left:
      menu_pos--;
      if (menu_pos < 0)
        menu_pos = GAMES_QUANTITY;
      MenuDraw(menu_pos);
      break;
    case right:
      menu_pos = (menu_pos + 1) % (GAMES_QUANTITY + 1);
      MenuDraw(menu_pos);
      break;
    case clicked:
      ClickInMenu(menu_pos);
      break;
    default:
      break;
    }
}