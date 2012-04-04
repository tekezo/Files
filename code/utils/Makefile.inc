CXXFLAGS = -I../../include \
	-Wall \
	-Wextra \
	-Wmissing-prototypes \
	-Wshadow \
	-Werror \
	-O2 \
	-std=c++11 \
	-F$(HOME)/Library/Frameworks

LDFLAGS =-F$(HOME)/Library/Frameworks -framework gtest \
	-L$(HOME)/Library/Frameworks/gtest.Framework/Resources \
	-lgtest_main

SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))
