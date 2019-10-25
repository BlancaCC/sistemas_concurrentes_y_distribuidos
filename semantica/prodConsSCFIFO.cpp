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

class ProdConsSCFIFO {

private: 
  //estrutura fifo (cola)
  int posLectura, posEscritura;  //punteros
  int *cola;
  int tamBuffer;
  int cntBuffer;
  int numDatos; //número total de datos a producir

  //variables de control exclusión y finalización monitor
   int cntProductores, cntConsumidores;
  mutex mtxCola,
    mtxInsertar, mtxExtraer,
    mtxPuedoProducir, mtxPuedoConsumir;
  condition_variable colaInsertar, colaExtraer;



public:
  
  ProdConsSCFIFO( int tam_buffer, int num_datos) {
    posLectura = posEscritura = cntBuffer = 0;
    cola = new int [tam_buffer];
    tamBuffer = tam_buffer;
    numDatos = num_datos; 

    cntProductores = cntConsumidores= 0;
  }

  ~ProdConsSCFIFO(){
    delete [] cola; 
  }

  void insertar( int id) {
    unique_lock<mutex> insertando( mtxInsertar);
    while ( cntBuffer == tamBuffer ) {
      
      colaInsertar.wait( insertando); 
    }

    int dato=producir_dato();
    cola[posEscritura]=dato;
    posEscritura = (posEscritura+1)%tamBuffer;

    
    mtxCola.lock();
    cout << " Productor(" << id << ") inserta " << dato << endl; 
    cntBuffer++;
    mtxCola.unlock();
    
    colaExtraer.notify_one(); 
    
  }

  int extraer(int id) {
    unique_lock<mutex> extrayendo( mtxExtraer);

    while(cntBuffer==0) {
      colaExtraer.wait( extrayendo); 
    }

    int dato = cola[posLectura];
    
    posLectura = (posLectura+1)%tamBuffer;

     mtxCola.lock();
     cout << "\t\t\tConsumidor(" << id << ") extrae " << dato << endl; 
     cntBuffer--;
     mtxCola.unlock();
     
     colaInsertar.notify_one();
    return dato; 
  }

  bool puedoProducir() {
    bool ret=false;
    
    unique_lock<mutex>(mtxPuedoProducir);
    if( cntProductores < numDatos ) {
      ret =true;
      cntProductores++;
    }
    return ret; 
  }
  
  bool puedoConsumir() {
    bool ret=false;
    
    unique_lock<mutex>(mtxPuedoConsumir);
    if( cntConsumidores < numDatos ) {
      ret =true;
      cntConsumidores++;
    }
    return ret;
    
  }
};



void Consumidor ( int id, ProdConsSCFIFO * monitor) {
  while( monitor->puedoConsumir()) {
    
    consumir_dato ( monitor->extraer(id), id); 
  }

  cout << "\t\t¡¡¡Consumidor "<< id << " finaliza su tarea !!!"<< endl; 
}

void Productor( int id, ProdConsSCFIFO * monitor) {

  while( monitor->puedoProducir() ) {
    monitor->insertar( id);
  }
    cout << "¡¡¡Productor "<< id << " finaliza su tarea!!!"<< endl; 
}


//------- main----

int main() {

  const int N_CONSUMIDORES=5, N_PRODUCTORES=10;
  const int TAM_BUFFER=4;
  cout << "------------------------------------------------------------------------------------" << endl
	 << "  Problema de los productores-consumidores (solución FIFO con monitor y semántica SC)" << endl
	 << "Nº de ítems: "<< N_DATOS << " | Nº consumidores: " << N_CONSUMIDORES
	 << " | Nº productores: " << N_PRODUCTORES << " | tamaño buffer: " <<TAM_BUFFER << endl
	 << "--------------------------------------------------------------------------------------" << endl
       << flush ;

  ProdConsSCFIFO monitor(TAM_BUFFER,N_DATOS);
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



  


  
  return 0; 
}
