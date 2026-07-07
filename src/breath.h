#pragma once
// ==================== OPTIMIZACIONES ====================
#define NDEBUG
#if defined(CONFIG_IDF_TARGET_ESP32)
    typedef int16_t v4i16 __attribute__((vector_size(8))); typedef uint16_t v4u16 __attribute__((vector_size(8)));
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    typedef int16_t v4i16 __attribute__((vector_size(8))); typedef uint16_t v4u16 __attribute__((vector_size(8)));
    typedef int32_t v4i32 __attribute__((vector_size(16))); typedef uint32_t v4u32 __attribute__((vector_size(16)));
#endif
#pragma GCC optimize ("Ofast") 
#pragma GCC optimize ("omit-frame-pointer", "tree-vectorize", "no-branch-protection") 
#pragma GCC optimize ("prefetch-loop-arrays", "unroll-loops", "ipa-cp-clone", "gcse-las")

#ifndef Breath_h
#define Breath_h
#include <quickMap.h> // <-- Cambiado a tu librería oficial
#include <driver/gpio.h>
// Definición de macros para simplificar el uso de los pines de reloj y espera.
#define switchPin gpio_set_level((gpio_num_t) CLOCK_PIN, 1); delayMicroseconds(1); gpio_set_level((gpio_num_t) CLOCK_PIN, 0); 
#define waitPin while(gpio_get_level((gpio_num_t)OUT_PIN))
#define readyPin (gpio_get_level((gpio_num_t)OUT_PIN) == 0)
// enRango: macro similar a la funcion constrain pero mas eficiente
#ifndef enRango
#define enRango(v,rI,rS) ((v) > (rS)) ? (rS) : (((v) < (rI)) ? (rI) : (v))
#endif
class Breath {
  private:
    byte CLOCK_PIN;         // Pin de reloj para leer datos del sensor.
    byte OUT_PIN;           // Pin de salida del sensor de presión.
    union {
      uint16_t value;
      uint8_t bytes[2];
    } result;
    float resistanceFactor; // Factor que ajusta la sensibilidad de la lectura al soplo.
    int16_t initRead;       // Valor inicial de calibración del sensor.
    int16_t finalRead;      // Valor final leído, ajustado por la resistencia.
    int16_t minRead = 0;    // Valor minimo leido
    int16_t maxRead = 255;  // Valor máximo leído (para cálculo).
    int16_t minOut = 0;     // minimo valor de salida permitido
    int16_t maxOut = 127;   // maximo valor de salida permitido
    quickMap mapRead;       // <-- Cambiado a tu clase quickMap
  public:
    Breath(byte output_pin, byte clock_pin);
    inline int16_t read();
    void setResistance(uint8_t level);
    void setMinOut(int16_t minOUT);
    void setMaxOut(int16_t maxOUT);
    void setMinRead(int16_t minREAD);
    void setMaxRead(int16_t maxREAD);
};
// Implementación del constructor.
Breath::Breath(byte output_pin, byte clock_pin) {
  CLOCK_PIN = clock_pin;
  OUT_PIN = output_pin;
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(OUT_PIN, INPUT);
  waitPin;
  result.bytes[1] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  result.bytes[0] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  switchPin;
  switchPin;
  switchPin;
  initRead = (result.value >> 2);
  resistanceFactor = 1.0f; 
  // Inicialización limpia usando tu nuevo quickMap (acepta ints directamente)
  mapRead.init(minRead, maxRead, minOut, maxOut);
}
// Establece el nivel de resistencia del soplo (1 a 5).
void Breath::setResistance(uint8_t level) {
  level = enRango(level, 1, 5);
  // Multiplicadores directos precalculados para evitar la división (1 / x) en ejecución
  // level 1 = 1.0, level 2 = 0.666, level 3 = 0.5, level 4 = 0.4, level 5 = 0.333
  static const float factors[5] = {1.0f, 0.666667f, 0.5f, 0.4f, 0.333333f};
  resistanceFactor = factors[level - 1];
}
// Métodos de configuración actualizados para tu quickMap
void Breath::setMinOut(int16_t minOUT)   { minOut = minOUT;   mapRead.init(minRead, maxRead, minOut, maxOut); }
void Breath::setMinRead(int16_t minREAD) { minRead = minREAD; mapRead.init(minRead, maxRead, minOut, maxOut); }
void Breath::setMaxOut(int16_t maxOUT)   { maxOut = maxOUT;   mapRead.init(minRead, maxRead, minOut, maxOut); }
void Breath::setMaxRead(int16_t maxREAD) { maxRead = maxREAD; mapRead.init(minRead, maxRead, minOut, maxOut); }
// Función crítica para leer la presión del sensor y ajustarla.
inline int16_t Breath::read() {
  if (readyPin) {
    result.bytes[1] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
    result.bytes[0] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
    shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
    switchPin;
    switchPin;
    switchPin;
    finalRead = ((result.value >> 2) - initRead) * resistanceFactor;
    finalRead = finalRead > 4096 ? finalRead - 8192 : finalRead;
  }
  // Realiza el mapeo ultra rápido con límites automáticos y lo devuelve como int16_t
  return (int16_t)mapRead.map(finalRead);
}
#endif
