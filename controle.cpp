#include "controle.h"

// ================================
// INICIALIZAÇÃO DOS PINOS
// ================================

void configurarControle() {

  pinMode(RPWM_ESQ, OUTPUT);
  pinMode(LPWM_ESQ, OUTPUT);

  pinMode(RPWM_DIR, OUTPUT);
  pinMode(LPWM_DIR, OUTPUT);

  pinMode(BOTAO, INPUT_PULLUP);

  pinMode(SWITCH1R, INPUT_PULLUP);
  pinMode(SWITCH1L, INPUT_PULLUP);
  pinMode(SWITCH2R, INPUT_PULLUP);
  pinMode(SWITCH2L, INPUT_PULLUP);

  pinMode(TCRT_ESQ, INPUT);
  pinMode(TCRT_DIR, INPUT);

  pinMode(XSHUT_ESQ, OUTPUT);
  pinMode(XSHUT_DIR, OUTPUT);

  pararMotores();
}

// ================================
// INICIALIZAÇÃO DOS SENSORES
// ================================

Adafruit_VL53L0X sensorEsq;
Adafruit_VL53L0X sensorDir;

bool inicializarSensores() {

  digitalWrite(XSHUT_ESQ, LOW);
  digitalWrite(XSHUT_DIR, LOW);

  delay(100);

  Wire.begin();
  Wire1.begin();

  // Sensor esquerdo
  digitalWrite(XSHUT_ESQ, HIGH);
  delay(100);

  if (!sensorEsq.begin(0x29, false, &Wire1)) {
    Serial.println("Erro no sensor esquerdo!");
    return false;
  }

  Serial.println("Sensor esquerdo inicializado!");

  // Sensor direito
  digitalWrite(XSHUT_DIR, HIGH);
  delay(100);

  if (!sensorDir.begin(0x29, false, &Wire)) {
    Serial.println("Erro no sensor direito!");
    return false;
  }

  Serial.println("Sensor direito inicializado!");

  return true;
}

// ================================
// CONTROLE DOS MOTORES
// ================================

void pararMotorEsq() {
  analogWrite(RPWM_ESQ, 0);
  analogWrite(LPWM_ESQ, 0);
}

void pararMotorDir() {
  analogWrite(RPWM_DIR, 0);
  analogWrite(LPWM_DIR, 0);
}

void pararMotores() {
  pararMotorEsq();
  pararMotorDir();
}

// ================================
// CONTROLE INDIVIDUAL DOS MOTORES
// ================================

// Motor esquerdo para frente
void motorEsqFrente(int velocidade) {
  velocidade = constrain(velocidade, 0, 255);

  analogWrite(RPWM_ESQ, velocidade);
  analogWrite(LPWM_ESQ, 0);
}

// Motor esquerdo para trás
void motorEsqTras(int velocidade) {
  velocidade = constrain(velocidade, 0, 255);

  analogWrite(RPWM_ESQ, 0);
  analogWrite(LPWM_ESQ, velocidade);
}

// Motor direito para frente
void motorDirFrente(int velocidade) {
  velocidade = constrain(velocidade, 0, 255);

  analogWrite(RPWM_DIR, 0);
  analogWrite(LPWM_DIR, velocidade);
}

// Motor direito para trás
void motorDirTras(int velocidade) {
  velocidade = constrain(velocidade, 0, 255);

  analogWrite(RPWM_DIR, velocidade);
  analogWrite(LPWM_DIR, 0);
}

// ================================
// MOVIMENTAÇÃO DO ROBÔ
// ================================

// Ambos os motores para frente
void moverFrente(int velocidade) {
  motorEsqFrente(velocidade);
  motorDirFrente(velocidade);
}

// Ambos os motores para trás
void moverTras(int velocidade) {
  motorEsqTras(velocidade);
  motorDirTras(velocidade);
}

// Giro para esquerda
void girarEsquerda(int velocidade) {
  motorEsqTras(velocidade);
  motorDirFrente(velocidade);
}

// Giro para direita
void girarDireita(int velocidade) {
  motorEsqFrente(velocidade);
  motorDirTras(velocidade);
}

// ================================
// LEITURA DOS SWITCHES
// ================================

int verificaEstrategia() {

  int switch1 = digitalRead(SWITCH1L);
  int switch2 = digitalRead(SWITCH2L);

  if (switch1 == HIGH && switch2 == HIGH) {
    return 1;
  }
  else if (switch1 == LOW && switch2 == HIGH) {
    return 2;
  }
  else if (switch1 == LOW && switch2 == LOW) {
    return 3;
  }
  else {
    return 0;
  }
}

void imprimirSwitches() {

  Serial.println("------------------------");
  Serial.print("Switch 1 - R: ");
  Serial.print(digitalRead(SWITCH1R));

  Serial.print(" | L: ");
  Serial.println(digitalRead(SWITCH1L));

  Serial.print("Switch 2 - R: ");
  Serial.print(digitalRead(SWITCH2R));

  Serial.print(" | L: ");
  Serial.println(digitalRead(SWITCH2L));
  Serial.println("------------------------");
  delay(500);
}

// ================================
// LEITURA DOS SENSORES
// ================================

bool lerDistancias(int &distanciaEsq, int &distanciaDir) {

  VL53L0X_RangingMeasurementData_t medidaEsq;
  VL53L0X_RangingMeasurementData_t medidaDir;

  sensorEsq.rangingTest(&medidaEsq, false);
  sensorDir.rangingTest(&medidaDir, false);

  Serial.print("Esquerdo: ");
  Serial.print(medidaEsq.RangeMilliMeter);
  Serial.print("mm | Direito: ");
  Serial.print(medidaDir.RangeMilliMeter);
  Serial.println("mm");

  //if (medidaEsq.RangeStatus != 0 ||
  //    medidaDir.RangeStatus != 0) {
  //
  //  return false;
  //}

  distanciaEsq = medidaEsq.RangeMilliMeter;
  distanciaDir = medidaDir.RangeMilliMeter;

  return true;
}