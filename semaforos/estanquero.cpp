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

mutex mtxCout; 

class Monitor {

private:

  //inicializamos estanqueros y fumadores sin suministros
  Semaphore sReponer = 1; //al comenzar se repone a un estanquero
  Semaphore sMostrador = 1; //mostrador vacío
  Semaphore estanquero[numEstanqueros]={0,0};
  Semaphore sFumador[numFumadores]=  {0,0,0,0}; 

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
     cout << "\nGESTOR( " <<  duracion_reponer.count()
	  << "ms) repone al estanquero " << n_estanquero
	  << "con " << numSuministros<< "elementos"<<endl << flush;
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

    int elemento = aleatorio <0,(numFumadores-1)>();

    //tiempo que tarda en reponer
     chrono::milliseconds duracion_poner( aleatorio<20,200>() );
     this_thread::sleep_for( duracion_poner);
     mtxCout.lock();
     cout << " \tMOSTRADOR estanquero "<< n_estanquero<<") " <<  duracion_poner.count()
	  << "ms coloca elemento " << elemento << " en el mostrador "<< endl<<flush;
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

    mtxCout.lock();
    cout << "fumador " << n_fumador << "coge objeto del mostrador" << endl; 
    sem_signal( sMostrador);
    mtxCout.unlock();
    
    // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar
   mtxCout.lock();
   cout << "\t\t FUMAR: Fumador " << n_fumador 	<< " empieza a fumar"<< endl;
   mtxCout.unlock();
   
   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );
    mtxCout.lock();
   cout << "\t\tFumador " << n_fumador 	<< " ha terminado fumar ("
	<<duracion_fumar.count()<< " ms)"<< endl;
   mtxCout.unlock();
   
	       
  }

  /**
     @brief Determina si se ha cumplido cierta condición 
   */

  bool fin() {
    //NUNCA ACABA  ret = false
    /**
    mtxElementosConsumidos.lock(); 
    bool ret = !( elementosConsumidos < finGestor );
    mtxElementosConsumidos.unlock();
    */
    bool ret = false; 
    return ret; 
  }
  
}; 

Monitor gestor;

//----- funciones  hebra --------

void funcion_hebra_fumador( int id) {
  while ( !gestor.fin()) {
    gestor.fumador(id); 
  }
  mtxCout.lock();
  cout << "\t\t\tFIN fumador " << id << " se cansa de fumar"<< endl; 
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
   /**
   for ( int i=0; i<numFumadores; i++)
     hFumadores[i].join();
   
   for ( int i=0; i<numEstanqueros; i++)
     hEstanquero[i].join(); 
   */
  hGestor.join(); 
}
