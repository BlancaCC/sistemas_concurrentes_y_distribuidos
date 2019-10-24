#include <iostream>
#include <iomanip>
#include "HoareMonitor.h"
#include <cassert>
#include <random>

using namespace std ;
using namespace HM ;

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//si hay dos estanqueras
int generarIngrediente (const int maxIngrediente=2){

  int ingrediente = aleatorio< 0 , 2>();
 
  return ingrediente; 
}

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

class Estanco : public HoareMonitor {

private:
  CondVar *CVfumadores;
  CondVar CVestanquero;
  int mostrador; 

public:
  Estanco(int n_fumadores) {
    CVfumadores=new CondVar[n_fumadores];
    mostrador=-1;
    
    //incializamos variables condición 
    CVestanquero = newCondVar();
    for(int i=0; i<n_fumadores; i++)
      CVfumadores[i] = newCondVar();
  }
  ~Estanco() { delete [] CVfumadores;}

  void ponerIngrediente(int i) {
    cout << "Se pone ingrediente " << i<< endl; 
    mostrador=i;
    if( !CVfumadores[i].empty())
      CVfumadores[i].signal(); 
  }

  void esperarRecogidaIngrediente() {
    cout << "Se espera ingrediente "<< endl;
    if( mostrador != -1)
      CVestanquero.wait();
    cout << "Se termian de esperar la recogida del ingrediente " << endl; 
  }


  void obtenerIngrediente(int id_fumador){
    if( mostrador != id_fumador)
      CVfumadores[id_fumador].wait();

    mostrador =-1; 
    cout << "Fumador " << id_fumador << "recoge ingrediente " << endl;
    //if(!CVestanquero.empty())
    CVestanquero.signal();
  }
};


void Fumador(MRef<Estanco>monitor, int id){
  while(true){
    monitor->obtenerIngrediente(id);
    fumar(id); 
  }
}

void Estanquero(MRef<Estanco> monitor){
  while(true){
    monitor->ponerIngrediente(generarIngrediente());
    monitor->esperarRecogidaIngrediente();
  }
}

int main() {

  cout << "Programa fumador" << endl;
  const int numFumadores=3;
  
  MRef<Estanco> monitor = Create<Estanco>(numFumadores);

  thread tEstanquero(Estanquero, monitor);
  thread tFumadores[numFumadores];
  
  for(int i=0; i<numFumadores; i++){
    tFumadores[i] = thread(Fumador, monitor, i);
  }

  tEstanquero.join(); 
  
}
