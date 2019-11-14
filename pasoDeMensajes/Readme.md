# Para compilar y ejecutar  
mpicxx holamundo.cpp -o holamundo.out
mpirun -np 20 --oversubscribe  holamundo.out

El oversubcribe se debe a que si escribimos más hebras de las que soporta 

