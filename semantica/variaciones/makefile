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


# _____ semánticas SU  _____ 

dosBarberos.out: dosBarberos.cpp
	$(compilador) $(opcionesc) -o $@ $< HoareMonitor.cpp Semaphore.cpp
tijeras.out: tijeras.cpp $(hmonsrcs)
	$(compilador) $(opcionesc) -o $@ $< HoareMonitor.cpp Semaphore.cpp

prodConsSUFIFO.out: prodConsSUFIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp
prodConsSULIFO.out: prodConsSULIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp




clean:
	rm -f *.out *~ *.pdf

