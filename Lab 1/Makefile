CXX := g++
CXXFLAGS := -O3 -std=c++20 -Wno-narrowing

SRCDIR = src
ZADPREF = zad

TARGETS := $(patsubst %, $(ZADPREF)%, 1 2 3 4 5)

%:
	$(CXX) $(CXXFLAGS) -o $(ZADPREF)$@ $(SRCDIR)/$(ZADPREF)$@.cpp

clean:
	rm -f $(TARGETS) 
