#ifndef __TUNE_H__
#define __TUNE_H__

#ifdef ARDUINO

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#endif

#include "pitches.h"

// пин пищалки
#define BUZZER_PIN 8

extern bool sound_on;

struct Note {
  int note_num;
  int note_dur;
};

// функция музыки
void PlayTune (struct Note* Tune, int note_quan);

#endif