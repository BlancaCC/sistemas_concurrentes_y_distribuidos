.SUFFIXES:
.PHONY: 
.PRECIOUS: %_exe

compilador:=g++

sclifo: prodConsSCLIFO_exe
	./$<

%_exe: %.cpp plantilla.h
	$(compilador) -std=c++11 -pthread -o $@ $< 



clean:
	rm -f *_exe *~

