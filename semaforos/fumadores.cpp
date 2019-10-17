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


// ************* VARIABLES COMPARTIDAD ***************

const int numFumadores = 3;
Semaphore mostrador_disponible = 1;
const int maxIngrediente = numFumadores -1; 

//fumadores esperando 
//vector <Semaphore> fumador ( numFumadores,  Semaphore(0));
Semaphore fumador[ numFumadores] = {0,0,0}; 


  




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

//----------------------------------------------------------------------
// Función generar ingrediente

int generarIngrediente () {

  int ingrediente = aleatorio< 0 , maxIngrediente>();
  sem_signal(fumador[ingrediente] );

  return ingrediente; 
}
//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(  )
{
  while (true) {
    sem_wait (mostrador_disponible);
    cout << "Se coloca el ingrediente " << generarIngrediente() << endl;
  }

}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

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

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {
     sem_wait (fumador[ num_fumador]);
     cout << "Fumador " << num_fumador << "coge objeto " << endl; 
     sem_signal(mostrador_disponible); 
     fumar (num_fumador);
   }
}

//----------------------------------------------------------------------

int main()
{
  thread estanquero_hebra (funcion_hebra_estanquero);
  thread fumador_hebra[ numFumadores ];

  for (int i=0; i<numFumadores ; i++ )
    fumador_hebra[i] = thread ( funcion_hebra_fumador, i );

  estanquero_hebra.join(); //no hace falra esperar a las otras 
}
