include ../Makefile.inc

all: run

a.out: $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

run: a.out
	@./a.out

clean:
	rm -f *.o a.out
