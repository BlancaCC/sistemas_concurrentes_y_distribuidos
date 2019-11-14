/**
   Problema del barbero durmiente  barbero con capacidad y exclusividad  de pelar a dos clientes simultáneos 
   todos los clientes pasa por la sala de espera esperando de que el barbero los llame

@author: Blanca Cano Camarero 
@date: 14 / XI / 19 

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
  const  int numClientesParalelo = 2; //número de clientes que es capaz de cortar a la vez
  
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

  
  if(salaEspera.get_nwt()<numClientesParalelo) {
    cout << "\t\t\t\t\t\t No hay " << numClientesParalelo << " clientes así que se acuesta " << endl; 
    barbero.wait();
  }

  cout << "Barbero llama a la escalofrientate cantidad de " << numClientesParalelo
       <<" clientes de la sala de espera a que se sienten para ser pelados" << endl; 
  for( int i=0; i< numClientesParalelo; i++)
    salaEspera.signal();
  
}

void Barberia::cortarPelo(int id) {
  cout << "Cliente " << id << " entra a la barbería " << endl; 

    // si el barbero está durmiendo y hay alguien más esperando
  if(!barbero.empty() && !salaEspera.empty()) {
    cout << "Cliente " << id << " avisa al barbero de que ya hay otra persona esperando " << endl; 
    barbero.signal();
  }
  cout << "\t Cliente " << id << " pasa a sala espera"<< endl; 
  salaEspera.wait();

   cout << "\t\t\t\t\t Cliente " << id << " se sienta en la sala de pelar"<< endl;
   sillonPelar.wait();


   cout << "\t\t\t\t\t\t Cliente " << id << " marcha pelado " << endl; 
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

  //cout << "El barbero ha tardado " << duracion.count() << " en despachar al cliente  " << endl; 
}


void Barberia::finCliente() {

  for(int i=0; i< numClientesParalelo; i++)
  sillonPelar.signal();
  cout << "\t\t\t\t\t\tEl barbero ha despachado con maestría a sus " << numClientesParalelo <<" clientes " << endl<<endl; 
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

  const int N_CLIENTES=7; 
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
