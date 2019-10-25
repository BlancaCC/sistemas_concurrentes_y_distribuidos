#include <iostream>
#include <cassert>
#include <random>
using namespace std;


// -------- Funciones de la plantilla ---- 

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

void sleep_this_thread() {
  this_thread::sleep_for( chrono::milliseconds( aleatorio<100,200>() ));
}
//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------
/**
int producir_dato()
{
   static int contador = 0 ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   //cout << "producido: " << contador << endl << flush ;

   cont_prod[contador] ++ ;
   return contador++ ;
}
//----------------------------------------------------------------------

void consumir_dato( unsigned dato )
{
  //assert( dato < N_DATOS );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
   //if (cont_cons[dato] >1) cout << "ERROR(id.dato)"<< this_thread::get_id()<<dato<< endl; 
   //   cout << "\t\tconsumido(" <<this_thread::get_id() << ") "<< dato << endl ;

}
void consumir_dato( unsigned dato, int id )
{
  //assert( dato < N_DATOS );
   cont_cons[dato] ++ ;
  this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
  //if (cont_cons[dato] >1) cout << "ERROR(id.dato)"<< id<<dato<< endl; 
   //cout << "\t\tconsumido(" <<id << ") "<< dato << endl ;

}


//----------------------------------------------------------------------

void test_contadores( int N_DATOS)
{
//---------------------- mis productores y consumidores --------------

   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < N_DATOS ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces" << endl ;
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


*/
