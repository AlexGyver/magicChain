/*
  Скетч к проекту "Волшебная цепь"
  Страница проекта (схемы, описания): https://alexgyver.ru/magicChain
  Исходники на GitHub: https://github.com/AlexGyver/magicChain
  Нравится, как написан и закомментирован код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  http://AlexGyver.ru/
*/
/*
   Система работает без прерываний, просто потому что почему бы и нет =)
*/

// ------- НАСТРОЙКИ -------
#define FIXED_MOTOR 0     // 1 - частота мотора не меняется крутилкой, 0 - меняется
#define BASE_PERIOD 5000  // мс, период работы мотора по умолчанию (для режима FIXED_MOTOR 0)
#define LIGHT_PIN 4       // пин света (мосфет)
#define DRV_1 2           // пин драйвера 1
#define DRV_2 3           // пин драйвера 2
#define POT_FREQ 0        // пин крутилки частоты драйвера
#define POT_FREQ_L 1      // пин крутилки частоты света
#define EXPO 400          // длительность вспышки света (мкс)

// ------- ДЛЯ РАЗРАБОТЧИКОВ -------
#include "GyverHacks.h"   // библиотека с быстрымыи функциями

#define SET_TIMER 500
boolean flashState, motorState;
uint16_t motorTimer, lightTimer, flashDelay;
uint32_t motorPrev, lightPrev, setPrev;

void setup() {
  Serial.begin(9600);
  setADCrate(2);      // ускоряем analogRead

  pinMode(DRV_1, OUTPUT);
  pinMode(DRV_2, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  motorTimer = BASE_PERIOD;
}

void loop() {
  // таймер чтения крутилок
  if (millis() - setPrev >= SET_TIMER) {
    setPrev = millis();

#if (FIXED_MOTOR == 0)
    // среднее по 4м измерениям
    int potVal = 0;
    for (byte i = 0; i < 4; i++) potVal += analogRead(POT_FREQ);
    potVal /= 4;
    motorTimer = 3000 + potVal * 10;  // (мин. период 3000 мкс)
#endif

    // период света как 2 периода мотора + подстройка
    flashDelay = motorTimer * 2 + analogRead(POT_FREQ_L) - 512;
  }

  // таймер света
  if (micros() - lightPrev >= lightTimer) {
    lightPrev = micros();

    flashState = !flashState;         // инвертируем состояние вспышки
    setPin(LIGHT_PIN, flashState);    // подаём на вспышку

    // перенастраиваем таймер согласно состоянию
    if (flashState) lightTimer = EXPO;
    else lightTimer = flashDelay - EXPO;
  }

  // таймер драйвера
  if (micros() - motorPrev >= motorTimer) {
    motorPrev = micros();
    motorState = !motorState;
    setPin(DRV_1, motorState);
    setPin(DRV_2, !motorState);
  }
}
