#include <iostream>
#include <iomanip>
#include "HoareMonitor.h"
#include <mutex>
#include "plantilla.h"

using namespace std ;
using namespace HM ;

/**
   @brief Monitor varios productores varios consumidores FIFO
   con semántica señalar urgente
 */
class ProdConsSUFIFO : public HoareMonitor {

private:
  CondVar colaProductores, colaConsumidores;
  mutex mtxProductores, mtxConsumidores, mtxProducirDato; 

  //tipo estructua
  int tamBuffer, cntBuffer; //tamaño del buffer y contador de datos escritos 
  int *cola;                //puntero estructura almacenamiento tipo fifo
  int posEscritura,posLectura;  //posiciones de escritura y lectura actuales en el buffer
  int dato=0; //contador de datos a producir
  

public:
  ProdConsSUFIFO ( int tam_buffer);
  ~ProdConsSUFIFO();

  int producirDato(); 
  void insertar(int dato, int id);
  int extraer(int id); 
    
};

ProdConsSUFIFO::ProdConsSUFIFO( int tam_buffer) {
  //
  dato = 0; 
  //inicializamos estructura de almacenamiento
  tamBuffer = tam_buffer;
  posEscritura = posLectura = cntBuffer=0;
  cola = new int [tamBuffer];

  //variables de control de exclusión
  colaProductores = newCondVar();
  colaConsumidores = newCondVar();  
}
ProdConsSUFIFO::~ProdConsSUFIFO() {
  delete [] cola;
  cola=NULL; 
}

void ProdConsSUFIFO::insertar(int dato, int id) {
  if ( cntBuffer == tamBuffer) { //buffer lleno tiene que esperar
    colaProductores.wait(); 
  }
  //escribimos en la cola
  //región crítica entre productores
  mtxProductores.lock();
  cntBuffer++; 
  cola[posEscritura] = dato;
  cout << "Productor " << id  << " escribe " << cola[posEscritura] << endl<<flush; 
  posEscritura = (posEscritura + 1) % tamBuffer;

  
  mtxProductores.unlock();

  //un elemento más puede consumirse
  colaConsumidores.signal(); 
  
}

int ProdConsSUFIFO::extraer( int id) {
  if( cntBuffer == 0) { //nada que consumir, debe esperar
    colaConsumidores.wait(); 
  }

  mtxConsumidores.lock();
  cntBuffer--; 
  int dato = cola[posLectura];
  posLectura = (posLectura + 1) % tamBuffer;

  cout << "\t\t\t Consumidor " << id << " extrae elemento " << dato << endl<<flush;
  mtxConsumidores.unlock();

  //liberamos una casilla en la que escribir
  colaProductores.signal(); 

  return dato; 
}

int ProdConsSUFIFO::producirDato() {
  unique_lock<mutex>(mtxProducirDato); 
  return dato++; 
}
// ---------- productores consumidores código -------------

void Productor( int inicio,int fin, MRef<ProdConsSUFIFO>  monitor, int id) {

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

void Consumidor(int cantidad, MRef<ProdConsSUFIFO> monitor, int id) {
  
  for(int i=0; i<cantidad; i++) {
    monitor->extraer(id); //consumimos dato
    sleep_this_thread(); 
  }
  cout << "\t\t\t Consumidor " <<id  << " ha acabado de consumir " << endl; 
}


//=============================== main ======================

int main() {

  int tamBuffer = 3;
  int numConsumidores= 4;
  int numProductores = 5;
  int cantidad = 21;


  cout << "------------------------------------------------------------------------------------" << endl
       << "  Problema de los productores-consumidores (solución FIFO con monitor y semántica SU)" << endl
       << "Nº de ítems: "<< cantidad
       << " | Nº consumidores: " <<numConsumidores
       << " | Nº productores: " << numProductores
       <<" | tamaño buffer: " <<tamBuffer<< endl
       << "--------------------------------------------------------------------------------------" << endl;
  
  // inicial monitor
  MRef<ProdConsSUFIFO> monitor = Create<ProdConsSUFIFO>(tamBuffer); 

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
