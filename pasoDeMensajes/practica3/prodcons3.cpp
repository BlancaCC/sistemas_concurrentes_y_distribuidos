// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: prodcons3.cpp
// Implementación del problema del productores-consumidores con
// un proceso intermedio que gestiona un buffer finito y recibe peticiones
// en orden arbitrario
// (versión con varios productores y varios consumidores)
//
// Fecha Noviembre 2019 
// Blanca Cano Camarero 
// -----------------------------------------------------------------------------

#include <iostream>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <mpi.h>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
  num_productores   = 5,
  num_consumidores  = 3,
  num_items             = 20,
  tam_vector            = 5,

  
  tag_productor     = 1,
  id_productor_inf  = 1,
  id_productor_sup  = num_productores, //  id_productor_sup <= num_productor <=   id_productor_sup
  
  id_buffer         = 0,
  
  tag_consumidor    = 2, 
  id_consumidor_inf = num_productores + 1,
  id_consumidor_sup = num_consumidores + id_consumidor_inf - 1,
  
  num_procesos_esperados = num_productores + num_consumidores + 1;
 
  

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}
// ---------------------------------------------------------------------
// ptoducir produce los numeros en secuencia (1,2,3,....)
// y lleva espera aleatorio
int producir( int id, int & contador )
{
  //static int contador = 0 ;
   sleep_for( milliseconds( aleatorio<10,100>()) );
   contador++ ;
   cout << "Productor " << id << " produce " << contador << endl << flush;
   return contador ;
}
// ---------------------------------------------------------------------

void funcion_productor( int id )
{
  int datos_a_producir = num_items / num_productores,
    dato = datos_a_producir * id ,  //dato que se va a enviar, este valor se va a ajustar 
    resto = (num_items % num_productores);

  
  if( resto > 0) {
    
    if( id < (num_items % num_productores)) {
      datos_a_producir++;
      dato += id; //restos a sumar 
    }
    else
      dato += resto; 
  }
   for (  int i= 0 ; i < datos_a_producir ; i++ )
   {
      // producir valor
     int valor_prod = producir( id, dato );
      
      // enviar valor
      cout << "Productor " << id << " envía " << valor_prod << endl << flush;
      MPI_Ssend( &valor_prod, 1, MPI_INT, id_buffer, tag_productor, MPI_COMM_WORLD );
   }
}
// ---------------------------------------------------------------------

void consumir( int valor_cons, int id )
{
   // espera bloqueada
   sleep_for( milliseconds( aleatorio<110,200>()) );
   cout << " \t\t\t\t\t\t\tConsumidor " << id <<" consume " << valor_cons << endl << flush ;
}
// ---------------------------------------------------------------------

void funcion_consumidor( int id )
{
   int
     peticion,
     valor_rec = 1 ;

   
   MPI_Status  estado ;

   int valores_a_recibir = num_items/num_consumidores;
   if ( id < (num_items % num_consumidores) )
     valores_a_recibir += 1;

   
   for( int i=0 ; i < valores_a_recibir; i++ )
   {
      MPI_Ssend( &peticion,  1, MPI_INT, id_buffer, tag_consumidor, MPI_COMM_WORLD);
      MPI_Recv ( &valor_rec, 1, MPI_INT, id_buffer, 0, MPI_COMM_WORLD,&estado );
      cout << " \t\t\t\t\t\t\tConsumidor " << id <<" recibe " << valor_rec << endl << flush ;
      consumir( valor_rec, id );
   }
}
// ---------------------------------------------------------------------

void funcion_buffer()
{
  int
    buffer[tam_vector],      // buffer con celdas ocupadas y vacías
    valor,                   // valor recibido o enviado
    primera_libre       = 0, // índice de primera celda libre
    primera_ocupada     = 0, // índice de primera celda ocupada
    num_celdas_ocupadas = 0, // número de celdas ocupadas
    tag,                     // tag del emisor aceptable 
    id_emisor;               // identificacdor del mensaje recibido
  
  MPI_Status estado ;                 // metadatos del mensaje recibido

  for( unsigned int i=0 ; i < num_items*2 ; i++ ) //*2 ya que por cada items recibirá: una escritura en buffer y una petición lectura 
   {
      // 1. determinar si puede enviar solo prod., solo cons, o todos

      if ( num_celdas_ocupadas == 0 )               // si buffer vacío
	tag = tag_productor;                        // tag solo prod.
      else if ( num_celdas_ocupadas == tam_vector ) // si buffer lleno
	tag = tag_consumidor;                       // tag solo cons.
      else                                          // si no vacío ni lleno
	tag = MPI_ANY_TAG;   // tag  cualquiera

      
      // 2. recibir un mensaje del emisor o emisores aceptables
      MPI_Recv( &valor, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &estado );

      // 3. procesar el mensaje recibido
      
      id_emisor = estado.MPI_SOURCE; //leer emisor del mensaje en los metadatos
      //también se podría hace con un switch con los metadatos del tag
      if( id_emisor >= id_productor_inf && id_emisor <= id_productor_sup) {// si está en el rango de los productores, que es menor que el de los consumidores 

	buffer[primera_libre] = valor ;
	primera_libre = (primera_libre+1) % tam_vector ;
	num_celdas_ocupadas++ ;
	cout << "\t\t\tBuffer recibe " << valor << " de productor " << id_emisor - id_productor_inf<< endl ;
	
      }
      else {
	  valor = buffer[primera_ocupada] ;
	  primera_ocupada = (primera_ocupada+1) % tam_vector ;
	  num_celdas_ocupadas-- ;
	  cout << "\t\t\tBuffer envía " << valor << " a consumidor " << id_emisor - id_consumidor_inf << endl <<flush ;
	  MPI_Ssend( &valor, 1, MPI_INT, id_emisor, 0,   MPI_COMM_WORLD);
      }
  
   }
}

// muestra este mensje en el buffer para que sea único
void mensajeInicial() {
     cout << " ===========================================================" << endl
	<< "                PROBLEMA PRODUCTOR CONSUMIDOR " << endl
	<< " nº items: " << num_items << endl
	<< " nº productores: " << num_productores << endl
	<< " nº consumidores: " << num_consumidores << endl
        << " tamaño buffer: " << tam_vector << endl
	<< " ===========================================================" << endl;
   
}
// ---------------------------------------------------------------------

int main( int argc, char *argv[] )
{
   int id_propio, num_procesos_actual;


   // inicializar MPI, leer identif. de proceso y número de procesos
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
   MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );

   if ( num_procesos_esperados == num_procesos_actual )
   {
      // ejecutar la operación apropiada a 'id_propio'
     if ( id_productor_inf <= id_propio &&  id_productor_sup >= id_propio )    
       funcion_productor(id_propio - id_productor_inf); //ajustamos el id para que empiece a mostrar en 0
     else if ( id_propio == id_buffer ) {
       mensajeInicial(); 
       funcion_buffer();
     }
     else
       funcion_consumidor( id_propio - id_consumidor_inf); //ajustamos el id para que empiece a mostrar en 0
   }
   else
   {
      if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
      { cout << "el número de procesos esperados es:    " << num_procesos_esperados << endl
             << "el número de procesos en ejecución es: " << num_procesos_actual << endl
             << "(programa abortado)" << endl ;
      }
   }

   // al terminar el proceso, finalizar MPI
   MPI_Finalize( );
   return 0;
}
