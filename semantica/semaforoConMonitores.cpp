/**
   @brief construcción de un semáforo utilizando montores 
 */
#include <iostream>
#include <condition_variable>

using namespace std;

class Semaforo {
  
  int cntSignal;
  condition_variable cola; 

public: 
  Semaforo(int n) :cntSignal(n){}

  void sem_signal( ) {
  
    if( !cola.empty()) {
      cola.notify_one();   
    }
    else
      cntSignal++;
      
  }
  
  void sem_wait() {
    if (cntSignal == 0) {
      cola.wait();
      
    }
    else
      cntSignal--; 
  }
};



int main() {

  Semaforo s(2);

  s.sem_signal();
  s.sem_wait();
  
  return 0; 
}
