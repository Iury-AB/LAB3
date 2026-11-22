
#include "controle.h"
#include "estrategia1.h"
#include "estrategia2.h"
#include "estrategia3.h"

// ================================
// CONTROLE DE INICIALIZAÇÃO
// ================================

bool botaoPressionado = false;
bool roboLiberado = false;
bool aguardandoSoltar = false;

bool estrategiaCapturada = false;

int estrategiaSelecionada = 0;

unsigned long instanteMudancaBotao = 0;
unsigned long instanteInicioEspera = 0;

int ultimaLeituraBotao = HIGH;
int estadoBotaoEstavel = HIGH;

// ================================
// CONTROLE DO BOTÃO
// ================================

void verificarBotao() {

  int leituraAtual = digitalRead(BOTAO);

  // Detecta alteração no estado físico do botão
  if (leituraAtual != ultimaLeituraBotao) {

    ultimaLeituraBotao = leituraAtual;
    instanteMudancaBotao = millis();

  }

  // Confirma alteração após o debounce
  if (millis() - instanteMudancaBotao >= DEBOUNCE &&
      leituraAtual != estadoBotaoEstavel) {

    estadoBotaoEstavel = leituraAtual;

    // ----------------------------
    // BOTÃO PRESSIONADO
    // ----------------------------

    if (estadoBotaoEstavel == LOW &&
        !estrategiaCapturada) {

      botaoPressionado = true;

      // Salva a estratégia apenas uma vez
      estrategiaSelecionada = verificaEstrategia();
      estrategiaCapturada = true;

      Serial.print("Estrategia selecionada: ");
      Serial.println(estrategiaSelecionada);

    }

    // ----------------------------
    // BOTÃO SOLTO
    // ----------------------------

    else if (estadoBotaoEstavel == HIGH &&
             botaoPressionado &&
             !roboLiberado) {

      botaoPressionado = false;
      aguardandoSoltar = true;

      instanteInicioEspera = millis();

      Serial.println("Botao solto!");
      Serial.println("Aguardando 5 segundos...");

    }

  }

  // ================================
  // ESPERA DE 5 SEGUNDOS
  // ================================

  if (aguardandoSoltar && !roboLiberado) {

    if (millis() - instanteInicioEspera >= TEMPO_ESPERA) {

      aguardandoSoltar = false;
      roboLiberado = true;

      Serial.println("Tempo concluido!");
      Serial.println("ROBO LIBERADO!");

    }

  }

}

// ================================
// EXECUÇÃO DA ESTRATÉGIA
// ================================

void executarEstrategiaSelecionada() {

  switch (estrategiaSelecionada) {

    case 1:
      executarEstrategia1();
      break;

    case 2:
      executarEstrategia2();
      break;

    case 3:
      executarEstrategia3();
      break;

    default:
      pararMotores();
      break;

  }

}

// ================================
// SETUP
// ================================

void setup() {

  Serial.begin(115200);

  configurarControle();

  if (!inicializarSensores()) {

    Serial.println("Falha na inicializacao!");

    while (true) {
      pararMotores();
    }

  }

  Serial.println("Sistema inicializado!");
  Serial.println("Pressione e solte o botao para iniciar.");

}

// ================================
// LOOP PRINCIPAL
// ================================

void loop() {

  verificarBotao();
  

  if (!roboLiberado) {

    pararMotores();

    delay(10);
    return;

  }

  executarEstrategiaSelecionada();

  delay(100);

}
