CXXFLAGS = -I../../ -Wall -Werror -O2
LDFLAGS =-F$(HOME)/Library/Frameworks -framework gtest \
	-L$(HOME)/Library/Frameworks/gtest.Framework/Resources \
	-lgtest_main

SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

all: run

a.out: $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

run: a.out
	@./a.out

clean:
	rm -f *.o a.out
