#include <chrono> // Função SLEEP
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random> // Sorteio de quantas horas serão dormidas
#include <thread>

#define N_LEOES 1
#define N_AVESTRUZES 0
#define N_SURICATOS 0
#define N_VETERINARIOS 1

using namespace std;

mutex mtx; // Mutex para sincronizar o acesso ao tempo (relógio global)
condition_variable
    cv; // variável condicional para notificar outras threadsAnimais

int tempo = 0; // tempo em horas

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
    while (true) {
      // Executa a tarefa a cada segundo
      this_thread::sleep_for(chrono::seconds(1));
      cout << "Executando a tarefa depois de " << ++segundos << " segundos\n";
    }
  }
};

class Animal {
protected:
  static mutex mutex_;
  static int contador;
  static int registro;
  int id;

public:
  void virtual comer() {}
  void virtual exibir() {}
  void virtual dormir() {}

  void viver() {
    while (true) {
      comer();
      exibir();
      comer();
      dormir();
    }
  }

  Animal() {
    contador++;
    id = contador;
  }
};

int Animal::contador = 0;
int Animal::registro = 0; 
mutex Animal::mutex_;

class Leao : public Animal {
public:
  Leao() : Animal() {}

  void comer() {

    int qtdComida = Util::random(2, 5);
    while (comedouroLeoes < qtdComida) {
      this_thread::sleep_for(chrono::milliseconds(1));
      // caso nao haja comida suficiente, ele fica esperando até o veterinario
      // colocar mais comida
    }
    unique_lock<mutex> lock(mutex_);
    cout << "Leão comendo... " << qtdComida << " (Leão < " << id << " >)"
         << endl;
    comedouroLeoes = comedouroLeoes - qtdComida;
    registro = registro + qtdComida;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(1));
  }

  void exibir() {

    unique_lock<mutex> lock(mutex_);
    cout << "Exibindo a Juba..        (Leão < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(1));
  }

  void dormir() {

    unique_lock<mutex> lock(mutex_);
    cout << "Dormindo...              (Leão < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::seconds(Util::random(1, 2)));
    cout << "Acordei...               (Leão < " << id << " >)" << endl;
  }
};

class Avestruz : public Animal {
public:
  Avestruz() : Animal() {}

  void comer() {

    unique_lock<mutex> lock(mutex_);
    cout << "Avestruz comendo...      (Avestruz < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(1));
  }

  void exibir() {

    unique_lock<mutex> lock(mutex_);
    cout << "Exibindo o pescoço...    (Avestruz < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(1));
  }

  void dormir() {

    unique_lock<mutex> lock(mutex_);
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

    unique_lock<mutex> lock(mutex_);
    cout << "Suricato comendo...      (Suricato < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(1));
  }

  void exibir() {

    unique_lock<mutex> lock(mutex_);
    cout << "Suricato se exibindo...  (Suricato < " << id << " >)" << endl;
    lock.unlock();

    this_thread::sleep_for(chrono::milliseconds(1));
  }

  void dormir() {

    unique_lock<mutex> lock(mutex_);
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
    while (true) {

      if (comedouroLeoes == 30) {
        continue;
      } else {
        unique_lock<mutex> lockLeoes(mutexComedouroLeoes);
        comedouroLeoes++;
        lockLeoes.unlock();

        unique_lock<mutex> lockEstoqueCarne(mutexEstoqueCarne);
        estoqueCarne--;
        lockEstoqueCarne.unlock();

        cout << "Veterinario " << id << " moveu 1 carne para o comedouro.";
      }
      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }

  void viver() {
    entregarComida();
  }
};

int Veterinario::contador = 0;

class Fornecedor {
  void abastecer() {
    while (true) {
      if (estoqueLarvas == 0 || estoquePasto == 0 || estoqueCarne == 0) {
        estoqueLarvas = 30;
        estoquePasto = 30;
        estoqueCarne = 30;
        cout << "Reestocagem: tudo cheio.";
      }
    }
  }
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

  return 0;
}