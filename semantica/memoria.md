# Porqué en la semántica SC es necersario implentar un while en la espera

Véase la siguiente implementación de insertar  con if:
```c++

  void  insertar( int dato, int nHebra) {
    unique_lock<mutex> ulCompruebaLibreProd( mtxProductores);
    if ( primeraLibre ==TAM_BUFFER) {  // while ( primeraLibre ==TAM_BUFFER) {
      colaProductor.wait(ulCompruebaLibreProd);
    }
    
    mtxPila.lock();
    pila[primeraLibre++]=dato;
    cout << "Productor " << nHebra << " escribe " << dato
	 << "(datos en pila: " << primeraLibre <<")" << endl;
    mtxPila.unlock();

    colaConsumidor.notify_one(); 
  }
```
tras lanzar el código nos hemos encontrado este caso: 
```
------------------------------------------------------------------------------------
  Problema de los productores-consumidores (solución LIFO con monitor y semántica SC)
Nº de ítems: 40 | Nº consumidores: 4 | Nº productores: 4 | tamaño buffer: 3
--------------------------------------------------------------------------------------

roductor 2 escribe 20 datos en pila: 2
		consumido: 10
			Consumidor 1 extrae 20 datos en pila: 1
producido: 21
Productor 0 escribe 21 datos en pila: 2
producido: 22
Productor 1 escribe 22 datos en pila: 3
producido: 23
		consumido: 20
			Consumidor 1 extrae 22 datos en pila: 2
producido: 24
Productor 3 escribe 24 datos en pila: 3
Productor 0 escribe 23 datos en pila: 23

```


Error dectado con un while

```c++
./prodConsSCLIFO_exe
------------------------------------------------------------------------------------
  Problema de los productores-consumidores (solución LIFO con monitor y semántica SC)
Nº de ítems: 20 | Nº consumidores: 5 | Nº productores: 10 | tamaño buffer: 3
--------------------------------------------------------------------------------------
Consumir?? 0
Consumir?? 1
Consumir?? 2
Consumir?? 3
¿producir? 0
Productor 0 escribe 0(datos en pila: 1)
Consumir?? 4
			Consumidor 1 extrae 0 (datos en pila: 0)
¿producir? Consumir?? 5
¿producir? 1
Productor 3 escribe 1(datos en pila: 11
)
¿producir? 3
			Consumidor ¿producir? 3
1 extrae 1 (datos en pila: 0)
¿producir? Productor 2 escribe 2(datos en pila: 1)Consumir?? 
¿producir? 5
			Consumidor 2 extrae 2 (datos en pila: 0)
Consumir?? 6
Productor 3¿producir?  escribe 63(datos en pila: 1)

			Consumidor 2 extrae 3 (datos en pila: 0)
¿producir? Consumir?? 7
Productor 8 escribe 6¿producir? 7(datos en pila: 
1)
8
6
			Consumidor 3 extrae 6 (datos en pila: 0)
Consumir?? 8
6
Productor 2 escribe 5(datos en pila: 1)
¿producir? 10
¿producir? 11
¿producir? 12
Productor 3 escribe 7(datos en pila: 2)
¿producir? 13
			Consumidor 2 extrae 7 (datos en pila: 1)
Consumir?? 9
			Consumidor 2 extrae 5 (datos en pila: 0)
Consumir?? 10
¿producir? Productor 14
3 escribe 13(datos en pila: 1)
¿producir? 15
Productor 3 escribe 15(datos en pila: 2)
			Consumidor 0 extrae 15 (datos en pila: 1)
Consumir?? 11
			Consumidor 0 extrae 13 (datos en pila: 0)
Consumir?? 12
¿producir? 16
¿producir? 17
¿producir? 18
Productor 3 escribe 18(datos en pila: 1)
¿producir? 19
Productor 3 escribe 19(datos en pila: 2)
¿producir? 20
Productor 7 escribe 10(datos en pila: 3)
			Consumidor 3 extrae 10 (datos en pila: 2)
Consumir?? 13
			Consumidor 3 extrae 19 (datos en pila: 1)
Consumir?? 14
			Consumidor 3 extrae 18 (datos en pila: 0)
Consumir?? 15
¿producir? 20
Productor 2 escribe 11(datos en pila: 1)
¿producir? 20
			Consumidor 4 extrae 11 (datos en pila: 0)
Consumir?? 17
Productor 8 escribe 12(datos en pila: 1)
			Consumidor 4 extrae 12 (datos en pila: 0)
Consumir?? ¿producir? 20
18
Productor 0 escribe 4(datos en pila: 1)
			Consumidor 0 extrae 4 (datos en pila: 0)
Consumir?? 19
¿producir? 20
Productor 1 escribe 14(datos en pila: 1)
¿producir? 20
			Consumidor 2 extrae 14 (datos en pila: 0)
Consumir?? 20
Productor 5 escribe 8(datos en pila: 1)
¿producir? 20
Productor 4 escribe 16(datos en pila: 2)
			Consumidor 1 extrae 16 (datos en pila: 1)
¿producir? 20
			Consumidor 4 extrae 8 (datos en pila: 0)
Consumir?? Consumir?? 20Productor 6 escribe 17
(datos en pila: 1)
20
			Consumidor 3 extrae 17 (datos en pila: 0)
Consumir?? 20
¿producir? 20
Productor 9 escribe 9(datos en pila: 1)
¿producir? 20
			Consumidor 0 extrae 9 (datos en pila: 0)
Consumir?? 20
comprobando contadores ....error: valor 4 producido 0 veces

```


Otro error 

```------------------------------------------------------------------------------------
  Problema de los productores-consumidores (solución LIFO con monitor y semántica SC)
Nº de ítems: 50 | Nº consumidores: 5 | Nº productores: 10 | tamaño buffer: 3
--------------------------------------------------------------------------------------
producido: 0
Productor 0 escribe 0(datos en pila: 1)
producido: 1
Productor 0 escribe 1(datos en pila: 2)
producido: 2
			Consumidor producido: 2
producido: 1 extrae 1 (datos en pila: 1)
producido: 2
2
		consumido: 			Consumidor 1
producido: 5
producido: 0 extrae 0 (datos en pila: 0)
producido: 7
5
producido: 		consumido: 0
8Productor 
producido: 310 escribe 
producido: 10
2(datos en pila: 1)
			Consumidor 0 extrae 2 (datos en pila: 0)
producido: 12
Productor 1 escribe 3(datos en pila: 1)
		consumido: 2
producido: 13
			Consumidor 3 extrae 3 (datos en pila: 0)
Productor 2 escribe 		consumido: 34
(datos en pila: 1)
			Consumidor 2 extrae 4 (datos en pila: 0)
producido: 14
Productor 4 escribe 5(datos en pila: 1)
			Consumidor 0 extrae 5 (datos en pila: 0)
producido: 15
Productor 0 escribe 6(datos en pila: 1)
Productor 4 escribe 15(datos en pila: 2)
		consumido: 		consumido: 4
5
producido: 16			Consumidor 
producido: 17
3 extrae 15 (datos en pila: 1)
		consumido: 15
Productor 6 escribe 8(datos en pila: 2)
			Consumidor 4 extrae 8 (datos en pila: 1)
producido: 18
Productor 7 escribe 9(datos en pila: 2)
		consumido: 8
			Consumidor 1 extrae 9 (datos en pila: 1)
producido: 19
			Consumidor 2 extrae 6 (datos en pila: 0)
		consumido: 6
		consumido: 9
Productor 9 escribe 10(datos en pila: 1)
			Consumidor 2 extrae 10 (datos en pila: 0)
		consumido: 10producido: 
20
Productor 8 escribe 11(datos en pila: 1)
producido: 21
			Consumidor 0 extrae 11 (datos en pila: 0)
		consumido: 11
Productor 3 escribe 12(datos en pila: 1)
			Consumidor 0 extrae 12 (datos en pila: 0)
producido: 22
Productor 1 escribe 13(datos en pila: 1)		consumido: 
12
			Consumidor 3 extrae 13 (datos en pila: 0)
producido: 23
		consumido: 13
Productor 2 escribe 14(datos en pila: 1)
			Consumidor 0 extrae 14 (datos en pila: 0)
producido: 24
Productor 1 escribe 23(datos en pila: 1)
		consumido: 14
			Consumidor 4 extrae 23 (datos en pila: 0)
		consumido: 23
producido: 25
Productor 1 escribe 25(datos en pila: 1)
producido: 26
Productor 1 escribe 26(datos en pila: 2)
			Consumidor producido: 3 extrae 2726 (datos en pila: 1)

		consumido: 26
Productor 0 escribe 17(datos en pila: 2)
			Consumidor 3 extrae 17 (datos en pila: 1)
producido: 28
Productor 0 escribe 28(datos en pila: 2)
		consumido: 17
producido: 			Consumidor 3 extrae 28 (datos en pila: 1)
		consumido: 28
			Consumidor 3 extrae 25 (datos en pila: 0)
		consumido: 25
29
Productor 6 escribe 18(datos en pila: 1)
producido: 30
Productor 7 escribe 19(datos en pila: 2)
			Consumidor 1 extrae 19 (datos en pila: 1)
producido: 31			Consumidor 		consumido: 19
4 extrae 18 (datos en pila: 0)
Productor 9 escribe 20(datos en pila: 1)
			Consumidor 2 extrae 20 (datos en pila: 0)

producido: 32
		consumido: 20
		consumido: 18
Productor 8 escribe 21(datos en pila: 1)
			Consumidor 1 extrae 21 (datos en pila: 0)
producido: 33
Productor 3 escribe 22(datos en pila: 1)
			Consumidor 4 extrae 22 (datos en pila: 0)
producido: Productor 2 escribe 24(datos en pila: 34
		consumido: 121)

		consumido: 22
producido: 35
			Consumidor 3 extrae 24 (datos en pila: 0)
Productor 5 escribe 7(datos en pila: 		consumido: 241)

			Consumidor 0 extrae 7 (datos en pila: 0)
Productor 4 escribe 16(datos en pila: 1		consumido: 7
)
producido: 36
producido: 37
Productor 1 escribe 27(datos en pila: 2)
			Consumidor 1 extrae 27 (datos en pila: 1)
producido: 38
Productor 0 escribe 29(datos en pila: 2)
		consumido: 27
			Consumidor 2 extrae 29 (datos en pila: 1)
producido: 39
Productor 6 escribe 30(datos en pila: 2)
		consumido: 29
			Consumidor 3 extrae 30 (datos en pila: 1)
producido: 40
			Consumidor 		consumido: 430
 extrae 16 (datos en pila: 0)
Productor 		consumido: 16
7 escribe 31(datos en pila: 1)
producido: 			Consumidor 410 extrae 31 (datos en pila: 
0)
Productor 9 escribe 32(datos en pila: 1)
		consumido: 31
producido: 42
			Consumidor 1 extrae 32 (datos en pila: 0)
Productor 8 escribe 33(datos en pila: 1)
		consumido: 32
			Consumidor 2 extrae 33 (datos en pila: 0)
producido: 43
Productor 3 escribe 34(datos en pila: 1)
		consumido: 33
			Consumidor 3 extrae 34 (datos en pila: 0)
producido: 44
		consumido: 34
Productor 2 escribe 35(datos en pila: 1)
			Consumidor 4 extrae 35 (datos en pila: 0)
producido: 45
		consumido: 35
Productor 5 escribe 36(datos en pila: 1)
			Consumidor 4 extrae 36 (datos en pila: 0)
producido: 46
Productor 		consumido: 36
4 escribe 37(datos en pila: 1)
			Consumidor 2 extrae 37 (datos en pila: 0)
producido: 47
Productor 1 escribe 38(datos en pila: 1)
		consumido: 37
			Consumidor 3 extrae 38 (datos en pila: 0)
producido: 48
Productor 0 escribe 39(datos en pila: 1)
		consumido: 38producido: 48

Productor 6 escribe 40(datos en pila: 2)
			Consumidor 1 extrae 40 (datos en pila: 1)
		consumido: 40
Productor 7 escribe 41(datos en pila: 2)
			Consumidor 1 extrae 41 (datos en pila: 1)
Productor 9 escribe 42		consumido: (datos en pila: 412)

			Consumidor 4 extrae 42 (datos en pila: 1)
		consumido: 42
			Consumidor 2 extrae 39 (datos en pila: 0)
Productor 8 escribe 43(datos en pila: 1)
		consumido: 39
			Consumidor 3 extrae 43 (datos en pila: 0)
		consumido: 43
Productor 3 escribe 44(datos en pila: 1)
			Consumidor 3 extrae 44 (datos en pila: 0)
		consumido: Productor 442 escribe 45(datos en pila: 1)

Productor 5 escribe 46(datos en pila: 2)
Productor 4 escribe 47(datos en pila: 3)
			Consumidor 1 extrae 47 (datos en pila: 2)
Productor 0 escribe 48(datos en pila: 3)		consumido: 
47
			Consumidor 4 extrae 48 (datos en pila: 2)
Productor 1 escribe 49(datos en pila: 		consumido: 348
)
			Consumidor 2 extrae 49 (datos en pila: 2)
		consumido: 49
			Consumidor 3 extrae 46 (datos en pila: 1)
		consumido: 46
			Consumidor 0 extrae 45 (datos en pila: 0)
		consumido: 45
    

```


## Error productor FIFO

```
/prodConsSCFIFO_exe
------------------------------------------------------------------------------------
  Problema de los productores-consumidores (solución FIFO con monitor y semántica SC)
Nº de ítems: 40 | Nº consumidores: 5 | Nº productores: 10 | tamaño buffer: 4
--------------------------------------------------------------------------------------
 Productor(0) inserta 0
 Productor(0) inserta 1
			Consumidor(0) extrae 0
			Consumidor(0) extrae 1
 Productor(0) inserta 2
			Consumidor(2) extrae 2
 Productor(0) inserta 3
			Consumidor(3) extrae 3
 Productor(0) inserta 4
			Consumidor(4) extrae 4
 Productor(0) inserta 5
			Consumidor(1) extrae 5
 Productor(0) inserta 6
			Consumidor(0) extrae 6
 Productor(0) inserta 7
 Productor(0) inserta 8
			Consumidor(3) extrae 7
 Productor(0) inserta 9
			Consumidor(3) extrae 8
 Productor(0) inserta 10
			Consumidor(3) extrae 9
			Consumidor(3) extrae 10
 Productor(0) inserta 11
			Consumidor(1) extrae 11
 Productor(0) inserta 12
 Productor(4) inserta 13
			Consumidor(3) extrae 12
 Productor(4) inserta 14
			Consumidor(3) extrae 13
 Productor(9) inserta 15
			Consumidor(3) extrae 14
 Productor(9) inserta 16
			Consumidor(3) extrae 15
			Consumidor(3) extrae 16
 Productor(9) inserta 17
			Consumidor(2) extrae 17
 Productor(9) inserta 18
			Consumidor(4) extrae 18
 Productor(9) inserta 19
			Consumidor(1) extrae 19
 Productor(9) inserta 20
			Consumidor(2) extrae 20
 Productor(9) inserta 21
			Consumidor(0) extrae 21
 Productor(9) inserta 22
			Consumidor(3) extrae 22
 Productor(9) inserta 23
			Consumidor(4) extrae 23
 Productor(9) inserta 24
			Consumidor(1) extrae 24
 Productor(9) inserta 25
			Consumidor(2) extrae 25
 Productor(9) inserta 26
			Consumidor(0) extrae 26
 Productor(9) inserta 27
			Consumidor(3) extrae 27
 Productor(9) inserta 28
 Productor(9) inserta 29
			Consumidor(4) extrae 28
 Productor(9) inserta 30
			Consumidor(4) extrae 29
 Productor(9) inserta 31
			Consumidor(4) extrae 30
			Consumidor(4) extrae 31
¡¡¡Productor 9 finaliza su tarea!!!
 Productor(4) inserta 32
			Consumidor(3) extrae 32
¡¡¡Productor 4 finaliza su tarea!!!
 Productor(6) inserta 33
			Consumidor(¡¡¡Productor 64 finaliza su tarea!!!
) extrae 33
 Productor(7) inserta 34
			Consumidor(1) extrae ¡¡¡Productor 347 finaliza su tarea!!!

 Productor(3) inserta 35
¡¡¡Productor 3 finaliza su tarea!!!
 Productor(0) inserta 36
¡¡¡Productor 0 finaliza su tarea!!!
			Consumidor(0) extrae 35
		¡¡¡Consumidor 0 finaliza su tarea !!!
			Consumidor(2) extrae 36
		¡¡¡Consumidor 2 finaliza su tarea !!!
 Productor(2) inserta 37
¡¡¡Productor 			Consumidor(23) extrae 37 finaliza su tarea!!!

		¡¡¡Consumidor 3 finaliza su tarea !!!
 Productor(5) inserta 38
			Consumidor(4) extrae 38
		¡¡¡Consumidor 4 finaliza su tarea !!!
¡¡¡Productor 5 finaliza su tarea!!!
 Productor(1) inserta 39
¡¡¡Productor 1 finaliza su tarea!!!
 Productor(8) inserta 40
¡¡¡Productor 8 finaliza su tarea!!!
			Consumidor(1) extrae 39
		¡¡¡Consumidor 1 finaliza su tarea !!!
comprobando contadores ....error: valor 0 consumido 2 veces

```
