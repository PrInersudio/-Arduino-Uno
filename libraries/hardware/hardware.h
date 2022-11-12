#ifndef __HARDWARE_H__
#define __HARDWARE_H__

// подключаем стандартную библиотеку Arduino (Arduino.h, если версия Arduino IDE 1.0.0 или выше, WProgram.h иначе)
#ifdef ARDUINO

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#endif

// библиотека для работы с RGB-матрицей
#include <Adafruit_NeoPixel.h>
 
// номерв пинов, к которым подключены RGB-матрицы
#define MATRIX_B_PIN    6
#define MATRIX_E_PIN    9
#define MATRIX_C_PIN    4
#define MATRIX_F_PIN    10

// номера пинов, к которым подключён джойстик
#define X  A5
#define Y  A4
#define Z  2

// количество матриц
#define MAT_COUNT_X 2
#define MAT_COUNT_Y 2
#define MAT_COUNT (MAT_COUNT_X*MAT_COUNT_Y)

// количество светодиодов в матрице
#define LED_COUNT_X 4
#define LED_COUNT_Y 4
#define LED_COUNT (LED_COUNT_X*LED_COUNT_Y)

// максимальная яркость матрицы от 0 до 255
#define BRIGHTNESS 5

// размеры экрана
#define SCREEN_LENGHT (LED_COUNT_X*MAT_COUNT_X)
#define SCREEN_HIGHT (LED_COUNT_Y*MAT_COUNT_Y)

// пин пищалки
#define BUZZER_PIN 8

#define BLACK 0
#define WHITE 0xFFFFFF

// для того чтобы получить зерно для ГСЧ используем неподключённый порт
#define ANALOG_PIN_FOR_RND A3

extern Adafruit_NeoPixel matrix [];

enum joystick_pos {not_touched, left, right, up, down, clicked, held}; // double_clicked, triple_clicked}; - на будущее оставлю, вдруг придумаю, как реализовать

// считывает взаимодействие с джостиком
joystick_pos JoystickHandler();

// инициализация вывода пищалки и матриц
void HardwareInit();

#endif