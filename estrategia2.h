
#ifndef ESTRATEGIA_DOIS_H
#define ESTRATEGIA_DOIS_H

#include "Controle.h"

// ================================
// ESTRATÉGIA 2
// BUSCA ALEATÓRIA E APROXIMAÇÃO
// ================================

#define DISTANCIA_DETECCAO 700
#define DISTANCIA_SEGURANCA 50

#define VELOCIDADE_BUSCA 100
#define VELOCIDADE_APROXIMACAO 120

#define TEMPO_GIRO_MIN 1000
#define TEMPO_GIRO_MAX 3000

#define TEMPO_INVESTIDA 3000

enum EstadoEstrategia2 {
  GIRANDO,
  AVANCANDO,
  APROXIMANDO
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

    randomSeed(analogRead(A0));

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
  // LEITURA DOS SENSORES
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

    // ----------------------------
    // BUSCA: GIRO
    // ----------------------------

    case GIRANDO:

      Serial.println("GIRANDO");

      if (agora - instanteEstado >= duracaoGiro) {

        moverFrente(VELOCIDADE_BUSCA);

        estado = AVANCANDO;
        instanteEstado = agora;

        Serial.println("Busca: avancando");

      }

      break;

    // ----------------------------
    // BUSCA: AVANÇO
    // ----------------------------

    case AVANCANDO:
      Serial.println("AVANCANDO");

      if (agora - instanteEstado >= TEMPO_INVESTIDA) {

        pararMotores();

        // Escolhe um novo giro
        duracaoGiro = random(
          TEMPO_GIRO_MIN,
          TEMPO_GIRO_MAX + 1
        );

        if (random(0, 2) == 0) {
          girarEsquerda(VELOCIDADE_BUSCA);
          Serial.println("Girando para esquerda");
        } else {
          girarDireita(VELOCIDADE_BUSCA);
          Serial.println("Girando para direita");
        }

        estado = GIRANDO;
        instanteEstado = agora;

        Serial.println("Busca: novo giro");

      }

      break;

    // ----------------------------
    // APROXIMAÇÃO CONTROLADA
    // ----------------------------

    case APROXIMANDO:
      Serial.println("APROXIMANDO");

      if (menorDistancia <= DISTANCIA_SEGURANCA) {

        pararMotores();

        Serial.println("Distancia de seguranca atingida!");

      } else if (menorDistancia <= DISTANCIA_DETECCAO) {

        moverFrente(VELOCIDADE_APROXIMACAO);

      } else {
        Serial.println(menorDistancia);
        Serial.println("GIRANDO");
        pararMotores();

        // Escolhe um novo giro
        duracaoGiro = random(
          TEMPO_GIRO_MIN,
          TEMPO_GIRO_MAX + 1
        );

        if (random(0, 2) == 0) {
          girarEsquerda(VELOCIDADE_BUSCA);
          Serial.println("Girando para esquerda");
        } else {
          girarDireita(VELOCIDADE_BUSCA);
          Serial.println("Girando para direita");
        }

        estado = GIRANDO;
        instanteEstado = agora;

      }

      break;

  }

}


#endif
