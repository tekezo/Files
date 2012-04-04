include ../Makefile.inc

all: run

a.out: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

run: a.out
	@./a.out

clean:
	rm -f *.o a.out
