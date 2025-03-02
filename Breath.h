#pragma once
#ifndef Breath_h
#define Breath_h
#include "FastMapLite.h"

// Definición de macros para simplificar el uso de los pines de reloj y espera.
// switchPin: genera un pulso de reloj bajando y subiendo el pin CLOCK_PIN.
#define switchPin digitalWrite(CLOCK_PIN, HIGH); digitalWrite(CLOCK_PIN, LOW)
// waitPin: espera a que el pin OUT_PIN se ponga en estado bajo (lectura lista).
#define waitPin while(digitalRead(OUT_PIN))
// readyPin: similar a waitPin
#define readyPin !digitalRead(OUT_PIN)
// enRango: macro similar a la funcion constrain pero mas eficiente
#ifndef enRango(v,rI,rS)
#define enRango(v,rI,rS) (v > rS) ? rS : ((v < rI) ? rI : v)
#endif
  
class Breath {
  private:
    byte CLOCK_PIN;         // Pin de reloj para leer datos del sensor.
    byte OUT_PIN;           // Pin de salida del sensor de presión.
    // Unión que facilita el manejo de los datos leídos como un valor de 16 bits
    union {
      uint16_t value;
      uint8_t bytes[2];
    } result;
    int16_t initRead;       // Valor inicial de calibración del sensor.
    int16_t finalRead;      // Valor final leído, ajustado por la resistencia.
    int16_t minRead=0;      // Valor minimo leido
    int16_t maxRead = 255;    // Valor máximo leído (para cálculo).
    int16_t minOut=0;   //minimo valor de salida permitido
    int16_t maxOut=127; //maximo valor de salida permitido
    float resistanceFactor; // Factor que ajusta la sensibilidad de la lectura al soplo.
    FastMapLite mapRead;

  public:
    // Constructor de la clase, recibe los pines de salida y reloj.
    Breath(byte output_pin, byte clock_pin);
    // Función que lee el valor de presión del sensor, lo ajusta y devuelve.
    inline IRAM_ATTR int16_t read();
    // Establece el nivel de resistencia del soplo (1-5).
    void setResistance(uint8_t level);
    // Establece el valor máximo de salida.
    void setMinOut(int16_t minOUT);
    void setMaxOut(int16_t maxOUT);
    void setMinRead(int16_t minREAD);
    // Establece el valor máximo de lectura antes de limitar.
    void setMaxRead(int16_t maxREAD);
};

// Implementación del constructor.
Breath::Breath(byte output_pin, byte clock_pin) {
  CLOCK_PIN = clock_pin;
  OUT_PIN = output_pin;
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(OUT_PIN, INPUT);
  
  // Espera a que el pin de salida esté listo para leer.
  waitPin;
  // Lee los bytes de datos del sensor utilizando shiftIn.
  result.bytes[1] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  result.bytes[0] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  // Realiza una lectura adicional para estabilizar el valor.
  shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  // Realiza tres pulsos de reloj para finalizar la lectura.
  switchPin;
  switchPin;
  switchPin;
  // Ajusta la lectura inicial (calibración) y guarda el valor.
  initRead = (result.value >> 2);
  // Inicializa el factor de resistencia con un valor por defecto.
  resistanceFactor = 1.0f; // Sin modificar la sensibilidad de la lectura.
 mapRead.init(minRead,maxRead,minOut,maxOut);
}

// Establece el nivel de resistencia del soplo, con un rango de 1 a 5.
void Breath::setResistance(uint8_t level) {
  // Asegura que el nivel esté entre 1 (mínima resistencia) y 5 (máxima resistencia).
  level = enRango(level, 1, 5);
  // Define el factor de resistencia para ajustar la sensibilidad de la lectura.
  resistanceFactor = 1 / (1.0f + (level - 1) * 0.5f); 
  // Esto ajusta el factor de 1.0 (sin cambio) hasta 0.2 (mayor resistencia).
}

void Breath::setMinOut(int16_t minOUT) {   minOut=minOUT; }
void Breath::setMinRead(int16_t minREAD) { minRead=minREAD; }
// Establece el valor máximo de salida permitido.
void Breath::setMaxOut(int16_t maxOUT) { maxOut = maxOUT;}
// Establece el valor máximo de lectura antes de limitar.
void Breath::setMaxRead(int16_t maxREAD) { maxRead = maxREAD;}

// Función para leer la presión del sensor y ajustarla.
inline int16_t IRAM_ATTR Breath::read() {
  // Espera a que el sensor esté listo para proporcionar una nueva lectura.
  //waitPin;
  if(readyPin)
  {
  // Lee los bytes de datos del sensor.
  result.bytes[1] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  result.bytes[0] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  // Realiza una lectura adicional para estabilizar el valor.
  shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  // Realiza tres pulsos de reloj para completar la lectura.
  switchPin;
  switchPin;
  switchPin;
  // Ajusta la lectura final restando el valor de calibración inicial y aplicando el factor de resistencia.
  finalRead = ((result.value >> 2) - initRead) * resistanceFactor;
  finalRead = finalRead>4096?finalRead-8192:finalRead;
  }
  // Limita el valor de la lectura entre 0 y el máximo definido.
  // Luego mapea la lectura a un valor de 0 a maxOUT.
  return  mapRead.map(finalRead);
}
#endif
