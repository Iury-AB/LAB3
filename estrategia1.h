#ifndef ESTRATEGIA_UM_H
#define ESTRATEGIA_UM_H

#include "Controle.h"


// ========================================
// ESTRATÉGIA 1
// ATAQUE FRONTAL DIRETO
// ========================================

#define DMAX_E1 300

#define LIMIAR_BRANCO_E1 250
#define TEMPO_RECUO_E1 2000

enum EstadoEstrategia1 {
    E1_BUSCANDO,
    E1_ALINHANDO_ESQ,
    E1_ALINHANDO_DIR,
    E1_INVESTINDO,
    E1_RECUANDO
};

void executarEstrategia1() {

    static EstadoEstrategia1 estado = E1_BUSCANDO;

    static unsigned long instanteRecuo = 0;

    unsigned long agora = millis();

    int distanciaEsq;
    int distanciaDir;

    // ====================================
    // LEITURA DOS SENSORES DE REFLEXÃO
    // ====================================

    int reflexaoEsq = analogRead(TCRT_ESQ);
    int reflexaoDir = analogRead(TCRT_DIR);

    bool brancoEsq = reflexaoEsq < LIMIAR_BRANCO_E1;
    bool brancoDir = reflexaoDir < LIMIAR_BRANCO_E1;

    // ====================================
    // PRIORIDADE 1: DETECÇÃO DA BORDA
    // ====================================

    if (estado != E1_RECUANDO && (brancoEsq || brancoDir)) {

        estado = E1_RECUANDO;
        instanteRecuo = agora;

        pararMotores();

        Serial.println("Borda detectada! Recuando.");
    }

    // ====================================
    // ESTADO DE RECUO
    // ====================================

    if (estado == E1_RECUANDO) {

        if (agora - instanteRecuo < TEMPO_RECUO_E1) {

            moverTras(VELOCIDADE_MAXIMA);

        } else {

            pararMotores();

            estado = E1_BUSCANDO;

            Serial.println("Recuo concluido. Retomando busca.");
        }

        return;
    }

    // ====================================
    // LEITURA DOS VL53L0X
    // ====================================

    if (!lerDistancias(distanciaEsq, distanciaDir)) {

        pararMotores();
        return;
    }

    // ====================================
    // DETECÇÃO DO ADVERSÁRIO
    // ====================================

    bool detectouEsq = distanciaEsq > 0 &&
                       distanciaEsq <= DMAX_E1;

    bool detectouDir = distanciaDir > 0 &&
                       distanciaDir <= DMAX_E1;

    // ====================================
    // MÁQUINA DE ESTADOS
    // ====================================

    switch (estado) {

        // --------------------------------
        // BUSCA: NENHUM SENSOR DETECTA
        // --------------------------------

        case E1_BUSCANDO:

            if (detectouEsq && detectouDir) {

                estado = E1_INVESTINDO;

                Serial.println("Adversario alinhado!");
            }

            else if (detectouEsq) {

                estado = E1_ALINHANDO_ESQ;

                Serial.println("Alvo detectado a esquerda.");
            }

            else if (detectouDir) {

                estado = E1_ALINHANDO_DIR;

                Serial.println("Alvo detectado a direita.");
            }

            else {

                girarEsquerda(VELOCIDADE_MAXIMA);
            }

            break;

        // --------------------------------
        // ALINHAMENTO PELO SENSOR ESQUERDO
        // --------------------------------

        case E1_ALINHANDO_ESQ:

            if (detectouEsq && detectouDir) {

                estado = E1_INVESTINDO;

                Serial.println("Adversario alinhado!");
            }

            else if (detectouEsq) {

                girarEsquerda(VELOCIDADE_BUSCA);
            }

            else if (detectouDir) {

                estado = E1_ALINHANDO_DIR;
            }

            else {

                estado = E1_BUSCANDO;
            }

            break;

        // --------------------------------
        // ALINHAMENTO PELO SENSOR DIREITO
        // --------------------------------

        case E1_ALINHANDO_DIR:

            if (detectouEsq && detectouDir) {

                estado = E1_INVESTINDO;

                Serial.println("Adversario alinhado!");
            }

            else if (detectouDir) {

                girarDireita(VELOCIDADE_BUSCA);
            }

            else if (detectouEsq) {

                estado = E1_ALINHANDO_ESQ;
            }

            else {

                estado = E1_BUSCANDO;
            }

            break;

        // --------------------------------
        // INVESTIDA FRONTAL
        // --------------------------------

        case E1_INVESTINDO:

            if (detectouEsq && detectouDir) {

                moverFrente(VELOCIDADE_MAXIMA);
            }

            else if (detectouEsq) {

                estado = E1_ALINHANDO_ESQ;
            }

            else if (detectouDir) {

                estado = E1_ALINHANDO_DIR;
            }

            else {

                estado = E1_BUSCANDO;
            }

            break;

        case E1_RECUANDO:
            // Tratado anteriormente.
            break;
    }
}


#endif