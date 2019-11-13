/**
   Problema del barbero durmiente con varias sillas, barberos y clientes

@author: Blanca Cano Camarero 
@date: 13 / XI / 19 

- Hay Varios Barberos
- Hay varios clientes
- Hay varias sillas

Condiciones: 
- Un cliente entra a la barbería, si el sillón de pelar está desierto te sienta, si el babero está durmiendo lo despierta para que lo atienda, y si todos los barberos están trabajando espera a que le atiendan

- Un barbero pela si hay alguien en la silla de pelar, si no lo llama de la sala de espera, si tampoco lo hay ahí se echa dormir

- Tras pelar el barbero busca a otro cliente 
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

  int numBarberos, numSillas;
  int barberosTrabajando = 0;


public:

  //tiempo en que se está cortando el pelo el cliente
  Barberia(int nb,int ns):numBarberos(nb),numSillas(ns){};
  void cortarPelo(int id);
  void siguienteCliente(int id); // el barbero se pone a pelar a otro cliente
  void finCliente(int id); // fin del cliente que está pelado 
  
};


void Barberia::siguienteCliente(int id) {

  // si no hay cliente esperando a ser pelado en el sillón 
  if( sillonPelar.get_nwt() == barberosTrabajando) { //el número de barberos atendiendo (que no sea yo) es menor que la sillas a espensas des ser atendidas
    // si no hay cliente en la sala de espera esperando a ser pelado
    if(salaEspera.empty()) {
      //bloqueamos al barbero
      cout << "No hay clientes el barbero " << id <<" se duerme "<< endl;
      barbero.wait();
    }
    else {
      // sí hay un cliente esperando en al sala de espera
      cout << "Barbero " << id <<" llama a un cliente de la sala de espera" << endl; 
      salaEspera.signal(); //lo saca para el sillón de pelar 
    } 
  }
barberosTrabajando++;
}

void Barberia::cortarPelo(int id) {

  // DESPERTAR BARBERO
  // hay sillas libres, todos los barberos activos están atendiendo y quedan barberos descansando
  int sillas = sillonPelar.get_nwt();
  
  if( sillas< numSillas && sillas == barberosTrabajando && !barbero.empty()) {
    //if(barbero.get_nwt() == numBarberos){ // si ambos barberos están dormidos 
    cout << "El cliente " << id << " despierta a uno de los  barberos y pasa directamente a pelarse " << endl; 
    barbero.signal();
  }
  
  //mira si quedan sillas libres donde sentarse
  else if (sillas == numSillas) {
    cout << "Clinte " << id << " en sala de espera " << endl;
    salaEspera.wait(); // se espera en la sala de espera
  }

  cout << "Cliente " << id << " ocupa sillón de pelar" << endl
       << "barberos durmiendo " << barbero.get_nwt() << endl;

  sillonPelar.wait(); //ocupa el sillón de pelar

  cout << "\t\t\tCliente " << id << " marcha pelado " << endl; 
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

  //cout << "El barbero ha tardado " << duracion.count()  << " en despachar al cliente  " << endl; 
}


void Barberia::finCliente(int id) {
  sillonPelar.signal();
  cout << "\t\t\t\t El barbero " << id <<" ha despachado a su cliente " << endl
       << "\t\t\t\t \tsillas ocupadas :" << sillonPelar.get_nwt() << endl;
  barberosTrabajando--;
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


void Barbero(MRef<Barberia>Mbarberia, int id){
  while(true){
    Mbarberia->siguienteCliente(id);
    CortarPeloCliente();
    Mbarberia->finCliente(id);
  }
}


int main(){

  const int N_CLIENTES=4;
  const int N_BARBEROS=2;
  const int N_SILLAS = 3;
  cout << " --------- PROBLEMA DEL BARBERO DURMIENTE ---------- " << endl
       << "Nº clientes: " << N_CLIENTES << endl
       << "Número de barberos " << N_BARBEROS << endl
       << "Nº de sillas " << N_SILLAS << endl
       << " ----------------------------------------------------" << endl;

  MRef<Barberia> Mbarberia = Create<Barberia>(N_BARBEROS,N_SILLAS);
  //thread barbero(Barbero, Mbarberia);
  
  thread clientes[N_CLIENTES];
  thread barberos[N_BARBEROS];

  
  for(int i=0; i<N_CLIENTES; i++)
    clientes[i]=thread(Cliente, i, Mbarberia);

  for(int i=0; i<N_BARBEROS; i++)
    barberos[i]=thread(Barbero, Mbarberia, i);

  barberos[0].join(); //no queremos que el programa no acabe nunca 


  
  return 0; 
}
