#ifndef __SCREENCOORDS_H__
#define __SCREENCOORDS_H__

#ifdef ARDUINO

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#endif

class ScreenCoords {
	public:
		int x;
		int y;
		void DrawPixel(uint32_t color);
	private:
		// номер матрицы по координатам
		int MatrixNum();
		// номер светодиода по координатам
		int LEDNum();
};

void UpdateMatrix();

// заполняет экран цветом
void ScreenFiller (uint32_t color);

#endif