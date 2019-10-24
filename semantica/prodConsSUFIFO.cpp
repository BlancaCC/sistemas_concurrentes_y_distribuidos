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
  mutex mtxProductores, mtxConsumidores; 

  //tipo estructua
  int tamBuffer, cntBuffer; //tamaño del buffer y contador de datos escritos 
  int *cola;                //puntero estructura almacenamiento tipo fifo
  int posEscritura,posLectura;  //posiciones de escritura y lectura actuales en el buffer 

public:
  ProdConsSUFIFO ( int tam_buffer);
  ~ProdConsSUFIFO();

  void insertar(int dato, int id);
  int extraer(int id); 
    
};

ProdConsSUFIFO::ProdConsSUFIFO( int tam_buffer) {
  //inicializamos estructura de almacenamiento
  tamBuffer = tamBuffer;
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
  posEscritura = (posEscritura + 1) % tamBuffer;

  cout << "Productor " << this_thread::get_id()  << " escribe " << dato << endl; 
  mtxProductores.unlock();

  //un elemento más puede consumirse
  colaConsumidores.signal(); 
  
}

int ProdConsSUFIFO::extraer( int id) {
  if( cntBuffer == 0) { //nada que consumir, debe esperar
    cout << "consumidor " << this_thread::get_id() <<" queda paralizada "<< endl; 
    colaConsumidores.wait(); 
  }

  mtxConsumidores.lock();
  cntBuffer--; 
  int dato = cola[posLectura];
  posLectura = (posLectura + 1) & tamBuffer;

  cout << "\t\t Consumidor " << this_thread::get_id() << " extrae elemento " << dato << endl;
  mtxConsumidores.unlock();

  //liberamos una casilla en la que escribir
  colaProductores.signal(); 

  return dato; 
}
// ---------- productores consumidores código -------------

void Productor( int cantidad, ProdConsSUFIFO * monitor, int id) {

  for(int i=0; i<cantidad; i++) {
    int dato = producir_dato(); 
    monitor->insertar( dato, id);
  }
  cout << "Productor " << this_thread::get_id()<< " termina de producir " << endl; 
}

void Consumidor(int cantidad, ProdConsSUFIFO * monitor, int id) {
  
  for(int i=0; i<cantidad; i++) {
    consumir_dato( monitor->extraer(id));
  }
  cout << "\t\t Consumidor " <<this_thread::get_id()  << "ha acabado de consumir " << endl; 
}


//=============================== main ======================

int main() {

  int tamBuffer = 3;
  int numConsumidores= 4;
  int numProductores = 5;
  int cantidad = 40;


  cout << "------------------------------------------------------------------------------------" << endl
       << "  Problema de los productores-consumidores (solución FIFO con monitor y semántica SU)" << endl
       << "Nº de ítems: "<< cantidad
       << " | Nº consumidores: " <<numConsumidores
       << " | Nº productores: " << numProductores
       <<" | tamaño buffer: " <<tamBuffer<< endl
       << "--------------------------------------------------------------------------------------" << endl;
  
  // inicial monitor
  ProdConsSUFIFO monitor(tamBuffer); 

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
    cout << " se ha producido al hebra " << i << "  con módulo " << endl; 
    tConsumidor[i] = thread( Consumidor, (repartoConsumidor+1), &monitor); 
  }
  for(int i=restoConsumidor; i<numConsumidores; i++){
    cout << " se ha producido al hebra " << i << " sin módulo " << endl; 
    tConsumidor[i] = thread(Consumidor, repartoConsumidor, &monitor); 
  }

  //reparto productores
  for(int i=0; i<restoProductor; i++) {
    cout << " se ha producido el productor " << i << "  con módulo " << endl; 
    tProductor[i] = thread(Productor, (repartoProductor+1), &monitor, i); 
  }
  for(int i=restoProductor; i<numProductores; i++){
    cout << " se ha producido el productor " << i << "  SIN módulo " << endl; 
    tProductor[i] = thread(Productor, repartoProductor, &monitor, i); 
  }


  // esperamos a todos los consumidores (que serán los últimos en acabar)
 for(int i=0; i<numProductores; i++){
    tProductor[i].join();
  }
 for(int i=0; i<numConsumidores; i++){
    tConsumidor[i].join();
  }

  //comprobación contadores
  test_contadores(cantidad); 
  
  return 0; 
}
