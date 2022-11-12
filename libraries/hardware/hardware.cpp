#include "hardware.h"

Adafruit_NeoPixel matrix [] = {  Adafruit_NeoPixel(LED_COUNT, MATRIX_F_PIN, NEO_GRB + NEO_KHZ800),
                                 Adafruit_NeoPixel(LED_COUNT, MATRIX_E_PIN, NEO_GRB + NEO_KHZ800),
                                 Adafruit_NeoPixel(LED_COUNT, MATRIX_C_PIN, NEO_GRB + NEO_KHZ800),
                                 Adafruit_NeoPixel(LED_COUNT, MATRIX_B_PIN, NEO_GRB + NEO_KHZ800)
                              };

// считывает взаимодействие с джостиком
joystick_pos JoystickHandler() {
	if (analogRead(X) < 350)
		return left;
	if (analogRead(X) > 850)
		return right;
	if (analogRead(Y) > 850)
		return down;
	if (analogRead(Y) < 350)
		return up;
  int hold_time = 0;
  while (digitalRead(Z)) {
    hold_time++;
    if (hold_time >= 1000)
      return held;
    delay(1);
  }
  if (hold_time > 0)
    return clicked;
  return not_touched;
}

// инициализация вывода пищалки и матриц
void HardwareInit() {
	// инициализируем выход пищалки
	pinMode(BUZZER_PIN, OUTPUT);
	// инициализируем LED матрицы и устанавливаем их яркость
	for (int i = 0; i < MAT_COUNT; i++) {
		matrix[i].begin();
		matrix[i].setBrightness(BRIGHTNESS);
	}
}