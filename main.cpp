/*Universidad Del Valle de Guatemala
Facultad de Ingeniería
Departamento de Electrónica, Mecatrónica y Biomédica
Electrónica Digital 2

Carlos Daniel Camacho Tista - 22690
Susan Daniela Guevara Catalán - 22519

Laboratorio No. 4 - Señales de PWM

PARTE 1: Se implementa una señal PWM para mover la posición del servo entre su rango máximo, sin dañarlo, utilizando dos botones. 
El primer botón (B1) será para aumentar el movimiento para el lado derecho. 
El botón (B2) sirve para mover el movimiento de regreso a la izquierda.

PARTE 2: Se implentaron tres señales PWM para controlar 3 leds de color Rojo, Verde y Azul utilizando dos botones. 
El primer botón (B3) selecciona cuál de los tres colores queremos modificar. 
El otro botón (B4) puedo modificar el brillo del LED seleccionado:
-Al presionar debe aumentar el brillo.
-Cuando llegue al brillo máximo, si se vuelve a presionar empieza desde apagado y se va incrementando de nuevo.

PARTE 3: Con una rutina se selecciona, con el mismo botón (B3),un lugar al servo. 
Dependiendo de la posición del servo cambie el color del LED RGB.
*/

#include <ESP32Servo.h>
#include <driver/ledc.h>
#include <Arduino.h>

// Pines para los botones
const int BSelec = 32;  // Selección de LED o modo
const int BBrillo = 33; // Cambio de brillo
const int buttonB1 = 23; // Mover servo a la derecha
const int buttonB2 = 21; // Mover servo a la izquierda

// Pines para los LEDs
const int ledr = 2;  // Rojo
const int ledv = 19; // Verde
const int leda = 22; // Azul

// Pin para el servomotor
const int servoPin = 18;

// Variables para seleccionar el color actual
int colorActual = 0; // 0: Rojo, 1: Verde, 2: Azul, 3: Modo Servo

// Variables para el control del servomotor
Servo servo;
int servoPosition = 90; // Posición inicial

void setup() {
  // Configurar pines de botones
  pinMode(BSelec, INPUT_PULLUP);
  pinMode(BBrillo, INPUT_PULLUP);
  pinMode(buttonB1, INPUT_PULLUP);
  pinMode(buttonB2, INPUT_PULLUP);

  // Configurar pines de LEDs
  pinMode(ledr, OUTPUT);
  pinMode(ledv, OUTPUT);
  pinMode(leda, OUTPUT);

  // Configurar PWM para los LEDs
  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);

  // Asignar canales PWM a los LEDs
  ledcAttachPin(ledr, 0);
  ledcAttachPin(ledv, 1);
  ledcAttachPin(leda, 2);

  // Configurar y conectar el servomotor
  servo.attach(servoPin);
  servo.write(servoPosition);

  // Inicializar comunicación serial para depuración
  Serial.begin(115200);
  Serial.println("Inicialización completada.");
}

void loop() {
  static int brillo[3] = {0, 0, 0}; // Brillo para R, G, B
  static bool lastEstadoBBrillo = HIGH;
  static unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 50;

  // Leer estado de los botones
  bool estadoBSelec = digitalRead(BSelec);
  bool estadoBBrillo = digitalRead(BBrillo);

  // Manejar selección de color o modo con el botón B3
  if (estadoBSelec == LOW) {
    colorActual = (colorActual + 1) % 4; // Ciclar entre 0, 1, 2, 3
    Serial.print("Color actual: ");
    Serial.println(colorActual);
    delay(200); // Debouncing simple
  }

  // Ajustar brillo o modo con el botón B4
  if (estadoBBrillo == LOW && lastEstadoBBrillo == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    if (colorActual < 3) {
      brillo[colorActual] = (brillo[colorActual] + 32) % 256;
      ledcWrite(colorActual, brillo[colorActual]);
      Serial.print("Brillo del color ");
      Serial.print(colorActual);
      Serial.print(": ");
      Serial.println(brillo[colorActual]);
    }
  }

  // Actualizar el estado anterior del botón BBrillo
  lastEstadoBBrillo = estadoBBrillo;

  // Control del servomotor
  if (digitalRead(buttonB1) == LOW) {
    servoPosition += 5;
    if (servoPosition > 180) servoPosition = 180;
    servo.write(servoPosition);
    Serial.print("Posición del servo: ");
    Serial.println(servoPosition);
    delay(200); // Debounce delay
  }

  if (digitalRead(buttonB2) == LOW) {
    servoPosition -= 5;
    if (servoPosition < 0) servoPosition = 0;
    servo.write(servoPosition);
    Serial.print("Posición del servo: ");
    Serial.println(servoPosition);
    delay(200); // Debounce delay
  }

  // Modo de cambio de color basado en la posición del servo
  if (colorActual == 3) {
    int colorPosition = map(servoPosition, 0, 180, 0, 255);
    ledcWrite(0, colorPosition);        // Rojo
    ledcWrite(1, 255 - colorPosition);  // Verde
    ledcWrite(2, 0);                    // Azul
    Serial.print("Color del LED basado en la posición del servo: ");
    Serial.println(colorPosition);
  }
}