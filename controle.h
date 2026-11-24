
#ifndef CONTROLE_H
#define CONTROLE_H

#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// ================================
// SENSORES VL53L0X
// ================================

#define XSHUT_ESQ 32
#define XSHUT_DIR 33

// ================================
// MOTORES BTS7960
// ================================

#define RPWM_ESQ 6
#define LPWM_ESQ 7

#define RPWM_DIR 5
#define LPWM_DIR 4

// ================================
// BOTÃO
// ================================

#define BOTAO 30

// ================================
// SWITCHES
// ================================

#define SWITCH1R 28
#define SWITCH1L 26

#define SWITCH2R 24
#define SWITCH2L 22

// =================================
// SENSORES DE REFLEXÃO
// ==================================
#define TCRT_ESQ A0
#define TCRT_DIR A1

// ================================
// CONFIGURAÇÕES
// ================================

#define DISTANCIA_LIMITE 200
#define VELOCIDADE_MAXIMA 80

#define TEMPO_ESPERA 5000
#define DEBOUNCE 50

void configurarControle() ;
bool inicializarSensores();
void pararMotorEsq();
void pararMotorDir();
void pararMotores();
void motorEsqFrente(int velocidade);
void motorEsqTras(int velocidade);
void motorDirFrente(int velocidade);
void motorDirTras(int velocidade);
void moverFrente(int velocidade);
void moverTras(int velocidade);
void girarEsquerda(int velocidade);
void girarDireita(int velocidade) ;
int verificaEstrategia();
void imprimirSwitches() ;
bool lerDistancias(int &distanciaEsq, int &distanciaDir);
void motorEsqVelocidade(int velocidade);
void motorDirVelocidade(int velocidade);
void moverDiferencial(int velocidadeEsq, int velocidadeDir);
void girarNoEixoHorario(int velocidade);
void girarNoEixoAntiHorario(int velocidade);

#endif
