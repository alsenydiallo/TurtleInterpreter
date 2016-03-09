.PHONY: all clean clobber

CXXFLAGS += -g -std=c++11 -Wall
ALL=turtle

all: $(ALL)

JUNK=*.o *~ *.dSYM scannertest turtle

clean:
	-rm -rf $(JUNK)

clobber:
	-rm -rf $(JUNK) $(ALL)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

scannertest: scannertest.o Scanner.o
	$(CXX) $(CXXFLAGS) $^ -o $@

turtle : turtle.o Scanner.o Env.o Parser.o
	$(CXX) $(CXXFLAGS) $^ -o $@

Scanner.o: Scanner.cpp Scanner.h
Env.o: Env.cpp Env.h
Parser.o: Parser.cpp Parser.h AST.h Env.h Scanner.h
turtle.o: turtle.cpp Parser.h AST.h Env.h Scanner.h
