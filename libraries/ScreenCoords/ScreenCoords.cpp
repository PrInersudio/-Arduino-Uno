#include "ScreenCoords.h"
#include "hardware.h"

// номер матрицы по координатам
int ScreenCoords::MatrixNum() {
	if (x < 0) x = -x;
	if (y < 0) y = -y;
	x %= SCREEN_LENGHT;
	y %= SCREEN_HIGHT;
	return x/LED_COUNT_X + y/LED_COUNT_Y*MAT_COUNT_X;
}

// номер светодиода по координатам
int ScreenCoords::LEDNum () {
	if (x < 0) x = -x;
	if (y < 0) y = -y;
	// так как матрицы в нижнем ряду стоят вверх ногами
	if (MatrixNum() >= MAT_COUNT_X * (MAT_COUNT_Y/2))
		return LED_COUNT - 1 - (x%LED_COUNT_X + y%LED_COUNT_Y * LED_COUNT_X);
	return x%LED_COUNT_X + y%LED_COUNT_Y * LED_COUNT_X;
}

void ScreenCoords::DrawPixel (uint32_t color) {
	matrix[MatrixNum()].setPixelColor(LEDNum(), color);
}

void UpdateMatrix() {
	for (int i = 0; i < MAT_COUNT; i++)
		matrix[i].show();
}

// заполняет экран цветом
void ScreenFiller (uint32_t color) {
	ScreenCoords pixel;
	for (int i = 0; i < SCREEN_LENGHT; i++)
		for (int j = 0; j < SCREEN_HIGHT; j++) {
			pixel = {i,j};
			pixel.DrawPixel(color);
    }
	UpdateMatrix();
}