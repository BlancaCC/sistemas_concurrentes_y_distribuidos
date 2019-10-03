#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//**********************************************************************

// número consumidores productores

const int num_consumidores = 3;
const int num_productores = 3;

// variables compartidas

const int num_items = 10 ,   // número de items
	       tam_vec   = 10 ;   // tamaño del buffer
unsigned  cont_prod[num_items*num_productores] = {0}, // contadores de verificación: producidos
          cont_cons[num_items*num_consumidores] = {0}; // contadores de verificación: consumidos

// IMPLEMENTACIÓN LIFO

Semaphore puede_escribir_lifo = tam_vec;
Semaphore puede_leer_lifo = 0;

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


//_________ implementación FIFO ______
Semaphore puede_escribir_fifo = tam_vec;
Semaphore puede_leer_fifo = 0;

Semaphore zona_critica_consumidores_fifo = 1;
Semaphore zona_critica_productores_fifo= 1; 
//cola sin comprobaciones
struct Cola {
  int v[tam_vec];
  int ini=-1; //última posición leída
  int fin=0; // apunta a la posición a añadir

  void escribir( int n) {
    v[fin] =n;
    fin = (fin+1)%tam_vec;     
  }
  int leer() {
    ini = (ini+1)%tam_vec;
    return v[ini]; 
  }
};
Cola cola; 

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
  assert( dato < num_items*num_consumidores || dato < num_items*num_productores );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << " consumido: " << dato << endl ;

}


//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items*num_productores ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
   }
   for( unsigned i = 0 ; i < num_items*num_consumidores ; i++ )
   {  
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
      
      
      sem_wait( puede_escribir_lifo ); //queda espacio en la pila
      
      sem_wait( zona_critica); //el consumidor no está en la zona crítica       
      //!M tocando_pila.lock();
      int dato = producir_dato() ;
      pila.escribir(dato); //escribimos dato en la pila común 
      
      //!Mtocando_pila.unlock();
      
      sem_signal( zona_critica); 
      sem_signal( puede_leer_lifo);
      
   }
}
void funcion_hebra_consumidora_lifo(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato ;

      
      sem_wait( puede_leer_lifo); //hay datos escritos

      sem_wait ( zona_critica);
      dato= pila.leer();
      consumir_dato( dato ) ;

      sem_signal( zona_critica); 

      sem_signal( puede_escribir_lifo); 

      
      
    }
}
//----------------------------------------------------------------------
//--------------------------- FIFO  --------------------------------
//----------------------------------------------------------------------

void funcion_hebra_consumidora_fifo(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato ;

      
      sem_wait( puede_leer_fifo); //hay datos escritos
      sem_wait( zona_critica_consumidores_fifo); 
      dato= cola.leer();
      consumir_dato( dato ) ;
      sem_signal( zona_critica_consumidores_fifo); 
      sem_signal( puede_escribir_fifo); 

      
      
    }
}
void  funcion_hebra_productora_fifo(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      
      
      sem_wait( puede_escribir_fifo ); //queda espacio en la cola           

       sem_wait( zona_critica_productores_fifo); 
      int dato = producir_dato() ;
      cola.escribir(dato); //escribimos dato en la pila común
      sem_signal( zona_critica_productores_fifo); 
      sem_signal( puede_leer_fifo);
      
   }
}

//----------------------------------------------------------------------

int main( int argc, char * argv[])
{
  if (argc > 1 ) { //  Si se ha escrito algo por argumentos 
   cout << "--------------------------------------------------------" << endl
        << "Problema de los VARIOS productores-consumidores (solución LIFO)." << endl
        << "--------------------------------------------------------" << endl
        << flush ;

   thread hebra_productora[num_productores],hebra_consumidora[num_consumidores];
    
    for(int i=0; i< num_productores; i++)
      hebra_productora[i]= thread( funcion_hebra_productora_lifo );
    for(int i=0; i< num_consumidores; i++)
      hebra_consumidora[i]=thread( funcion_hebra_consumidora_lifo );
    
    for(int i=0; i< num_productores; i++)
      hebra_productora[i].join() ;
    for(int i=0; i< num_consumidores; i++)
      hebra_consumidora[i].join() ;
  
   test_contadores();

  }
  else {
    cout << "--------------------------------------------------------" << endl
	 << "Problema de VARIOS  productores-consumidores (solución FIFO)." << endl
	 << "--------------------------------------------------------" << endl
	 << flush ;

    thread hebra_productora[num_productores],hebra_consumidora[num_consumidores];
    
    for(int i=0; i< num_productores; i++)
      hebra_productora[i]= thread( funcion_hebra_productora_fifo );
    for(int i=0; i< num_consumidores; i++)
      hebra_consumidora[i]=thread( funcion_hebra_consumidora_fifo );
    
    for(int i=0; i< num_productores; i++)
      hebra_productora[i].join() ;
    for(int i=0; i< num_consumidores; i++)
      hebra_consumidora[i].join() ;

    test_contadores();
  }
  return 0; 
}
