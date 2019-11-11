/**
   Problema del barbero durmiente 

@author: Blanca Cano Camarero 
@date: 30 / X / 19 

- Hay un barbero que pela
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


class Barberia : public HoareMonitor {

private:
  CondVar salaEspera = newCondVar();
  CondVar barbero = newCondVar(); //ell barbero espera cuando duerme, es decir no hay clientes que pelar
  CondVar sillonPelar = newCondVar(); //mientras el cliente está siendo pelado


public:

  //tiempo en que se está cortando el pelo el cliente
  void cortarPelo(int id);
  void siguienteCliente(); // el barbero se pone a pelar a otro cliente
  void finCliente(); // fin del cliente que está pelado 
  
};


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
  else(!sillonPelar.empty()) {
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

  cout << "Tras " << duracion.count() << " el ciente " << id
       << " se dirige a la barbería " << endl; 
}


/**
   Cortar pelo tiempo en que el barbero está cotando el pelo 
 */


void CortarPeloCliente() {
  chrono::milliseconds duracion(aleatorio<30,300>());

  this_thread::sleep_for(duracion);

  cout << "El barbero ha tardado " << duracion.count() 
       << " en despachar al cliente  " << endl; 
}


void Barberia::finCliente() {
  sillonPelar.signal();
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


void Barbero(MRef<Barberia>Mbarberia){
  while(true){
    Mbarberia->siguienteCliente();
    CortarPeloCliente();
    Mbarberia->finCliente();
  }
}


int main(){

  const int N_CLIENTES=10; 
  cout << " --------- PROBLEMA DEL BARBERO DURMIENTE ---------- " << endl
       << "Nº clientes: " << N_CLIENTES << endl
       << " ----------------------------------------------------" << endl;

  MRef<Barberia> Mbarberia = Create<Barberia>();
  thread barbero(Barbero, Mbarberia);
  thread clientes[N_CLIENTES];
  
  for(int i=0; i<N_CLIENTES; i++)
    clientes[i]=thread(Cliente, i, Mbarberia);

  barbero.join();
  return 0; 
}
