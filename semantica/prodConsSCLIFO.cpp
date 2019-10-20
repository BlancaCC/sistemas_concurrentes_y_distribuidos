/**
   @author Blanca Cano Camarero
 */

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "plantilla.h"

using namespace std;



class ProdConsSCLIFO {

private:
  //estructura LIFO
  
  int primeraLibre=0; 
  int *pila;
  int TAM_BUFFER;
  //variables permanentes para acceso y control
  mutex mtxPila,                       //exclusión mutua pila
    mtxPuedoConsumir,mtxPuedoProducir, //condión fin de consumidores y prods
    mtxProductores, mtxConsumidores;
  
  condition_variable colaProductor, colaConsumidor;
  int cntProductores, cntConsumidores; 
  
  

public:

  ProdConsSCLIFO( int tamBuffer) {
    primeraLibre=0;
    TAM_BUFFER=tamBuffer;
    pila=new int[TAM_BUFFER];
    cntProductores=0;
    cntConsumidores=0; 
  }
  ~ProdConsSCLIFO() {
    delete [] pila;
    pila=NULL; 
  }

  void  insertar( int nHebra) {

    //tener cuidado con la variable primeraLibre
    unique_lock<mutex> ulCompruebaLibreProd( mtxProductores);
    while ( primeraLibre ==TAM_BUFFER) {
    //if ( primeraLibre ==TAM_BUFFER) {
      colaProductor.wait(ulCompruebaLibreProd);
    }
    
    mtxPila.lock();
    int dato = producir_dato(); //producimos dato justo aquí
    pila[primeraLibre++]= dato; 
    cout << "Productor " << nHebra << " escribe " << dato
	 << "(datos en pila: " << primeraLibre << ")" << endl;
    mtxPila.unlock();

    colaConsumidor.notify_one(); 
  }

  int extraer( int nHebra) {

    unique_lock<mutex> ulCompruebaLibreCons(mtxConsumidores);
    while (primeraLibre == 0) {
    //if (primeraLibre == 0) {
      colaConsumidor.wait( ulCompruebaLibreCons);
    }

    mtxPila.lock();
    int dato=pila[--primeraLibre]; 
    cout << "\t\t\tConsumidor " << nHebra << " extrae " << dato
	 << " (datos en pila: " << primeraLibre <<  ")"<< endl; 
    mtxPila.unlock();
    
    colaProductor.notify_one();

    return dato; 
    
  }

  /**
     @brief nos proporciona si quedan datos por producir
   */
  bool puedoProducir() {
    bool ret=false;
    mtxPuedoProducir.lock();
    //unique_lock<mutex>(mtxPuedoProducir);
    //      cout << "¿producir? " <<  cntProductores << endl;  
    if( cntProductores < N_DATOS) {
      ret=true;
      cntProductores++; 
    }
     mtxPuedoProducir.unlock();
    return ret; 
  }
  
  bool puedoConsumir() {
    
    bool ret=false;
    // unique_lock<mutex>(mtxPuedoConsumir);
    mtxPuedoConsumir.lock();
    //cout << "Consumir?? " << cntConsumidores << endl; 
    if( cntConsumidores <N_DATOS) {
      ret=true;
      cntConsumidores++;
    }
    mtxPuedoConsumir.unlock(); 
    return ret; 
  }

 
  
};


// -------- Funciones de la plantilla ---- 

//---------------------- mis productores y consumidores --------------

void Consumidor ( int id, ProdConsSCLIFO * monitor) {
  while( monitor->puedoConsumir()) {
    
    consumir_dato ( monitor->extraer(id)); 
  }

  cout << "\t\t¡¡¡Consumidor "<< id << " finaliza su tarea !!!"<< endl; 
}

void Productor( int id, ProdConsSCLIFO * monitor) {

  while( monitor->puedoProducir() ) {
    monitor->insertar( id);
  }
    cout << "¡¡¡Productor "<< id << " finaliza su tarea!!!"<< endl; 
}


//------- main----

int main() {

  const int N_CONSUMIDORES=5, N_PRODUCTORES=10;
  const int TAM_BUFFER=5;
  cout << "------------------------------------------------------------------------------------" << endl
	 << "  Problema de los productores-consumidores (solución LIFO con monitor y semántica SC)" << endl
	 << "Nº de ítems: "<< N_DATOS << " | Nº consumidores: " << N_CONSUMIDORES
	 << " | Nº productores: " << N_PRODUCTORES << " | tamaño buffer: " <<TAM_BUFFER << endl
	 << "--------------------------------------------------------------------------------------" << endl
       << flush ;

  ProdConsSCLIFO monitor(TAM_BUFFER);
  thread tConsumidores[N_CONSUMIDORES],tProductores[N_PRODUCTORES];

  for(int i=0; i<N_CONSUMIDORES; i++)
    tConsumidores[i]=thread(Consumidor, i,&monitor);
  
  for(int i=0; i<N_PRODUCTORES; i++)
    tProductores[i]=thread(Productor, i,&monitor);

  //los consumidores acabaran después de los productores, por tanto solo bastará esperarlos a ellos
  for(int i=0; i<N_PRODUCTORES; i++)
    tProductores[i].join();

  for(int i=0; i<N_CONSUMIDORES; i++)
    tConsumidores[i].join();

  test_contadores(N_DATOS);

  


  
  return 0; 
}
