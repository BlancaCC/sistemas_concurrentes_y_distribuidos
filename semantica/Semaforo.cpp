/**
   @brief construcción de un semáforo utilizando montores 
 */
#include <iostream>
#include <condition_variable>

usign namespace std;

class Semaforo {
  int cntSignal;
  

  Semaforo(int n) :signal(n){}

  void Signal( ) {
    cntSignal++;

    if( !cola.empty()) {
      cola.signal(); 
    }
  }
  void Wait() {
    if  (signal == 0) {
      cola.wait();
      
    }

    valor
  }
}; 
