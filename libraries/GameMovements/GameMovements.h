#include "hardware.h"
#include "ScreenCoords.h"

#ifndef __GAMEMOVEMENTS__
#define __GAMEMOVEMENTS__

class GameMovements {
	public:
		ScreenCoords cur_pos;
		// метод вызвывает соответствующие методы движения при отклонении джойстика, или возвращает clicked при нажатии на него, или held при удержании
		joystick_pos PlayerMove();
	protected:
		virtual void MoveLeft();
		virtual void MoveRight();
		virtual void MoveUp();
		virtual void MoveDown();
};

#endif