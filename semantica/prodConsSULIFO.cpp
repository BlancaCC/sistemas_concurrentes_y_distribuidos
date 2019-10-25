#include <iostream>
#include <iomanip>
#include "HoareMonitor.h"
#include <mutex>
#include "plantilla.h"

using namespace std ;
using namespace HM ;

/**
   @brief Monitor varios productores varios consumidores LIFO
   con semántica señalar urgente
 */
class ProdConsSULIFO : public HoareMonitor {

private:
  CondVar colaProductores, colaConsumidores;
  mutex mtxProductores, mtxConsumidores, mtxProducirDato; 

  //tipo estructua
  int tamBuffer, cntBuffer; //tamaño del buffer y contador de datos escritos 
  int *pila;                //puntero estructura almacenamiento lifo
  int posTope;  //posiciones de escritura y lectura actuales en el buffer
  int dato=0; //contador de datos a producir
  

public:
  ProdConsSULIFO ( int tam_buffer);
  ~ProdConsSULIFO();

  int producirDato(); 
  void insertar(int dato, int id);
  int extraer(int id); 
    
};

ProdConsSULIFO::ProdConsSULIFO( int tam_buffer) {
  //
  dato = 0; 
  //inicializamos estructura de almacenamiento
  tamBuffer = tam_buffer;
  posTope = cntBuffer=0;
  pila = new int [tamBuffer];

  //variables de control de exclusión
  colaProductores = newCondVar();
  colaConsumidores = newCondVar();  
}
ProdConsSULIFO::~ProdConsSULIFO() {
  delete [] pila;
  pila=NULL; 
}

void ProdConsSULIFO::insertar(int dato, int id) {
  if ( cntBuffer == tamBuffer) { //buffer lleno tiene que esperar
    colaProductores.wait(); 
  }
  //escribimos en la cola
  //región crítica entre productores
  mtxProductores.lock();
  cntBuffer++; 
  pila[posTope++] = dato;
  cout << "Productor " << id  << " escribe " <<   pila[(posTope-1)] << endl<<flush; 
  

  
  mtxProductores.unlock();

  //un elemento más puede consumirse
  colaConsumidores.signal(); 
  
}

int ProdConsSULIFO::extraer( int id) {
  if( cntBuffer == 0) { //nada que consumir, debe esperar
    colaConsumidores.wait(); 
  }

  mtxConsumidores.lock(); //los primeros casos no nos aseguran la exclusión mutua
  cntBuffer--; 
  int dato = pila[--posTope];
  cout << "\t\t\t Consumidor " << id << " extrae elemento " << dato << endl<<flush;
  mtxConsumidores.unlock();

  //liberamos una casilla en la que escribir
  colaProductores.signal(); 

  return dato; 
}

int ProdConsSULIFO::producirDato() {
  unique_lock<mutex>(mtxProducirDato); 
  return dato++; 
}
// ---------- productores consumidores código -------------

void Productor( int inicio,int fin, MRef<ProdConsSULIFO>  monitor, int id) {

  for(int dato=inicio; dato<fin; dato++) {
    //versión numeración hebras desordeada
    // monitor->insertar( dato, id); //producimos datos desordenador

    //versión hebras ordenadas
    int md = monitor->producirDato(); 
    monitor->insertar(md , id); //datos ordenados
    
    sleep_this_thread(); 
  }
  cout << "--- Productor " << id<< " termina de producir ---" << endl; 
}

void Consumidor(int cantidad, MRef<ProdConsSULIFO> monitor, int id) {
  
  for(int i=0; i<cantidad; i++) {
    monitor->extraer(id); //consumimos dato
    sleep_this_thread(); 
  }
  cout << "\t\t\t --- Consumidor " <<id  << " ha acabado de consumir ---" << endl; 
}


//=============================== main ======================

int main() {

  int tamBuffer = 5;
  int numConsumidores= 7;
  int numProductores = 6;
  int cantidad = 40;


  cout << "------------------------------------------------------------------------------------" << endl
       << "  Problema de los productores-consumidores (solución LIFO con monitor y semántica SU)" << endl
       << "Nº de ítems: "<< cantidad
       << " | Nº consumidores: " <<numConsumidores
       << " | Nº productores: " << numProductores
       <<" | tamaño buffer: " <<tamBuffer<< endl
       << "--------------------------------------------------------------------------------------" << endl;
  
  // inicial monitor
  MRef<ProdConsSULIFO> monitor = Create<ProdConsSULIFO>(tamBuffer); 

  // hebras que entran en juego
  thread tProductor[numProductores], tConsumidor[numConsumidores];
    
  // cálculos prereparto hebras
  //(resuelve que la cantidad no sea un múltiplo del número de productor/consumidr
  int repartoConsumidor = cantidad /numConsumidores;
  int restoConsumidor = cantidad % numConsumidores;

  int repartoProductor = cantidad / numProductores;
  int restoProductor = cantidad % numProductores;

  // Consumidores con una consumición más, "reparto módulo"
  for(int i=0; i<restoConsumidor; i++) {
    //cout << " se ha producido al hebra " << i << "  con módulo " << endl; 
    tConsumidor[i] = thread( Consumidor, (repartoConsumidor+1), monitor, i); 
  }
  for(int i=restoConsumidor; i<numConsumidores; i++){
    //cout << " se ha producido al hebra " << i << " sin módulo " << endl; 
    tConsumidor[i] = thread(Consumidor, repartoConsumidor, monitor, i); 
  }

  //reparto productores
  int cantidad_inicio = 0,
    cantidad_fin = 0 ; 
  for(int i=0; i<restoProductor; i++) {
   
    cantidad_inicio = cantidad_fin;
    cantidad_fin += (repartoProductor+1);

    //cout << " se ha producido el productor " << i << "  con módulo " << endl;
    tProductor[i] = thread(Productor,cantidad_inicio, cantidad_fin, monitor, i); 
  }
  for(int i=restoProductor; i<numProductores; i++){
    //cout << " se ha producido el productor " << i << "  SIN módulo " << endl;
    cantidad_inicio = cantidad_fin;
    cantidad_fin += (repartoProductor);
    tProductor[i] = thread(Productor, cantidad_inicio, cantidad_fin, monitor, i); 
  }


  // esperamos a todos los consumidores (que serán los últimos en acabar)
 for(int i=0; i<numProductores; i++){
    tProductor[i].join();
  }
 for(int i=0; i<numConsumidores; i++){
    tConsumidor[i].join();
  }
  
  return 0; 
}
