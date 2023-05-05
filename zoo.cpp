#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <random>
#include <thread>

#define N_LEOES 4
#define N_AVESTRUZES 7
#define N_SURICATOS 10
#define N_VETERINARIOS 2
#define N_FORNECEDORES 1
// Segundos
#define TEMPO_EXECUCAO 72
// Milissegundos
#define TEMPO_ACAO 500

using namespace std;

// Flag para finalizar a execução das threads
bool continuarExecutando = true;
priority_queue<int> queue_leoes;
mutex priority_queue_leoes;
priority_queue<int> queue_avestruzes;
mutex priority_queue_avestruzes;
priority_queue<int> queue_suricatos;
mutex priority_queue_suricatos;

// Comedouros de dentro da jaula dos animais
int comedouroLeoes = 0;
mutex mutexComedouroLeoes;

int comedouroSuricatos = 0;
mutex mutexComedouroSuricatos;

int comedouroAvestruzes = 0;
mutex mutexComedouroAvestruzes;

// Barreira pra finalização e sincronização do relatório
pthread_barrier_t barreira;

// Estoque do zoológico de cada tipo de alimento
int estoqueCarne = 30;
mutex mutexEstoqueCarne;

int estoqueLarvas = 30;
mutex mutexEstoqueLarvas;

int estoquePasto = 30;
mutex mutexEstoquePasto;

class Util {
public:
  static int random(int botLimit, int topLimit) {
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 rng(seed);
    uniform_int_distribution<> distrib(botLimit, topLimit);
    return distrib(rng);
  }

  static void tempo() {

    int segundos = 0;

    while (continuarExecutando) {
      this_thread::sleep_for(chrono::seconds(1));
      cout << endl
           << ++segundos << " HORAS" << endl
           << "Carnes < " << comedouroLeoes << " > |"
           << " Pastos < " << comedouroAvestruzes << " > |"
           << " Larvas < " << comedouroSuricatos << " > |" << endl
           << endl
           << "Est. Carnes < " << estoqueCarne << " > |"
           << " Est. Pastos < " << estoquePasto << " > |"
           << " Est. Larvas < " << estoqueLarvas << " > |" << endl
           << endl;
      if (segundos == TEMPO_EXECUCAO) {
        continuarExecutando = false;
      }
    }
  }
};

class Animal {
protected:
  static mutex mutex_animais;
  static int contador;
  int acessou_comida;
  int registro;
  int id;

public:
  void virtual comer() {}
  void virtual exibir() {}
  void virtual dormir() {}
  void virtual encerrar() {}

  void viver() {
    while (continuarExecutando) {
      comer();
      exibir();
      comer();
      dormir();
    }
    pthread_barrier_wait(&barreira);
    unique_lock<mutex> lockCouts(mutex_animais);
    encerrar();
    lockCouts.unlock();
  }

  Animal() {
    contador++;
    id = contador;
    registro = 0;
    acessou_comida = 0;
  }
};

int Animal::contador = 0;
mutex Animal::mutex_animais;

class Leao : public Animal {
public:
  Leao() : Animal() {}

  void comer() {

    int qtdComida = Util::random(2, 5);
    int tempoDeEspera = 0;
    bool querComer = true;
    int priority = 0;

    unique_lock<mutex> lockComedouroLeoes(mutexComedouroLeoes, defer_lock);

    lock_guard<mutex> lock(priority_queue_leoes);
    priority++;
    queue_leoes.push(priority);

    while (querComer) {

      lockComedouroLeoes.lock();

      if (comedouroLeoes - qtdComida >= 0 && queue_leoes.top() == priority) {
        cout << "LEÃO     < ID " << id << " > COMENDO < " << qtdComida
             << " > CARNES." << endl;
        comedouroLeoes = comedouroLeoes - qtdComida;
        registro = registro + qtdComida;
        acessou_comida++;
        querComer = false;
        queue_leoes.pop();
        priority = 0;
      } else {
        // cout << "LEÃO < " << id << " > TEVE A CHANCE DE COMER, MAS NÃO ROLOU"
        // << endl
        tempoDeEspera++;
        priority++;
        queue_leoes.push(priority);
      }
      lockComedouroLeoes.unlock();

      this_thread::sleep_for(chrono::milliseconds(500));

      if (!continuarExecutando) {
        return;
      }
    }
  }

  void exibir() {
    cout << "LEÃO     < ID " << id << " > EXIBINDO A JUBA." << endl;
    this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
  }

  void dormir() {
    cout << "LEÃO     < ID " << id << " > DORMINDO." << endl;
    this_thread::sleep_for(chrono::seconds(Util::random(8, 12)));
    cout << "LEÃO     < ID " << id << " > ACORDOU." << endl;
  }

  void encerrar() {
    cout << "| RELATÓRIO | >> "
         << "LEÃO      < ID: " << id << " > COMEU: < " << registro
         << " > CARNES."
         << "          | ACESS < " << acessou_comida << " > " << endl;
  }
};

class Avestruz : public Animal {
public:
  Avestruz() : Animal() {}

  void comer() {

    int qtdComida = Util::random(4, 8);
    int tempoDeEspera = 0;
    bool querComer = true;
    int priority = 0;

    unique_lock<mutex> lockComedouroAvestruzes(mutexComedouroAvestruzes,
                                               defer_lock);

    lock_guard<mutex> lock(priority_queue_avestruzes);
    priority++;
    queue_avestruzes.push(priority);

    while (querComer) {

      lockComedouroAvestruzes.lock();

      if (comedouroAvestruzes - qtdComida >= 0 &&
          queue_avestruzes.top() == priority) {
        cout << "AVESTRUZ < ID: " << id << " > COMENDO < " << qtdComida
             << " > PASTOS E ERVAS" << endl;
        comedouroAvestruzes = comedouroAvestruzes - qtdComida;
        registro = registro + qtdComida;
        acessou_comida++;
        querComer = false;
        queue_avestruzes.pop();
        priority = 0;
      } else {
        tempoDeEspera++;
        priority++;
        queue_avestruzes.push(priority);
      }
      lockComedouroAvestruzes.unlock();

      this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));

      if (!continuarExecutando) {
        return;
      }
    }
  }

  void exibir() {

    // unique_lock<mutex> lock(mutex_animais);
    cout << "AVESTRUZ < ID: " << id << " > EXIBINDO O PESCOÇO" << endl;
    // lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
  }

  void dormir() {

    cout << "AVESTRUZ < ID: " << id << " > DORMINDO" << endl;
    this_thread::sleep_for(chrono::seconds(Util::random(1, 2)));
    cout << "AVESTRUZ < ID: " << id << " > ACORDOU" << endl;
  }

  void encerrar() {
    cout << "| RELATÓRIO | >> "
         << "AVESTRUZ  < ID: " << id << " > COMEU: < " << registro
         << " > PASTOS E ERVAS."
         << "  | ACESS < " << acessou_comida << " > " << endl;
  }
};

class Suricato : public Animal {
public:
  Suricato() : Animal() {}

  void comer() {

    int qtdComida = Util::random(1, 2);
    int tempoDeEspera = 0;
    bool querComer = true;
    int priority = 0;

    unique_lock<mutex> lockComedouroSuricatos(mutexComedouroSuricatos,
                                              defer_lock);

    lock_guard<mutex> lock(priority_queue_suricatos);
    priority++;
    queue_suricatos.push(priority);

    while (querComer) {

      lockComedouroSuricatos.lock();

      if (comedouroSuricatos - qtdComida >= 0 &&
          queue_suricatos.top() == priority) {
        cout << "SURICATO < ID: " << id << " > COMENDO < " << qtdComida
             << " > INSETOS E LARVAS." << endl;
        comedouroSuricatos = comedouroSuricatos - qtdComida;
        registro = registro + qtdComida;
        acessou_comida++;
        querComer = false;
        queue_suricatos.pop();
        priority = 0;
      } else {
        tempoDeEspera++;
        priority++;
        queue_suricatos.push(priority);
      }
      lockComedouroSuricatos.unlock();

      this_thread::sleep_for(chrono::milliseconds(500));

      if (!continuarExecutando) {
        return;
      }
    }
  }

  void exibir() {
    cout << "SURICATO < ID: " << id << " > SE EXIBINDO" << endl;
    this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
  }

  void dormir() {
    cout << "SURICATO < ID: " << id << " > DORMINDO" << endl;
    this_thread::sleep_for(chrono::seconds(Util::random(1, 2)));
    cout << "SURICATO < ID: " << id << " > ACORDOU" << endl;
  }

  void encerrar() {
    cout << "| RELATÓRIO | >> "
         << "SURICATO < ID: " << id << " > COMEU: < " << registro
         << " > INSETOS E LARVAS."
         << "  | ACESS < " << acessou_comida << " > " << endl;
  }
};

class Veterinario {

protected:
  static int contador;
  int id;
  bool temComida;

public:
  Veterinario() {
    contador++;
    id = contador;
  }

  void entregarComida() {
    while (continuarExecutando) {

      if (comedouroLeoes == 10) {
        cout << "VETERINÁRIO < ID: " << id << " > "
             << "Comedouro de carne cheio!" << endl;
      } else {

        unique_lock<mutex> lockEstoqueCarne(mutexEstoqueCarne);
        if (estoqueCarne > 0) {
          temComida = true;
          estoqueCarne--;
        } else {
          cout << "Sem carne no estoque!" << endl;
          temComida = false;
        }
        lockEstoqueCarne.unlock();

        unique_lock<mutex> lockLeoes(mutexComedouroLeoes);
        if (temComida) {
          comedouroLeoes++;
        }
        lockLeoes.unlock();

        cout << "VETERINÁRIO < ID: " << id
             << " > moveu < 1 > carne para o comedouro." << endl;
        this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
      }

      if (comedouroAvestruzes == 10) {
        cout << "VETERINÁRIO < ID: " << id << " > "
             << "Comedouro de pastos e ervas cheio!" << endl;
      } else {

        unique_lock<mutex> lockEstoquePasto(mutexEstoquePasto);
        if (estoquePasto > 0) {
          temComida = true;
          estoquePasto--;
        } else {
          cout << "Sem pasto no estoque!" << endl;
          temComida = false;
        }
        lockEstoquePasto.unlock();

        unique_lock<mutex> lockAvestruzes(mutexComedouroAvestruzes);
        if (temComida) {
          comedouroAvestruzes++;
        }

        lockAvestruzes.unlock();

        cout << "VETERINÁRIO < ID: " << id
             << " > moveu < 1 > pasto e ervas para o comedouro." << endl;
        this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
      }

      if (comedouroSuricatos == 10) {
        cout << "VETERINÁRIO < ID: " << id << " > "
             << "Comedouro de insetos e larvas cheio!" << endl;
      } else {

        unique_lock<mutex> lockEstoqueLarvas(mutexEstoqueLarvas);
        if (estoqueLarvas > 0) {
          temComida = true;
          estoqueLarvas--;
        } else {
          cout << "Sem larva no estoque!" << endl;
          temComida = false;
        }
        lockEstoqueLarvas.unlock();

        unique_lock<mutex> lockSuricatos(mutexComedouroSuricatos);
        if (temComida) {
          comedouroSuricatos++;
        }
        lockSuricatos.unlock();

        cout << "VETERINÁRIO < ID: " << id
             << " > moveu < 1 > insetos e larvas para o comedouro." << endl;
        this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
      }
      // Tempo de descanso
      this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
    }
  }

  void viver() { entregarComida(); }
};

int Veterinario::contador = 0;

class Fornecedor {
public:
  void abastecer() {
    while (continuarExecutando) {
      if (estoqueLarvas == 0 || estoquePasto == 0 || estoqueCarne == 0) {

        unique_lock<mutex> lockEstoqueLarvas(mutexEstoqueLarvas);
        estoqueLarvas = 30;
        lockEstoqueLarvas.unlock();

        unique_lock<mutex> lockEstoqueCarnes(mutexEstoqueCarne);
        estoqueCarne = 30;
        lockEstoqueCarnes.unlock();

        unique_lock<mutex> lockEstoquePasto(mutexEstoquePasto);
        estoquePasto = 30;
        lockEstoquePasto.unlock();

        cout << "Fornecedor foi chamado e o estoque está com tudo cheio."
             << endl;
        this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
      }
    }
  }
};

int main() {

  Animal *animais[N_LEOES + N_SURICATOS + N_AVESTRUZES];
  Veterinario *veterinarios[N_VETERINARIOS];
  Fornecedor *fornecedores[N_FORNECEDORES];

  pthread_barrier_init(&barreira, NULL, N_LEOES + N_SURICATOS + N_AVESTRUZES);

  const int N_ANIMAIS = N_LEOES + N_SURICATOS + N_AVESTRUZES;
  thread tempo(&Util::tempo);

  // Criação dos animais

  for (int i = 0; i < N_LEOES; i++) {
    animais[i] = new Leao();
  }

  for (int i = N_LEOES; i < N_LEOES + N_SURICATOS; i++) {
    animais[i] = new Suricato();
  }

  for (int i = N_LEOES + N_SURICATOS; i < N_LEOES + N_SURICATOS + N_AVESTRUZES;
       i++) {
    animais[i] = new Avestruz();
  }

  // Criação dos veterinários

  for (int i = 0; i < N_VETERINARIOS; i++) {
    veterinarios[i] = new Veterinario();
  }

  // Criação dos fornecedores

  for (int i = 0; i < N_FORNECEDORES; i++) {
    fornecedores[i] = new Fornecedor();
  }

  // Criação das Threads

  thread threadsVeterinario[N_VETERINARIOS];
  thread threadsAnimais[N_ANIMAIS];
  thread threadsFornecedores[N_FORNECEDORES];

  // Lança a rotina viver para todos

  for (int i = 0; i < N_ANIMAIS; i++) {
    threadsAnimais[i] = thread(&Animal::viver, animais[i]);
  }

  for (int i = 0; i < N_VETERINARIOS; i++) {
    threadsVeterinario[i] = thread(&Veterinario::viver, veterinarios[i]);
  }

  for (int i = 0; i < N_FORNECEDORES; i++) {
    threadsFornecedores[i] = thread(&Fornecedor::abastecer, fornecedores[i]);
  }

  for (int i = 0; i < N_VETERINARIOS; i++) {
    threadsVeterinario[i].join();
  }

  for (int i = 0; i < N_FORNECEDORES; i++) {
    threadsFornecedores[i].join();
  }
  // Deleta os objetos e threads

  for (int i = 0; i < N_VETERINARIOS; i++) {
    delete veterinarios[i];
  }

  for (int i = 0; i < N_ANIMAIS; i++) {
    threadsAnimais[i].join();
  }

  for (int i = 0; i < N_ANIMAIS; i++) {
    delete animais[i];
  }

  for (int i = 0; i < N_FORNECEDORES; i++) {
    delete fornecedores[i];
  }
  tempo.join();
  pthread_barrier_destroy(&barreira);

  cout << endl
       << " ------ \U0001F60A Fim da Simulação \U0001F60A ------ " << endl;
  return 0;
}
