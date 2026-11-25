
#ifndef ESTRATEGIA_DOIS_H
#define ESTRATEGIA_DOIS_H

#include "Controle.h"

// ================================
// ESTRATÉGIA 2
// BUSCA ALEATÓRIA E APROXIMAÇÃO
// ================================

#define DISTANCIA_DETECCAO 300


#define VELOCIDADE_APROXIMACAO 80

#define TEMPO_GIRO_MIN 1000
#define TEMPO_GIRO_MAX 3000

#define TEMPO_INVESTIDA 3000

#define LIMIAR_BRANCO 200
#define TEMPO_RECUO 1000

enum EstadoEstrategia2 {
  GIRANDO,
  AVANCANDO,
  APROXIMANDO,
  RECUANDO
};

void executarEstrategia2() {

  static EstadoEstrategia2 estado = GIRANDO;

  static unsigned long instanteEstado = 0;
  static unsigned long duracaoGiro = 0;

  static bool inicializada = false;

  int distanciaEsq;
  int distanciaDir;

  unsigned long agora = millis();

  // ================================
  // INICIALIZAÇÃO
  // ================================

  if (!inicializada) {

    randomSeed(analogRead(A2));

    duracaoGiro = random(
      TEMPO_GIRO_MIN,
      TEMPO_GIRO_MAX + 1
    );

    if (random(0, 2) == 0) {
      girarEsquerda(VELOCIDADE_BUSCA);
    } else {
      girarDireita(VELOCIDADE_BUSCA);
    }

    instanteEstado = agora;
    inicializada = true;
  }

  // ================================
  // LEITURA DOS SENSORES TCRT5000
  // ================================

  int reflexaoEsq = analogRead(TCRT_ESQ);
  int reflexaoDir = analogRead(TCRT_DIR);

  bool detectouBranco =
    reflexaoEsq < LIMIAR_BRANCO ||
    reflexaoDir < LIMIAR_BRANCO;

  // ================================
  // DETECÇÃO DE BRANCO
  // ================================

  if (estado != RECUANDO && detectouBranco) {

    estado = RECUANDO;
    instanteEstado = agora;

    Serial.println("Branco detectado! Recuando.");

  }

  // ================================
  // ESTADO DE RECUO
  // ================================

  if (estado == RECUANDO) {

    if (agora - instanteEstado < TEMPO_RECUO) {

      moverTras(VELOCIDADE_MAXIMA);

    } else {

      pararMotores();

      // Reinicia a busca
      duracaoGiro = random(
        TEMPO_GIRO_MIN,
        TEMPO_GIRO_MAX + 1
      );

      if (random(0, 2) == 0) {
        girarEsquerda(VELOCIDADE_BUSCA);
      } else {
        girarDireita(VELOCIDADE_BUSCA);
      }

      estado = GIRANDO;
      instanteEstado = agora;

      Serial.println("Recuo concluido. Retomando busca.");

    }

    return;
  }

  // ================================
  // LEITURA DOS VL53L0X
  // ================================

  if (!lerDistancias(distanciaEsq, distanciaDir)) {
    pararMotores();
    return;
  }

  int menorDistancia = min(
    distanciaEsq,
    distanciaDir
  );

  // ================================
  // DETECÇÃO DO ALVO
  // ================================

  if (estado != APROXIMANDO &&
      menorDistancia < DISTANCIA_DETECCAO) {

    estado = APROXIMANDO;

    Serial.println("Alvo detectado!");
  }

  // ================================
  // MÁQUINA DE ESTADOS
  // ================================

  switch (estado) {

    case GIRANDO:

      if (agora - instanteEstado >= duracaoGiro) {

        moverFrente(VELOCIDADE_BUSCA);

        estado = AVANCANDO;
        instanteEstado = agora;

        Serial.println("Busca: avancando");
      }

      break;

    case AVANCANDO:

      if (agora - instanteEstado >= TEMPO_INVESTIDA) {

        pararMotores();

        duracaoGiro = random(
          TEMPO_GIRO_MIN,
          TEMPO_GIRO_MAX + 1
        );

        if (random(0, 2) == 0) {
          girarEsquerda(VELOCIDADE_BUSCA);
        } else {
          girarDireita(VELOCIDADE_BUSCA);
        }

        estado = GIRANDO;
        instanteEstado = agora;

        Serial.println("Busca: novo giro");
      }

      break;

    case APROXIMANDO:

      if (menorDistancia <= DISTANCIA_DETECCAO) {

        moverFrente(VELOCIDADE_APROXIMACAO);

      } else {

        pararMotores();

        duracaoGiro = random(
          TEMPO_GIRO_MIN,
          TEMPO_GIRO_MAX + 1
        );

        if (random(0, 2) == 0) {
          girarEsquerda(VELOCIDADE_BUSCA);
        } else {
          girarDireita(VELOCIDADE_BUSCA);
        }

        estado = GIRANDO;
        instanteEstado = agora;
      }

      break;

    case RECUANDO:
      // Tratado anteriormente.
      break;
  }
}

#endif
