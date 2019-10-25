.SUFFIXES:
.PHONY: 
.PRECIOUS: %_exe

compilador:=g++
opcionesc:= -std=c++11 -pthread #-Wfatal-errors
hmonsrcs:= HoareMonitor.h HoareMonitor.cpp Semaphore.h Semaphore.cpp

## reglas de ejecución
# _____ semánticas SC  _____ 
sclifo: prodConsSCLIFO_exe
	./$<

scfifo: prodConsSCFIFO_exe
	./$<

# _____ semánticas SU  _____ 
sulifo: prodConsSULIFO_exe
	./$<
sufifo: prodConsSUFIFO_exe
	./$<

fumadores: fumadores_exe
	./$<


## reglas de compilación

prodConsSCLIFO_exe: prodConsSCLIFO.cpp plantilla.h
	$(compilador) $(opcionesc) -o $@ $<
prodConsSCFIFO_exe: prodConsSCFIFO.cpp plantilla.h
	$(compilador) $(opcionesc) -o $@ $<


prodConsSUFIFO_exe: prodConsSUFIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp
prodConsSULIFO_exe: prodConsSULIFO.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp

fumadores_exe: fumadores.cpp $(hmonsrcs)
	$(compilador) $(opcionesc)  -o $@ $< HoareMonitor.cpp Semaphore.cpp

#%_exe: %.cpp plantilla.h
#	$(compilador) $(opcionesc) -o $@ $< 



clean:
	rm -f *_exe *~ *.pdf

