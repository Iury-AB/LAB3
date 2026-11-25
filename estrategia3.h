
#ifndef ESTRATEGIA_TRES_H
#define ESTRATEGIA_TRES_H

#include "Controle.h"

// ========================================
// ESTRATÉGIA 3
// ========================================

#define TEMPO_CIRCULO_E3 60000
#define DISTANCIA_DETECCAO_E3 700

#define TEMPO_CORRECAO 100

#define TEMPO_GIRO_180_E3 1000

#define LIMIAR_BRANCO_E3 250
#define TEMPO_RECUO_E3 1000

// Relações de velocidade para a trajetória circular
#define RELACAO_CIRCULO_NORMAL 0.7111
#define RELACAO_CIRCULO_CORRECAO 0.55

enum EstadoEstrategia3 {
    E3_CIRCULAR,
    E3_VARREDURA,
    E3_INVESTINDO,
    E3_RECUANDO,
    E3_FINALIZADA
};

void executarEstrategia3() {

    static EstadoEstrategia3 estado = E3_CIRCULAR;

    static unsigned long inicioEstado = 0;

    static bool inicializada = false;

    static bool corrigindoRota = false;
    static bool aguardandoBrancoSair = false;
    static unsigned long inicioCorrecao = 0;

    unsigned long agora = millis();

    int distanciaEsq;
    int distanciaDir;

    // ====================================
    // INICIALIZAÇÃO
    // ====================================

    if (!inicializada) {

        inicioEstado = agora;
        inicializada = true;

        Serial.println("Estrategia 3 iniciada.");
    }

    // ====================================
    // LEITURA DOS TCRT5000
    // ====================================

    int reflexaoEsq = analogRead(TCRT_ESQ);
    int reflexaoDir = analogRead(TCRT_DIR);

    bool brancoEsq = reflexaoEsq < LIMIAR_BRANCO_E3;
    bool brancoDir = reflexaoDir < LIMIAR_BRANCO_E3;

    bool detectouBranco = brancoEsq || brancoDir;

    // ====================================
    // LEITURA DOS VL53L0X
    // ====================================

    bool leituraValida = lerDistancias(
        distanciaEsq,
        distanciaDir
    );

    bool alvoDetectado =
        leituraValida &&
        distanciaEsq > 0 &&
        distanciaDir > 0 &&
        distanciaEsq <= DISTANCIA_DETECCAO_E3 &&
        distanciaDir <= DISTANCIA_DETECCAO_E3;

    // ====================================
    // ESTADO 1: PERCURSO CIRCULAR
    // ====================================

    int tempoCorrecao = 0;

    // ====================================
// ESTADO 1: PERCURSO CIRCULAR
// ====================================

    if (estado == E3_CIRCULAR) {

        // Se os sensores voltaram a detectar preto,
        // permite uma nova correção no futuro.

        if (!detectouBranco) {
            aguardandoBrancoSair = false;
        }

        // --------------------------------
        // EXECUÇÃO DA CORREÇÃO
        // --------------------------------

        if (corrigindoRota) {

            if (agora - inicioCorrecao < TEMPO_CORRECAO) {

                girarEsquerda(100);

            } else {

                corrigindoRota = false;

                pararMotores();

                Serial.println("Correcao concluida.");
            }

            return;
        }

        // --------------------------------
        // DETECÇÃO DA BORDA
        // --------------------------------

        if (detectouBranco && !aguardandoBrancoSair) {

            corrigindoRota = true;
            aguardandoBrancoSair = true;

            inicioCorrecao = agora;

            Serial.println("Borda detectada!");
            Serial.println("Corrigindo rota para esquerda.");

            girarEsquerda(150);

            return;
        }

        // --------------------------------
        // PERCURSO CIRCULAR
        // --------------------------------

        int velocidadeExterna = VELOCIDADE_MAXIMA;

        float relacaoVelocidade;

        if (detectouBranco) {

            relacaoVelocidade = RELACAO_CIRCULO_CORRECAO;

        } else {

            relacaoVelocidade = RELACAO_CIRCULO_NORMAL;

        }

        int velocidadeInterna =
            (int)(velocidadeExterna * relacaoVelocidade);

        moverDiferencial(
            velocidadeInterna,
            velocidadeExterna
        );

        // --------------------------------
        // FINALIZAÇÃO DO PERCURSO
        // --------------------------------

        if (agora - inicioEstado >= TEMPO_CIRCULO_E3) {

            pararMotores();

            estado = E3_VARREDURA;
            inicioEstado = agora;

            Serial.println("Percurso circular concluido.");
            Serial.println("Iniciando varredura.");
        }

        return;
    }

    // ====================================
    // ESTADO 2: VARREDURA DE 180 GRAUS
    // ====================================

    if (estado == E3_VARREDURA) {

        if (alvoDetectado) {

            pararMotores();

            estado = E3_INVESTINDO;

            Serial.println("Alvo detectado pelos dois sensores!");
            Serial.println("Iniciando aproximacao controlada.");

            return;
        }

        if (agora - inicioEstado < TEMPO_GIRO_180_E3) {

            girarNoEixoHorario(VELOCIDADE_MAXIMA);

        } else {

            pararMotores();

            estado = E3_FINALIZADA;

            Serial.println("Varredura concluida sem deteccao.");
        }

        return;
    }

    // ====================================
    // ESTADO 3: INVESTIDA
    // ====================================

    if (estado == E3_INVESTINDO) {

        // Prioridade para detecção da borda.

        if (detectouBranco) {

            pararMotores();

            estado = E3_RECUANDO;
            inicioEstado = agora;

            Serial.println("Borda detectada durante aproximacao!");
            Serial.println("Iniciando recuo.");

            return;
        }

        // Avanço controlado.
        // A leitura dos VL53L0X não precisa continuar
        // identificando o alvo durante o avanço.

        moverFrente(VELOCIDADE_MAXIMA);

        return;
    }

    // ====================================
    // ESTADO 4: RECUO
    // ====================================

    if (estado == E3_RECUANDO) {

        if (agora - inicioEstado < TEMPO_RECUO_E3) {

            moverTras(VELOCIDADE_MAXIMA);

        } else {

            pararMotores();

            estado = E3_VARREDURA;
            inicioEstado = agora;

            Serial.println("Recuo concluido.");
            Serial.println("Reiniciando varredura.");
        }

        return;
    }

    // ====================================
    // ESTADO 5: ESTRATEGIA FINALIZADA
    // ====================================

    if (estado == E3_FINALIZADA) {

        pararMotores();

        return;
    }
}

#endif
