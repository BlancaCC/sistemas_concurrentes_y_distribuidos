/**
   @autor Blanca Cano  Camarero 
   
   @brief: un proveedor suministra a varios estanqueros que comparten un mostrador del que consumen varios fumadores 
 */

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <vector>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;


//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}


const int numEstanqueros= 2, numFumadores=4; //cada fumador consume un tipo concreto de producto, un estanquero puede vender cualquiera
//inicializamos estanqueros y fumadores sin suministros
  vector<Semaphore> estanquero ( numEstanqueros, Semaphore(0));
  vector<Semaphore> sFumador ( numFumadores, Semaphore(0));
mutex mtxCout; 

class Monitor {

private:

  //static int num_suministros = 5; //número de elementos quue le da al estanquero
  //inicializamos estanqueros y fumadores sin suministros
  //vector<Semaphore> estanquero ( numEstanqueros, Semaphore(0));
  //vector<Semaphore> fumador ( numFumadores, Semaphore(0));
  Semaphore sReponer = 1; //al comenzar se repone a un estanquero
  Semaphore sMostrador = 1; //mostrador vacío

  int elementosConsumidos = 0; //número de elementos que han consumido 
  const int numSuministros = 2;    // número de elementos que se recargan tras cada gestion
  const int finGestor = 3;  //número de elementos gestionados a los que se acaba

  mutex mtxElementosConsumidos;
  
public:

  /**
     Repone a un estanquero aleatorio @param num_suministros un número concreto de elementos
   */
  void reponer( ) {

    sem_wait( sReponer );
    
    int n_estanquero = aleatorio <0,(numEstanqueros-1)>();

    //tiempo que tarda de gestion
     chrono::milliseconds duracion_reponer( aleatorio<20,200>() );
     mtxCout.lock();
     cout << "GESTOR: " <<  duracion_reponer.count()
	  << "ms reponer al estanquero " << n_estanquero << endl;
     mtxCout.unlock();
     this_thread::sleep_for( duracion_reponer);

     //damos suministros al estanquero
     for (int i=0; i< numSuministros; i++)
       sem_signal(estanquero[n_estanquero] );   
  }

  /**
     @brief el estanquer @param n_estanquero repone en el mostrado
   */
  void poner( int n_estanquero) {
    
    sem_wait( estanquero[n_estanquero]); // tiene sumisnistros 
    sem_wait( sMostrador); // comprobamos que esté vacío

    int elemento = aleatorio <0,numFumadores-1>();

    //tiempo que tarda en reponer
     chrono::milliseconds duracion_poner( aleatorio<20,200>() );
     this_thread::sleep_for( duracion_poner);
     mtxCout.lock();
     cout << " \tMOSTRADOR (estanquero "<< n_estanquero<<") :" <<  duracion_poner.count()
	  << "ms el elemento " << elemento << " en el mostrador "<< endl;
     mtxCout.unlock();
     
     //activamos fumador correspondiente
     sem_signal( sFumador[elemento] );
     
     //veamos si se necesita volver a reponer
     mtxElementosConsumidos.lock(); //zona crítica
     if ( ++elementosConsumidos % numSuministros == 0 ) {
       sem_signal( sReponer); 
     }
     mtxElementosConsumidos.unlock(); 
    
  }

  void fumador ( int n_fumador) {

    sem_wait ( sFumador[n_fumador]); // hay en el mostrador algo para él

    // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar
   mtxCout.lock();
   cout << "\t\t FUMAR: Fumador " << n_fumador << "  :"
	 << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl<< endl;
   mtxCout.unlock();
   sem_signal( sMostrador);
   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );
   
	       
  }

  /**
     @brief Determina si se ha cumplido cierta condición 
   */

  bool fin() {
    //NUNCA ACABA 
    /**
    mtxElementosConsumidos.lock(); 
    bool ret = !( elementosConsumidos < finGestor );
    mtxElementosConsumidos.unlock();
    */
    return false; 
  }
  
}; 

Monitor gestor;

//----- funciones  hebra --------

void funcion_hebra_fumador( int n) {
  while ( !gestor.fin()) {
    gestor.fumador( n); 
  }
  mtxCout.lock();
  cout << "\t\t\tFIN fumador " << n << " se cansa de fumar"<< endl; 
  mtxCout.unlock();
}

void funcion_hebra_estanquero( int id ) {
  while (!gestor.fin())
    gestor.poner( id);
  mtxCout.lock();
  cout << "\t\t\tFIN Estanquero "<< id << " ya no le quedan recursos"<< endl; mtxCout.unlock();
}

void funcion_hebra_gestor() {
  while (!gestor.fin() )
    gestor.reponer();
  mtxCout.lock();
  cout << "\t\t\tFIN El gestor ya no tiene más abastos" << endl;
  mtxCout.unlock();
}
  

// ------------- main --------------


int main() {

  //creamos hebras 
  thread hGestor ( funcion_hebra_gestor);

  thread hEstanquero[ numEstanqueros],hFumadores[numFumadores];
  for ( int i=0; i<numEstanqueros; i++)
    hEstanquero[i] = thread( funcion_hebra_estanquero, i);

   for ( int i=0; i<numFumadores; i++)
    hFumadores[i] = thread( funcion_hebra_fumador, i);

   //esperamos hebras
   for ( int i=0; i<numFumadores; i++)
     hFumadores[i].join();
   
   for ( int i=0; i<numEstanqueros; i++)
     hEstanquero[i].join(); 
  hGestor.join(); 
}
