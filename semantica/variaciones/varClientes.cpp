/**
   Problema del barbero durmiente  modificación dos tipos clientes 

@author: Blanca Cano Camarero 
@date: 14 / XI / 19 


1.-Modificar la solución al problema del barbero durmiente, tal como se muestra a continuación, guardando la solución en un archivo denominado barberia2.cpp. El comportamiento es idéntico al problema original, salvo en dos aspectos:

a) En este caso, existen 2 tipos de clientes, tipo 0 y tipo 1, existiendo sillas específicas en la sala de espera para los clientes de cada tipo concreto. Se puede asumir que existen 3 clientes de cada tipo.

b) el barbero no puede cortar de forma consecutiva a dos clientes del mismo tipo por lo que debe ir cortando el pelo de forma alternada a los clientes de cada tipo.

c) Cuando el barbero termina con un cliente de un tipo, comprueba si hay algún cliente del otro tipo y, si no lo hay, se echa a dormir hasta que llegue uno del tipo apropiado.

d) Si llega un cliente que no es demandado en ese instante por el barbero, debe entrar en la sala de espera, aunque el barbero esté durmiendo. Al abrir la barbería, el barbero empieza siempre pelando a un cliente del tipo 0, después a uno del tipo 1 y así sucesivamente.

e) Una vez despertado, el barbero debe pelar al cliente que le despertó que ya estará sentado o estará a punto de sentarse en la silla de pelar.
- Hay un barbero que pela
- Varios clientes

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
  CondVar esperaTipo0 = newCondVar();
  CondVar esperaTipo1 = newCondVar();
  bool turnoTipo0 = true;
  
  CondVar barbero = newCondVar(); //el barbero espera cuando duerme, es decir no hay clientes que pelar
  CondVar sillonPelar = newCondVar(); //mientras el cliente está siendo pelado


public:

  //tiempo en que se está cortando el pelo el cliente
  void cortarPelo(int id, bool tipo);
  void siguienteCliente(); // el barbero se pone a pelar a otro cliente
  void finCliente(); // fin del cliente que está pelado
  void cambioTurno() { turnoTipo0 = !turnoTipo0; }
  
};


void Barberia::siguienteCliente() {

  // si no hay cliente esperando a ser pelado en el sillón 
  if( sillonPelar.empty() ) {
    // si no hay cliente en la sala de espera esperando a ser pelado
    if(turnoTipo0){
      if(esperaTipo0.empty()) {
	//bloqueamos al barbero
	cout << "\t\t\t\t\tNo hay clientes del tipo 0, el barbero se duerme "<< endl;
	barbero.wait();
      }
      else {
	// sí hay un cliente tipo 0 esperando en al sala de espera
	cout << "\tBarbero llama a un cliente del TIPO 0 de la sala de espera" << endl;

	esperaTipo0.signal(); //lo saca para el sillón de pelar 
      }
    }
    else { //el tipo 0 
      if(esperaTipo1.empty()) {
	//bloqueamos al barbero
	cout << "\t\t\t\t\tNo hay clientes del tipo 1, el barbero se duerme "<< endl;
	barbero.wait();
      }
      else {
	// sí hay un cliente tipo 0 esperando en al sala de espera
	cout << "\tBarbero llama a un cliente del TIPO 1 de la sala de espera" << endl;
	esperaTipo1.signal(); //lo saca para el sillón de pelar 
      } 
    }
  }
}

void Barberia::cortarPelo(int id, bool tipo) {

  string tipoCliente = (tipo == true)?"tipo 0" : "tipo 1"; 
  
  if(!barbero.empty() && (tipo == turnoTipo0)) { // si el barbero está dormido lo despertamos
    cout << "El cliente " << id << " que es de " << tipoCliente
	 << " despierta al barbero y pasa directamente a pelarse " << endl; 
    barbero.signal();
  }
  
  //mientras halla alguien pelándose
  else if((turnoTipo0 == tipo) || !sillonPelar.empty()) {
    cout << "Clinte " << tipoCliente << " nº " << id << " en sala de espera " << tipoCliente << endl;
    if (tipo) esperaTipo0.wait(); else esperaTipo1.wait(); 

  }

  cout << "Cliente " << tipoCliente << " nº " << id << " ocupa sillón de pelar" << endl;

  sillonPelar.wait(); //ocupa el sillón de pelar

  cout << "\t\t\t\tliente "<< tipoCliente << " nº " << id << " marcha pelado " << endl<< endl; 
}
/**
   Tiempo en que el cliente @param id está detenido 
 */
 void EsperarFueraBarberia(int id, bool tipo) {
  string tipoCliente = (tipo == true)?"tipo 0" : "tipo 1"; 
  chrono::milliseconds duracion(aleatorio<30,1000>());

  this_thread::sleep_for(duracion);

  cout << "Tras " << duracion.count() << " el ciente "<< tipoCliente << " nº " << id
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
  sillonPelar.signal();
  cout << "\t\t\t\tEl barbero ha despachado al cliente " << endl; 
}

/**
   @brief función que contendrá a la hebra cliente
 */
void Cliente(int id, MRef<Barberia>Mbarberia, bool tipo0){
  while(true){
    Mbarberia->cortarPelo(id,tipo0);
    EsperarFueraBarberia(id,tipo0);
  }
  
}


void Barbero(MRef<Barberia>Mbarberia){
  while(true){
    Mbarberia->siguienteCliente();
    CortarPeloCliente();
    Mbarberia->finCliente();
    Mbarberia->cambioTurno();
  }
}


int main(){

  const int N_CLIENTES=6; 
  cout << " --------- PROBLEMA DEL BARBERO DURMIENTE ---------- " << endl
       << "Nº clientes: " << N_CLIENTES << endl
       << " ----------------------------------------------------" << endl;

  MRef<Barberia> Mbarberia = Create<Barberia>();
  thread barbero(Barbero, Mbarberia);
  thread clientes[N_CLIENTES];
  
  for(int i=0; i<N_CLIENTES; i++)
    clientes[i]=thread(Cliente, i, Mbarberia, ((i%2)==0));

  barbero.join();
  return 0; 
}
