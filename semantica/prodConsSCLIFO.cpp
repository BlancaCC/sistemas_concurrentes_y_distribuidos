#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include <condition_variable>


using namespace std;


const int N_CONSUMIDORES=3, N_PRODUCTORES=2;
const int TAM_BUFFER=3;
// variables compartidas

const int N_DATOS = 40;    // número de items
unsigned  cont_prod[N_DATOS] = {0}, // contadores de verificación: producidos
  cont_cons[N_DATOS] = {0}; // contadores de verificación: consumidos

class ProdConsSCLIFO {

private:
  //estructura LIFO
  int pila[TAM_BUFFER]; 
  int primeraLibre=0; 

  //variables permanentes para acceso y control
  mutex mtxPila, mtxProductores, mtxConsumidores;
  condition_variable colaProductor, colaConsumidor;
  int cntProductores, cntConsumidores; 
  
  

public:

  void  insertar( int dato, int nHebra) {

    //tener cuidado con la variable primeraLibre
    while ( primeraLibre ==TAM_BUFFER) {
      wait(colaProductor);
    }
    
    mtxPila.lock();
    pila[primeraLibre++]=dato;
    cout << "Productor " << nHebra << " escribe " << dato << endl;
    mtxPila.unlock();

    notify_one(sConsumir); 
  }

  int extraer( int nHebra) {

    while (primeraLibre == 0) {
      wait( colaConsumidores);
    }

    mtxPila.lock();
    int dato=pila[--primeraLibre]; 
    cout << "\t\t\tConsumidor " << nHebra << " extrae" << dato << endl; 
    mtxPila.unlock();
    
    notify_one(colaProductores);

    return dato; 
    
  }

  /**
     @brief nos proporciona si quedan datos por producir
   */
  bool puedoPoducir() {

    unique_lock<mutex>(mtxProductores);
    bool ret=false;
    
    if( cntProductores < N_DATOS) {
      ret=true;
      cntProductores++; 
    }
    return ret; 
  }
  
  bool puedoConsumir() {
    
    bool ret=false;
    unique_lock<mutex>(mtxConsumidores);
    if( cntConsumidores <N_DATOS) {
      ret=true;
      cntConsumidores++;
    }
    return ret; 
  }
  
};


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
   assert( dato < N_DATOS );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "\t\tconsumido: " << dato << endl ;

}


//----------------------------------------------------------------------

void test_contadores()
{
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



//---------------------- mis productores y consumidores --------------

void Consumidor ( int id, ProdConsSCLIFO & monitor) {
  while( monitor.puedoConsumir()) {
    
    consumir_dato ( monitor.extraer() ); 
  }
}

void Productor( int id, ProdConsSCLIFO & monitor) {

  while( monitor.puedoProducir() ) {
    monitor.escribir( poducir_dato());
  }
}


//------- main----

int main() {
    cout << "------------------------------------------------------------------------------------" << endl
	 << "  Problema de los productores-consumidores (solución LIFO con monitor y semántica SC)" << endl
	 << "Nº de ítems: "<< N_DATOS << " | Nº consumidores:" << N_CONSUMIDORES << " | Nº productores" << N_PRODUCTORES << endl
	 << "--------------------------------------------------------------------------------------" << endl
       << flush ;

  ProdConsSCLIFO m;
  thread tConsumidores[N_CONSUMIDORES],tProductores[N_CONSUMIDORES];

  for(int i=0; i<N_CONSUMIDORES, i++)
    tConsumidores[i]=thread(Cosumidor, i,m);
  
  for(int i=0; i<N_PRODUCTORES, i++)
    tConsumidores[i]=thread(Productor, i,m);

  //los consumidores acabaran después de los productores, por tanto solo bastará esperarlos a ellos
  for(int i=0; i<N_CONSUMIDORES, i++)
    tConsumidores[i].join();


  
  return 0; 
}
