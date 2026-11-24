#ifndef ESTRATEGIA_TRES_H
#define ESTRATEGIA_TRES_H

#include "Controle.h"


// ========================================
// ESTRATEGIA 3
// ========================================

#define TEMPO_CIRCULO 60000
#define DISTANCIA_DETECCAO_E3 700

// Tempo inicial estimado para o giro de 180 graus.
// Deve ser ajustado experimentalmente.
#define TEMPO_GIRO_180 1000

enum EstadoEstrategia3 {
    E3_CIRCULAR,
    E3_VARREDURA,
    E3_ALVO_DETECTADO,
    E3_FINALIZADA
};

void executarEstrategia3() {

    static EstadoEstrategia3 estado = E3_CIRCULAR;
    static unsigned long inicioEstado = 0;
    static bool inicializada = false;

    if (!inicializada) {
        inicioEstado = millis();
        inicializada = true;
    }

    int distanciaEsq;
    int distanciaDir;

    bool leituraValida = lerDistancias(
        distanciaEsq,
        distanciaDir
    );

    // ------------------------------------
    // ESTADO 1: PERCURSO CIRCULAR
    // ------------------------------------

    if (estado == E3_CIRCULAR) {

        int velocidadeExterna = VELOCIDADE_MAXIMA;

        int velocidadeInterna =
            (int)(VELOCIDADE_MAXIMA * 32.0 / 45.0);

        moverDiferencial(
            velocidadeInterna,
            velocidadeExterna
        );

        if (millis() - inicioEstado >= TEMPO_CIRCULO) {

            pararMotores();

            estado = E3_VARREDURA;
            inicioEstado = millis();
        }

        return;
    }

    // ------------------------------------
    // ESTADO 2: VARREDURA DE 180 GRAUS
    // ------------------------------------

    if (estado == E3_VARREDURA) {

        // Os dois sensores precisam detectar
        // simultaneamente o alvo.

        bool alvoDetectado =
            leituraValida &&
            distanciaEsq > 0 &&
            distanciaDir > 0 &&
            distanciaEsq < DISTANCIA_DETECCAO_E3 &&
            distanciaDir < DISTANCIA_DETECCAO_E3;

        if (alvoDetectado) {

            pararMotores();

            estado = E3_ALVO_DETECTADO;
            return;
        }

        // Giro sobre o próprio eixo.
        if (millis() - inicioEstado < TEMPO_GIRO_180) {

            girarNoEixoHorario(VELOCIDADE_MAXIMA);

        } else {

            pararMotores();

            estado = E3_FINALIZADA;
        }

        return;
    }

    // ------------------------------------
    // ESTADO 3: ALVO DETECTADO
    // ------------------------------------

    if (estado == E3_ALVO_DETECTADO) {

        pararMotores();

        // Aguardar a próxima ação controlada.
        return;
    }

    // ------------------------------------
    // ESTADO 4: ESTRATEGIA FINALIZADA
    // ------------------------------------

    if (estado == E3_FINALIZADA) {

        pararMotores();
        return;
    }
}


#endif