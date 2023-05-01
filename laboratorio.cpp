#include <iostream>
#include <queue>
#include <thread>
#include <chrono>

std::queue<std::thread> queue_;

void access_resource() {
    // cria uma thread com o ID atual e a adiciona na fila
    std::thread t([](){ std::cout << "Thread " << std::this_thread::get_id() << " acessando recurso..." << std::endl; });
    queue_.push(std::move(t));
    
    // faz a thread dormir um tempo aleatório
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
    
    // remove a thread da fila e a executa
    std::thread t2 = std::move(queue_.front());
    queue_.pop();
    t2.join();
}

int main() {
    // cria 5 threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.emplace_back(access_resource);
    }

    // aguarda a conclusão das threads
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}