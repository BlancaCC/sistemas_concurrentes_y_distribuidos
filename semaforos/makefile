.SUFFIXES:
.PHONY: 
.PRECIOUS: %_exe

compilador:=g++

varfifo: prodcons_varios_consumidores_productores_exe
	./$<

varlifo: prodcons_varios_consumidores_productores_exe
	./$< lifo

lifopc: prodcons_exe
	./$< lifo
fifo: prodcons_exe
	./$<

fu: fumadores_exe
	./$<

%_exe: %.cpp
		$(compilador) -std=c++11 -pthread -o $@ $< Semaphore.cpp 

clean:
	rm -f *_exe
