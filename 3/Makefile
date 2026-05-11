CC = cc
CFLAGS = -O3 -g -fopenmp -fno-omit-frame-pointer -fasynchronous-unwind-tables
LDLIBS = -lm

OBJ = Main.o ScenarioIO.o ScenarioAlgorithm.o Utils.o

acoustics: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o acoustics $(LDLIBS)
clean:
	rm -rf *- *.o
