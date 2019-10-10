.SUFFIXES:
.PHONY: exeall
.PRECIOUS: %_exe

compilador:=g++

varfifo: prodcons_varios_consumidores_productores_exe
	./$<

varlifo: prodcons_varios_consumidores_productores_exe
	./$< 

lifopc: prodconsLIFO_exe
	./$< 
fifopc: prodconsFIFO_exe
	./$<

fu: fumadores_exe
	./$<

%_exe: %.cpp
	$(compilador) -std=c++11 -pthread -o $@ $< Semaphore.cpp

exeall: lifopc fifopc


clean:
	rm -f *_exe *~

