#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;



const int num_items = 40,
  tam_vec = 5,
  num_productores= 2,
  num_consumidores=5; 

unsigned  cont_prod[num_items] = {0}, // contadores de verificación: producidos
          cont_cons[num_items] = {0}; // contadores de verificación: consumidos

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

mutex mtxProductor;
int producirDato(int n_hebra)
{
  this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

  mtxProductor.lock(); 
  static int contador = 0 ;
  cout << "hebra "<<  n_hebra <<" produce: " << contador << endl << flush ;
  cont_prod[contador] ++ ;
  int ret = contador++;
  mtxProductor.unlock();
  
   return ret ;
}
//----------------------------------------------------------------------

mutex mtxConsumidor; 
void consumirDato( unsigned dato, int n_hebra )
{
  
    assert( dato < num_items); //matamos esta hebra
    this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    //hemos comentado esto ya que como queremos que sea fifo tanto a la hora de consumir como de mostrar mensaje lo hemos metido en la misma zona crítica que la lectura :D
    // mtxConsumidor.lock();
    cont_cons[dato] ++ ;
    cout << "\t\t\t hebra  "<< n_hebra<< " consume: " << dato << endl ;
    // mtxConsumidor.unlock(); 
   
  
}


//----------------------------------------------------------------------

void test_contadores( )
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items; i++ )
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


//cola sin comprobaciones

mutex mtxEscribirCola, mtxLeerCola;
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


//____ manejo concurrencia ______

int datosEscritos = 0;
mutex mtxEscritos;
Semaphore bufferEscritura= tam_vec;
Semaphore bufferLectura = 0; 

bool quedanDatosEscribir() {
  unique_lock<mutex> guarda ( mtxEscritos);
  bool salida = false; //se pueden escribir más datos
  
  if (datosEscritos < num_items ) {
    datosEscritos++;
    salida = true; 
  }

  return salida; 
  
}


void productor( int nProductor ) {

  while ( quedanDatosEscribir() ) {

    //buffer para escribir con espacio
    sem_wait(bufferEscritura);
    
    int dato = producirDato( nProductor);
    //zona crítica escribir en cola
    mtxEscribirCola.lock(); 
    cola.escribir( dato );
    mtxEscribirCola.unlock();
    
    sem_signal(bufferLectura);  // ya hay un dato más disponible consumir
    
  }
}

mutex mtxConsultaDat;
int datosConsumidos=0;

bool quedanDatosConsumir() {
  bool ret = false;

  mtxConsultaDat.lock();
  if ( datosConsumidos < num_items ) {
    ret = true;
    datosConsumidos++;
  }
  mtxConsultaDat.unlock();
  
  return ret; 
}


void consumidor ( int nConsumidor ) {

  while ( quedanDatosConsumir() ) {
    sem_wait ( bufferLectura); // hay datos escritos
    mtxConsumidor.lock();
    int dato = cola.leer();
    consumirDato( dato, nConsumidor);
    mtxConsumidor.unlock();
    
    sem_signal( bufferEscritura); //hemos dejado libre una casilla de escribir
    
    
   
  }
}



int main( )
{

  cout << "--------------------------------------------------------" << endl
       << "Problema de VARIOS  productores-consumidores (solución FIFO)." << endl
       << "nº productores: " << num_productores << endl
       << "nº consumidores: " << num_consumidores << endl
       << "nº datos a producir: " << num_items << endl
       << " tamaño buffer: " << tam_vec << endl
       << "--------------------------------------------------------" << endl
       << flush ;

  thread hebra_productora[num_productores],hebra_consumidora[num_consumidores];
    
  for(int i=0; i< num_productores; i++)
    hebra_productora[i]= thread( productor, i );
  for(int i=0; i< num_consumidores; i++)
    hebra_consumidora[i]=thread( consumidor, i );
    
  for(int i=0; i< num_productores; i++)
    hebra_productora[i].join() ;
  for(int i=0; i< num_consumidores; i++)
    hebra_consumidora[i].join() ;

  test_contadores();
  
  return 0; 
}
