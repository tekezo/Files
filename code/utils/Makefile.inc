CXXFLAGS = -I../../include \
	-Wall \
	-Wextra \
	-Wmissing-prototypes \
	-Wshadow \
	-Werror \
	-O2 \
	-std=c++11

SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))
