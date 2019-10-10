#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//**********************************************************************
// variables compartidas

const int num_items = 40 ,   // número de items
	       tam_vec   = 5 ;   // tamaño del buffer
unsigned  cont_prod[num_items] = {0}, // contadores de verificación: producidos
          cont_cons[num_items] = {0}; // contadores de verificación: consumidos


// IMPLEMENTACIÓN LIFO

Semaphore puede_escribir_lifo = tam_vec; //queda espacio en el buffer
Semaphore puede_leer_lifo = 0; //hay elementos para leer en el buffer

// para proteger la zona crítica puede usarse un semáforo o mutex
// se va a optar por semáforos así que se comentarán los mutex con la siguiente nomenclatura //!M 
Semaphore zona_critica = 1;

//!M mutex tocando_pila;

struct Pila {
  
  int p [ tam_vec ];
  int ind=0; //apunta siempre a la última posición vacía 
  const int ERROR=-1;
  const int OK=0; 

  int escribir ( int n) {
    if ( ind < tam_vec ) {
      p [ind++]=n;
      return OK;
    }
    else return ERROR;
  }

  int leer() { return (ind > 0 )?p[ --ind] : ERROR ;    }
  
};

Pila pila; 


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

//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------

int producir_dato()
{
   static int contador = 0 ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "producido: " << contador << endl << flush ;

   cont_prod[contador] ++ ;
   return contador++ ;
}
//----------------------------------------------------------------------

void consumir_dato( unsigned dato )
{
   assert( dato < num_items );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "\t\tconsumido: " << dato << endl ;

}


//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {  cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

//----------------------------------------------------------------------

void  funcion_hebra_productora_lifo(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
     //PREGUNTAR DIFERENCIA ENTRE PRODUCIR DATO AQUÍ O DENTRO DE LA ZONA CRÍTICA
      int dato = producir_dato() ;
      sem_wait( puede_escribir_lifo ); //queda espacio en la pila
      
      sem_wait( zona_critica); //el consumidor no está en la zona crítica       
      //!M tocando_pila.lock();
      
      //int dato = producir_dato() ;
      pila.escribir(dato); //escribimos dato en la pila común 
      
      //!Mtocando_pila.unlock();
      
      sem_signal( zona_critica); 
      sem_signal( puede_leer_lifo);
      
   }
}
void funcion_hebra_consumidora_lifo(  )
{
  int dato ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  
      sem_wait( puede_leer_lifo); //hay datos escritos

      sem_wait ( zona_critica);
      dato= pila.leer();
      consumir_dato( dato ) ;

      sem_signal( zona_critica); 

      sem_signal( puede_escribir_lifo); 

      
      
    }
}

//----------------------------------------------------------------------

int main( )
{

  cout << "--------------------------------------------------------" << endl
       << "Problema de los productores-consumidores (solución LIFO)." << endl
       << "--------------------------------------------------------" << endl
       << flush ;

  thread hebra_productora ( funcion_hebra_productora_lifo ),
    hebra_consumidora( funcion_hebra_consumidora_lifo );

  hebra_productora.join() ;
  hebra_consumidora.join() ;

  test_contadores();


  return 0; 
}
