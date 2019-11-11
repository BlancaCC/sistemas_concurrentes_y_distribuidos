/**
   @brief Simulación de una barbería compleja
   Multiples barberos,
   múltiples sillas de pelar
   una zona de descanso personal de trabajo 
   una sala de espera
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


class Barberia : public  HoareMonitor {

private:
  CondVar salaEspera = newCondVar();
  CondVar barberoDescansa = newCondVar();
  CondVar sillaPelar = newCondVar();

  int numBarberos, numSillas;
  int sillaSiendoAtendidas = 0;

public:
  Barberia(int nb, int ns):numBarberos(nb),numSillas(ns){};
  void irBarberia(int id);
  void atenderCliente(int id);
  void despedirCliente(int id);
 
};

void Barberia::irBarberia(int id) {
  if(sillaPelar.get_nwt() < numSillas) { // si quedan silla para pelarse libres
    if(!barberoDescansa.empty()) { //si tras ocupar la silla   ((numBarberos - barberoDescansa.get_nwt()) <= sillaPelar.get_nwt())
      barberoDescansa.signal();
    }
  }
  else { //no hay sillas disponibles así que se sienta en la sal ade espera
    salaEspera.wait();
    cout << "\t\tCliente: " << id << " a sala espera" << endl;
  }
  cout << "\t\t\t\tCliente " << id << " se siente en la silla de pelar " << endl;
  sillaPelar.wait();

  cout << "\t\t\t\tCliente " << id << " se marcha de la barbería" << endl;
}

void Barberia::atenderCliente(int id) {
  // mira si hay alguna silla sin atender
  if((sillaPelar.get_nwt() == sillaSiendoAtendidas)) {
   
  // si hay sillas libres y clientes en la sala de espera
    if(sillaPelar.get_nwt() < numSillas && !salaEspera.empty()) {
      salaEspera.signal();
    }
    // si no puede trabajar por falta de sillas o no hay clientes esperando
    else
      barberoDescansa.wait();
  }

  sillaSiendoAtendidas++;
  cout << "\t\t\tBarbero " << id << " va a la silla a pelar" << endl;
    
}

void Barberia::despedirCliente(int id) {
  sillaSiendoAtendidas--; 
  cout << "\t\t\t\tBarbero " << id << " termina su faena" << endl; 
}

//< Tiemoi esperando a que le crezca el pelo al cliente 
void esperarFuera(int id) {
  chrono::milliseconds duracion(aleatorio<30,1000>());

  this_thread::sleep_for(duracion);

  cout << "Tras " << duracion.count() << " el ciente " << id
       << " se dirige a la barbería " << endl; 
}



//< Hebra cliente 
void hCliente(int id, MRef<Barberia> monitor) {
  while(true) {
    esperarFuera(id);
    monitor->irBarberia(id);
  }
}

//< Tiempo que se tarda en cortar el pelo
void tiempoCorte() {
  chrono::milliseconds duracion(aleatorio<30,1000>());
  this_thread::sleep_for(duracion);

}


void hBarbero(int id, MRef<Barberia> monitor) {
  while(true) {
    // cout << "Este mensje en el barbero debría mostrar algo en pantalla "<< endl; 
    monitor->atenderCliente(id);
    tiempoCorte();
    monitor->despedirCliente(id);
  }
  
}


int main() {

  int numSillas = 3;
  int numBarberos = 5;
  int numClientes = 7;

  cout << " --- Barbería con varios barberos y varias sillas  --- "  << endl
       << " Número de sillas "  << numSillas << endl
       << " Numero de barberos " << numBarberos << endl
       << " Número de clientes " << numClientes << endl
       << " --------------------------------------------- " << endl;

  MRef<Barberia> M = Create<Barberia>(numBarberos, numSillas);
  thread barbero[numBarberos], clientes[numClientes];
  
  for(int i=0; i<numClientes; i++)
    clientes[i]=thread(hCliente, i, M);

  for(int i=0; i<numBarberos; i++)
    barbero[i]=thread(hBarbero, i, M);

  for(int i=0; i<numBarberos; i++)
	barbero[i].join(); //para que no acabe el programa de golpe
    
  for(int i=0; i<numClientes; i++)
    clientes[i].join();

  return 0; 
  
}


