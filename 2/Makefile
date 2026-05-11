CC = cc
CFLAGS = -O1 -g -fopenmp -fno-omit-frame-pointer -fasynchronous-unwind-tables
LDLIBS = -lm

OBJ = Main.o ScenarioIO.o ScenarioAlgorithm.o Utils.o

all: acoustics
	@mkdir -p export
acoustics: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o acoustics $(LDLIBS)
clean:
	rm -rf *- *.o
