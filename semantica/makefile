.SUFFIXES:
.PHONY: 
.PRECIOUS: %_exe

compilador:=g++
opcionesc:= -std=c++11 -pthread #-Wfatal-errors
hmonsrcs:= HoareMonitor.h HoareMonitor.cpp Semaphore.h Semaphore.cpp

## reglas de ejecución

sclifo: prodConsSCLIFO_exe
	./$<

scfifo: prodConsSCFIFO_exe
	./$<

sufifo: prodConsSUFIFO_exe
	./$<

fumadores: fumadores_exe
	./$<


## reglas de compilación 
prodConsSUFIFO_exe: prodConsSUFIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp
fumadores_exe: fumadores.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp

#%_exe: %.cpp plantilla.h
#	$(compilador) $(opcionesc) -o $@ $< 



clean:
	rm -f *_exe *~ *.pdf

