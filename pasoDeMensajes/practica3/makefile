.SUFFIXES:
.PHONY:    pc,pc2,pcm,fp,f,fi,fc,clean

compilador := mpicxx -std=c++11
flagsc     := -g -Wall 

mypc: prodcons3_exe
	mpirun --oversubscribe -np  9 ./$<
pc: prodcons_exe
	mpirun --oversubscribe -np  3 ./$<

pc2: prodcons2_exe
	mpirun --oversubscribe -np  3 ./$<

pcm: prodcons2-mu_exe
	mpirun  --oversubscribe -np  10 ./$<

fp: filosofos-plantilla_exe
	mpirun  --oversubscribe -np 10 ./$<

fi: filosofos-interb_exe
	mpirun  --oversubscribe -np 10 ./$<

f: filosofos_exe
	mpirun  --oversubscribe -np 10 ./$<

fc: filosofos-cam_exe
	mpirun  --oversubscribe -np  11 ./$<

%_exe: %.cpp
	$(compilador) $(flagsc) -o $@ $<

clean:
	rm -rf *_exe *.dSYM *.out *~
