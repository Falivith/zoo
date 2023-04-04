#include <fstream>
#include <iostream>
#include <thread>

using namespace std;

class animal {
	public:
		void comer(int id){
			cout << "Comendo... " << id << endl;
		}

		void exibir(int id){
			cout << "Se exibindo... " << id << endl;
		}

		void dormir(int id){
			cout << "Dormindo... " << id << endl; 	
		}
}

void leao (int leaoID){
	

void threadFunction(int id) {
	cout << "Thread " << id << " started" << endl;
	for(int i = 0; i < 10; i++){
		cout << i << endl;
	}
	cout << "Thread " << id << " ended" << endl;
}

int main () {
	const int N_THREADS = 4;
       	thread threads[N_THREADS];

	for(int i = 0; i < N_THREADS; i++) {
		threads[i] = thread(threadFunction, i);
	}

	for(int i = 0; i < N_THREADS; i++) {
		threads[i].join();
	}

	cout << "All the threads ended." << endl;
    return 0;
}
