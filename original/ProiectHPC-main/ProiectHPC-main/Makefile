CC = cc
CFLAGS = -O1 -g -fopenmp -fno-omit-frame-pointer -fasynchronous-unwind-tables
LDLIBS = -lm
OBJ = acoustics.o import_data.o export_data.o acoustics_alg.o utils.o

all: acoustics
	@mkdir -p export
acoustics: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o acoustics $(LDLIBS)
clean:
	rm -f *~ *.o
