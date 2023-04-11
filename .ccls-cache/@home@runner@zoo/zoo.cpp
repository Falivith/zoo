#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

#define num_leoes 2
#define num_avestruzes 2
#define num_suricatos 2

using namespace std;

class Animal {
protected:
  static int contador;
  int id;

public:
  Animal() {
    contador++;
    id = contador;
  }

  void virtual comer() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Comendo... " << id << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void virtual exibir() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Se exibindo... " << id << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void virtual dormir() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Dormindo... " << id << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void viver() {
    comer();
    exibir();
    comer();
    dormir();
  }

protected:
  static mutex mutex_;
};

int Animal::contador = 0;

mutex Animal::mutex_;

class Leao : public Animal {
public:
  Leao() : Animal() {}

  void comer() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Comendo comida de Leão...(Leão < " << id << " >)" << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void exibir() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Exibindo a Juba..(Leão < " << id << " >)" << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void dormir() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Dormindo... (Leão < " << id << " >)" << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
};

class Avestruz : public Animal {
public:
  Avestruz() : Animal() {}

  void comer() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Comendo comida de Avestruz...(Avestruz < " << id << " >)" << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void exibir() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Exibindo o pescoço..(Avestruz < " << id << " >)" << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void dormir() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Dormindo... (Avestruz < " << id << " >)" << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
};

class Suricato : public Animal {
public:
  Suricato() : Animal() {}

  void comer() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Comendo comida de Suricato...(Suricato < " << id << " >)" << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void exibir() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Exibindo a carinha de suricato..(Suricato < " << id << " >)"
         << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  void dormir() {
    std::unique_lock<std::mutex> lock(mutex_);
    cout << "Dormindo... (Suricato < " << id << " >)" << endl;
    lock.unlock(); // faz o unlock manualmente
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
};

int main() {

  Leao leoes[num_leoes];
  Suricato suricatos[num_suricatos];
  Avestruz avestruzes[num_avestruzes];
  thread threads[num_suricatos + num_leoes + num_avestruzes];

  int id = 0;
  
  for (int i = 0; i < num_leoes; i++) {
      threads[i] = thread(&Animal::viver, &leoes[id]);
      id++;
  }

  id = 0;

  for (int i = num_leoes; i < num_suricatos + num_leoes; i++) {
      threads[i] = thread(&Animal::viver, &suricatos[id]);
  }

  id = 0;
  
  for (int i = num_suricatos + num_leoes; i < num_suricatos + num_leoes +
  num_avestruzes; i++) { threads[i] = thread(&Animal::viver, &avestruzes[id]);
  }

  for (int i = 0; i < num_suricatos + num_leoes + num_avestruzes; i++) {
      threads[i].join();
  }

  return 0;
}