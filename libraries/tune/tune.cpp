#include "tune.h"
bool sound_on = false;
// функция музыки
void PlayTune (struct Note* Tune, int note_quan) {
  if (!sound_on) return;
  for (int i = 0; i < note_quan; i++) {
    tone(BUZZER_PIN, Tune[i].note_num, Tune[i].note_dur);
    // в примерах Arduino IDE предлагается делать паузу между нотами, равную длительности ноты + 30%
    delay(Tune[i].note_dur * 1.30);
    noTone(BUZZER_PIN);
  }
}