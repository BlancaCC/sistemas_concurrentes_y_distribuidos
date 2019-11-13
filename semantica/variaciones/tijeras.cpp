/**
   Problema del barbero durmiente con tijeras

@author: Blanca Cano Camarero 
@date: 13 / XI / 19 

- Hay un barbero que pela con una tijeras
- Varios clientes

Condiciones: 
- Un cliente entra a la barbería, si el sillón de pelar está desierto te sienta, si el babero está durmiendo lo despierta

- Un barbero pela si hay alguien en la silla de pelar, si no lo llama de la sala de espera, si tampoco lo hay ahí se echa dormir

- Tras pelar el barberlo busca a otro cliente 
- Tras ser pelado el cliente espera fuera de la barbería. 


 */

#include <iostream>
#include <iomanip>
#include "HoareMonitor.h"
#include <cassert>
#include <random>

using namespace std ;
using namespace HM ;

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}


class SuministroTijeras : public HoareMonitor {
private:
  CondVar barberoEsperaTijeras = newCondVar();
  CondVar bufferLLeno = newCondVar(); //no queda hueco para traer más tijeras

  int tamPaquete = 0; 

public:

  //barbero coge tijeras
  int cogerTijeras();
  void ponerTijeras(int i);
  
};


void SuministroTijeras::ponerTijeras(int n) {
  //comprueba si es necesario recargar
  if(tamPaquete != 0) {
    cout << " Sumintrador de tijeras no puede poner más porque el buffer está lleno" << endl; 
    bufferLLeno.wait();
  }
  tamPaquete = n;
  cout << "Suministrado de tijeras coloca " << n << " nuevas " << endl;
  if( !barberoEsperaTijeras.empty())
    barberoEsperaTijeras.signal();
}
int SuministroTijeras::cogerTijeras() {
  int ret;
  if(tamPaquete == 0) {
    cout << "No hay tijeras en el almacén el barbero espera" << endl; 
    barberoEsperaTijeras.wait(); //espera a que venga el paquete
  }
  ret = tamPaquete;
  tamPaquete = 0;
  
  if(!bufferLLeno.empty()) {
    cout << "Barbero avisa al repartidos de que ya puede volver a reponer" << endl; 
    bufferLLeno.signal();
  }
  cout << "Barbera marcha a la peluquería a seguir con sus quehaceres peluqueros " << endl; 
  return ret;
}


//clase barbería control; clientes y acción de pelar 
class Barberia : public HoareMonitor {

private:
  CondVar salaEspera = newCondVar();
  CondVar barbero = newCondVar(); //el barbero espera cuando duerme, es decir no hay clientes que pelar
  CondVar sillonPelar = newCondVar(); //mientras el cliente está siendo pelado

  int numTijeras = 0; //número de tijeras que tiene almacenadas 
  int numUsos = 0;    //número de usos que lleva con la tijera actual
  const int maxUsosTijeras = 2;
  
  void usarTijeras(); //función que gestiona el uso de las tijeras

public:

  //tiempo en que se está cortando el pelo el cliente
  void cortarPelo(int id);
  void siguienteCliente(); // el barbero se pone a pelar a otro cliente
  void finCliente(); //fin del cliente que está pelado
  bool barberoTieneTijeras(); //true si el barbero tiene suficientes tijeras pasa seguir
  void recargarTijeras(int n);
  
};

void Barberia::recargarTijeras(int n) {
  cout << "Barbero recarga " << n << "tijeras nuevas " << endl; 
  numTijeras = n;
}
void Barberia::usarTijeras() {
  numUsos++;
  if(numUsos == maxUsosTijeras) {
    numTijeras--;
    numUsos = 0;
  }
}

bool Barberia::barberoTieneTijeras() {
  return !(numTijeras == 0);
}

void Barberia::siguienteCliente() {

  // si no hay cliente esperando a ser pelado en el sillón 
  if( sillonPelar.empty() ) {
    // si no hay cliente en la sala de espera esperando a ser pelado
    if(salaEspera.empty()) {
      //bloqueamos al barbero
      cout << "No hay clientes el barbero se duerme "<< endl;
      barbero.wait();
    }
    else {
      // sí hay un cliente esperando en al sala de espera
      cout << "Barbero llama a un cliente de la sala de espera" << endl; 
      salaEspera.signal(); //lo saca para el sillón de pelar 
    } 
  }
}

void Barberia::cortarPelo(int id) {
  
  if(!barbero.empty()) { // si el barbero está dormido lo despertamos
    cout << "El cliente " << id << " despierta al barbero y pasa directamente a pelarse " << endl; 
    barbero.signal();
  }
  
  //mientras halla alguien pelándose
  else if(!sillonPelar.empty()) {
    cout << "Clinte " << id << " en sala de espera " << endl;
    salaEspera.wait(); // se espera en la sala de espera
  }

  cout << "Cliente " << id << " ocupa sillón de pelar" << endl;

  sillonPelar.wait(); //ocupa el sillón de pelar

  cout << "\t\tClienten " << id << " marcha pelado " << endl<< endl; 
}
/**
   Tiempo en que el cliente @param id está detenido 
 */
void EsperarFueraBarberia(int id) {
  
  chrono::milliseconds duracion(aleatorio<30,1000>());

  this_thread::sleep_for(duracion);

  cout << "Tras " << duracion.count() << " el cliente " << id
       << " se dirige a la barbería " << endl; 
}


void irAPorMasSuministros() {
    chrono::milliseconds duracion(aleatorio<30,1000>());

    this_thread::sleep_for(duracion);
    
     cout << "El proveedor ha tardado  " << duracion.count() << " en ir a por tijeras " << endl; 
}
/**
   Cortar pelo tiempo en que el barbero está cotando el pelo 
 */


void CortarPeloCliente() {
  chrono::milliseconds duracion(aleatorio<30,300>());

  this_thread::sleep_for(duracion);

  //cout << "El barbero ha tardado " << duracion.count() << " en despachar al cliente  " << endl; 
}


void Barberia::finCliente() {
  sillonPelar.signal();
  usarTijeras();
  cout << " El barbero ha despachado al cliente " << endl; 
}

/**
   @brief función que contendrá a la hebra cliente
 */
void Cliente(int id, MRef<Barberia>Mbarberia){
  while(true){
    Mbarberia->cortarPelo(id);
    EsperarFueraBarberia(id);
  }
  
}


void Barbero(MRef<Barberia>Mbarberia, MRef<SuministroTijeras>Mtijeras){
  while(true){
    //comprobamos que tiene material suficiente para trabajar
    if(! Mbarberia->barberoTieneTijeras()){
      Mbarberia->recargarTijeras( Mtijeras->cogerTijeras());
    }
    Mbarberia->siguienteCliente();
    CortarPeloCliente();
    Mbarberia->finCliente();
  }
}

void Proveedor(MRef<SuministroTijeras>Mtijeras){

  while(true) {
    
  Mtijeras->ponerTijeras(1);
  irAPorMasSuministros();
  }
  
}
int main(){

  const int N_CLIENTES=10; 
  cout << " --------- PROBLEMA DEL BARBERO DURMIENTE CON TIJERAS ---------- " << endl
       << "Nº clientes: " << N_CLIENTES << endl
       << " ----------------------------------------------------" << endl;

  MRef<Barberia> Mbarberia = Create<Barberia>();
  MRef<SuministroTijeras>Mtijeras = Create<SuministroTijeras>();
  
  thread barbero(Barbero, Mbarberia,Mtijeras);
  thread clientes[N_CLIENTES];
  
  for(int i=0; i<N_CLIENTES; i++)
    clientes[i]=thread(Cliente, i, Mbarberia);

  thread proveedor(Proveedor, Mtijeras);
  barbero.join();
  return 0; 
}
