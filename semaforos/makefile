.SUFFIXES:
.PHONY: exeall
.PRECIOUS: %_exe

compilador:=g++

varfifo: varios_consFIFO_exe
	./$<

varlifo: varios_consLIFO_exe
	./$< 

lifopc: prodconsLIFO_exe
	./$< 
fifopc: prodconsFIFO_exe
	./$<

fu: fumadores_exe
	./$<

%_exe: %.cpp
	$(compilador) -std=c++11 -pthread -o $@ $< Semaphore.cpp

exeall: lifopc fifopc varfifo varlifo


clean:
	rm -f *_exe *~

