#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <semaphore>
#include <queue>

#define N_LEOES 10
#define N_AVESTRUZES 0
#define N_SURICATOS 0
#define N_VETERINARIOS 1
// Segundos
#define TEMPO_EXECUCAO 15
// Milissegundos
#define TEMPO_ACAO 500

using namespace std;

bool continuarExecutando = true; // Flag pra finalizar as threads
priority_queue queue_leoes;
priority_queue queue_avestruzes;
priority_queue queue_suricatos;

// Comedouros de dentro da jaula dos animais
int comedouroLeoes = 0;
mutex mutexComedouroLeoes;

int comedouroSuricatos = 0;
mutex mutexComedouroSuricatos;

int comedouroAvestruzes = 0;
mutex mutexComedouroAvestruzes;

// Estoque do zoológico de cada tipo de alimento
int estoqueCarne = 100;
mutex mutexEstoqueCarne;

int estoqueLarvas = 100;
mutex mutexEstoqueLarvas;

int estoquePasto = 100;
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
      cout << endl << ++segundos << " HORAS" << endl << "Comida < " << comedouroLeoes << " > "<< endl;
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
    encerrar();
  }

  Animal() {
    contador++;
    id = contador;
    registro = 0;
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

    queue_leoes.lock();
    priority++;
    queue_leoes.push(priority);
    
    while (querComer) {

      lockComedouroLeoes.lock();
      
        if(comedouroLeoes - qtdComida >= 0 && queue_leoes.top() == priority){
          cout << "LEÃO < " << id << " > COMENDO < " << qtdComida << " > CARNES"
             << endl;
          comedouroLeoes = comedouroLeoes - qtdComida;
          registro = registro + qtdComida;          
          querComer = false;
          queue_leoes.pop();
          queue_leoes.unlock();
        }else{
          //cout << "LEÃO < " << id << " > TEVE A CHANCE DE COMER, MAS NÃO ROLOU" << endl
          tempoDeEspera++;
          priority++;
        }
        lockComedouroLeoes.unlock();
        
      
        this_thread::sleep_for(chrono::milliseconds(500));
        

      if (!continuarExecutando) {
        return;
      }
    }
  }

  void exibir() {
    //unique_lock<mutex> lock(mutex_animais);
    cout << "LEÃO < " << id << " > EXIBINDO A JUBA" << endl;
    //lock.unlock();
    this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
  }

  void dormir() {
    //unique_lock<mutex> lock(mutex_animais);
    cout << "LEÃO < " << id << " > DORMINDO" << endl;
    //lock.unlock();
    this_thread::sleep_for(chrono::seconds(Util::random(1, 2)));
    cout << "LEÃO < " << id << " > ACORDOU" << endl;
  }

  void encerrar() {
    cout << "Relatório >> "
         << "LEÃO < " << id << " > COMEU: < " << registro << " > CARNES"
         << endl;
  }
};

class Avestruz : public Animal {
public:
  Avestruz() : Animal() {}

  void comer() {

    unique_lock<mutex> lock(mutex_animais);
    cout << "Avestruz comendo...      (Avestruz < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(1));
  }

  void exibir() {

    unique_lock<mutex> lock(mutex_animais);
    cout << "Exibindo o pescoço...    (Avestruz < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
  }

  void dormir() {

    unique_lock<mutex> lock(mutex_animais);
    cout << "Dormindo...              (Avestruz < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::seconds(Util::random(1, 2)));
    cout << "Acordei...               (Avestruz < " << id << " >)" << endl;
  }
};

class Suricato : public Animal {
public:
  Suricato() : Animal() {}

  void comer() {

    unique_lock<mutex> lock(mutex_animais);
    cout << "Suricato comendo...      (Suricato < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(1));
  }

  void exibir() {

    unique_lock<mutex> lock(mutex_animais);
    cout << "Suricato se exibindo...  (Suricato < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
  }

  void dormir() {

    unique_lock<mutex> lock(mutex_animais);
    cout << "Dormindo...              (Suricato < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::seconds(Util::random(1, 2)));
    cout << "Acordei...               (Suricato < " << id << " >)" << endl;
  }
};

class Veterinario {

protected:
  static int contador;
  int id;

public:
  Veterinario() {
    contador++;
    id = contador;
  }

  void entregarComida() {
    while (continuarExecutando) {

      if (comedouroLeoes == 30) {
        cout << "Depósito de carne cheio!";
      } else {
        unique_lock<mutex> lockLeoes(mutexComedouroLeoes);
        comedouroLeoes++;
        lockLeoes.unlock();

        unique_lock<mutex> lockEstoqueCarne(mutexEstoqueCarne);
        estoqueCarne--;
        lockEstoqueCarne.unlock();

        cout << "VET < " << id << " > moveu < 1 > carne para o comedouro."
             << endl;
      }
      this_thread::sleep_for(chrono::milliseconds(TEMPO_ACAO));
    }
  }

  void viver() { entregarComida(); }
};

int Veterinario::contador = 0;

class Fornecedor {
  void abastecer() {
    while (continuarExecutando) {
      if (estoqueLarvas == 0 || estoquePasto == 0 || estoqueCarne == 0) {
        estoqueLarvas = 30;
        estoquePasto = 30;
        estoqueCarne = 30;
        cout << "Reestocagem: tudo cheio." << endl;
      }
    }
  }
};

class Comedouro {
protected:
  int capacidade;

public:
  Comedouro() { capacidade = 0; }
};

int main() {

  Animal *animais[N_LEOES + N_SURICATOS + N_AVESTRUZES];
  Veterinario *veterinarios[N_VETERINARIOS];

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

  // Criação das Threads

  thread threadsVeterinario[N_VETERINARIOS];
  thread threadsAnimais[N_ANIMAIS];

  // Lança a rotina viver para todos

  for (int i = 0; i < N_ANIMAIS; i++) {
    threadsAnimais[i] = thread(&Animal::viver, animais[i]);
  }

  for (int i = 0; i < N_VETERINARIOS; i++) {
    threadsVeterinario[i] = thread(&Veterinario::viver, veterinarios[i]);
  }

  for (int i = 0; i < N_VETERINARIOS; i++) {
    threadsVeterinario[i].join();
  }

  for (int i = 0; i < N_VETERINARIOS; i++) {
    delete veterinarios[i];
  }

  for (int i = 0; i < N_ANIMAIS; i++) {
    threadsAnimais[i].join();
  }

  for (int i = 0; i < N_ANIMAIS; i++) {
    delete animais[i];
  }

  tempo.join();

  // cout << "\U0001F346" << endl;
  // cout << "\U0001F4A7" << endl;

  cout << endl
       << " ------ \U0001F60A Fim da Simulação \U0001F60A ------ " << endl;
  return 0;
}
