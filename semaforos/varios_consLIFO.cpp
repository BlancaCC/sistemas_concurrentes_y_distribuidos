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
const int num_productores = 4;

// variables compartidas

const int num_items = 20 ,   // número de items
	       tam_vec   = 5 ;   // tamaño del buffer
unsigned  cont_prod[num_items] = {0}, // contadores de verificación: producidos
          cont_cons[num_items] = {0}; // contadores de verificación: consumidos

// IMPLEMENTACIÓN LIFO

Semaphore puede_escribir_lifo = tam_vec;
Semaphore puede_leer_lifo = 0;

struct Pila {
  
  int p [ tam_vec ];
  int ind=0; //apunta siempre a la última posición vacía 
  const int ERROR=-1;
  const int OK=0;
  //mutex mtxPila;  reduntante por la existencia de mtxlifo

  int escribir ( int n) {
    int ret = ERROR;
    // mtxPila.lock(); 
    if ( ind < tam_vec ) {
      p [ind++]=n; 
      ret=  OK;
    }
    //mtxPila.unlock();
    return ret; 
  }

  int leer() {
    // mtxPila.lock(); 
    int ret=(ind > 0 )?p[ --ind] : ERROR ;
    //mtxPila.unlock();
    return ret; 
  }
  
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

mutex producir_dato_ms; 
int producir_dato()
{
  this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

  producir_dato_ms.lock(); 
   static int contador = 0 ;
   cout << "producido: " << contador << endl << flush ;

   cont_prod[contador] ++ ;
   int ret = contador++ ;
   producir_dato_ms.unlock();
   
   return ret; 
}
//----------------------------------------------------------------------

//no hace falta añaidr un mutex para el flujo y representación ya que consumir_dato en está en la zona crítica del consumidor_lifo
void consumir_dato( unsigned dato )
{
  assert( dato < num_items*num_consumidores || dato < num_items*num_productores );
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
   }
   for( unsigned i = 0 ; i < num_items ; i++ )
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
mutex mtxQuedanDatosProducir;
int datos_producir = 0;

bool quedanDatosProducir() {
  bool ret = false;
  
  mtxQuedanDatosProducir.lock();
  if( datos_producir < num_items )  {
    datos_producir++;
    ret = true;
  }
  mtxQuedanDatosProducir.unlock();
  
  return ret; 
}
mutex mtxlifo;  //cerrojo no se puede escribir o consumir si no se ha terminado de una de esas tarean anterior
void  funcion_hebra_productora_lifo(  )
{
  while( quedanDatosProducir() )
   {
      
      sem_wait( puede_escribir_lifo ); //queda espacio en la pila

      //int dato = producir_dato() ;
      mtxlifo.lock(); 
      pila.escribir(producir_dato()); //escribimos dato en la pila común 
      mtxlifo.unlock(); 
 
      sem_signal( puede_leer_lifo); // un nuevo dato pendiente de consumir
      
   }
}

mutex mtxQuedanDatosConsumir;
int datos_consumir = 0; 
bool quedanDatosConsumir() {
  bool ret = false; 
  mtxQuedanDatosConsumir.lock();
  if( datos_consumir < num_items )  {
    datos_consumir++;
    ret = true;
  }
  mtxQuedanDatosConsumir.unlock();
  return ret; 
}

void funcion_hebra_consumidora_lifo(  )
{
  while( quedanDatosConsumir())
   {
      int dato ;
      sem_wait( puede_leer_lifo); //hay datos escritos

    
      mtxlifo.lock();  // zona crítica  de lectura y consumición lifo
      consumir_dato( pila.leer() ) ; // consumición 
      mtxlifo.unlock(); 

      sem_signal( puede_escribir_lifo);  //marcamos que se ha liberado un hueco

      
      
    }
}

//----------------------------------------------------------------------

int main()
{
  
  cout << "--------------------------------------------------------" << endl
       << "Problema de los VARIOS productores-consumidores (solución LIFO)." << endl
           << "nº productores: " << num_productores << endl
       << "nº consumidores: " << num_consumidores << endl
       << "nº datos a producir: " << num_items << endl
       << " tamaño buffer: " << tam_vec << endl
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

  

  return 0; 
}
