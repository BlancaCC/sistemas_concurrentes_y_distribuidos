.SUFFIXES:
.PHONY: 
.PRECIOUS: %.out

compilador:=g++
opcionesc:= -std=c++11 -pthread #-Wfatal-errors
hmonsrcs:= HoareMonitor.h HoareMonitor.cpp Semaphore.h Semaphore.cpp

## reglas de ejecución
# _____ semánticas SU  _____ 
sulifo: prodConsSULIFO.out
	./$<

sufifo: prodConsSUFIFO.out
	./$<
tijeras: tijeras.out
	./$<

dosBarberos: dosBarberos.out
	./$<
varClientes: varClientes.out
	./$<
barberia2: barberia2.out
	./$<

# _____ semánticas SU  _____

barberia2.out: barberia2.cpp $(hmonsrcs)
	$(compilador) $(opcionesc) -o $@ $< HoareMonitor.cpp Semaphore.cpp

varClientes.out: varClientes.cpp $(hmonsrcs)
	$(compilador) $(opcionesc) -o $@ $< HoareMonitor.cpp Semaphore.cpp

dosBarberos.out: dosBarberos.cpp $(hmonsrcs)
	$(compilador) $(opcionesc) -o $@ $< HoareMonitor.cpp Semaphore.cpp

tijeras.out: tijeras.cpp $(hmonsrcs)
	$(compilador) $(opcionesc) -o $@ $< HoareMonitor.cpp Semaphore.cpp

prodConsSUFIFO.out: prodConsSUFIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp
prodConsSULIFO.out: prodConsSULIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp




clean:
	rm -f *.out *~ *.pdf

