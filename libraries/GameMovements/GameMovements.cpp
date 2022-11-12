#include "GameMovements.h"

// метод вызвывает соответствующие методы движения при отклонении джостика, или возвращает clicked при нажатии на него, или held при удержании
joystick_pos GameMovements::PlayerMove() {
	switch(JoystickHandler()) {
    case left:
      MoveLeft();
      break;
    case right:
      MoveRight();
      break;
    case up:
      MoveUp();
      break;
    case down:
      MoveDown();
      break;
    case clicked:
      return clicked;
      break;
	case held:
	  return held;
	  break;
    default:
      break;
  }
  return not_touched;
}