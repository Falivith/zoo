#include <chrono> // Função SLEEP
#include <fstream>
#include <iostream>
#include <mutex>
#include <random> // Sorteio de quantas horas serão dormidas
#include <thread>
#include <condition_variable>

#define N_LEOES 2
#define N_AVESTRUZES 2
#define N_SURICATOS 2

using namespace std;

mutex mtx;  // Mutex para sincronizar o acesso ao tempo (relógio global)
condition_variable cv;  // variável condicional para notificar outras threads

int tempo = 0;  // tempo em horas

class Util {
public:
  static int random(int botLimit, int topLimit) {
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 rng(seed);
    uniform_int_distribution<> distrib(botLimit, topLimit);
    return distrib(rng);
  }
};

class Animal {
protected:
  static mutex mutex_;
  static int contador;
  int id;

public:
  void virtual comer() {}
  void virtual exibir() {}
  void virtual dormir() {}

  void viver() {
    while(true){
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
mutex Animal::mutex_;

class Leao : public Animal {
public:
  Leao() : Animal() {}

  void comer() {

    unique_lock<mutex> lock(mutex_);
    cout << "Leão comendo...          (Leão < " << id << " >)" << endl;
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

int main() {

  Animal *animais[N_LEOES + N_SURICATOS + N_AVESTRUZES];
  const int N_THREADS = N_LEOES + N_SURICATOS + N_AVESTRUZES;

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

  thread threads[N_THREADS];

  // Lança a rotina viver para todos animais

  for (int i = 0; i < N_THREADS; i++) {
    threads[i] = thread(&Animal::viver, animais[i]);
  }

  for (int i = 0; i < N_THREADS; i++) {
    threads[i].join();
  }

  for (int i = 0; i < N_THREADS; i++) {
    delete animais[i];
  }

  return 0;
}