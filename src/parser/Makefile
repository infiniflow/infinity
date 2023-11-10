CC    ?= clang
CXX   ?= clang++

EXE = query_cli

CDEBUG = -g -Wall

CXXDEBUG = -g -Wall

CSTD = -std=c99
CXXSTD = -std=c++20

CFLAGS = -Wno-deprecated-register -O0  $(CDEBUG) $(CSTD) 
CXXFLAGS = -Wno-deprecated-register -O0  $(CXXDEBUG) $(CXXSTD)


CPPOBJ = query_main query_driver
SOBJ =  query_parser query_lexer query_driver

FILES = $(addsuffix .cpp, $(CPPOBJ))

OBJS  = $(addsuffix .o, $(CPPOBJ))

CLEANLIST =  $(addsuffix .o, $(SOBJ)) $(OBJS) \
				 query_parser.cpp query_parser.h \
				 query_parser.output location.hh \
				 query_lexer.cpp $(EXE)\

.PHONY: all
all: query_cli

query_cli: $(FILES)
	$(MAKE) $(SOBJ)
	$(MAKE) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) query_parser.o query_lexer.o $(LIBS)

query_parser: query_parser.y
	bison -d -v -Wcounterexamples --defines=query_parser.h --output=query_parser.cpp query_parser.y
	$(CXX) $(CXXFLAGS) -c -I../../third_party/iresearch/core -I../../third_party/iresearch/external -I../../third_party/iresearch/external/frozen/include -I../../third_party/parallel-hashmap -o query_parser.o query_parser.cpp

query_lexer: query_lexer.l
	flex --outfile=query_lexer.cpp  $<
	$(CXX)  $(CXXFLAGS) -c -I../../third_party/iresearch/core -I../../third_party/iresearch/external -I../../third_party/iresearch/external/frozen/include -I../../third_party/parallel-hashmap query_lexer.cpp -o query_lexer.o

query_driver: query_driver.cpp
	$(CXX) $(CXXFLAGS) -c -I../../third_party/iresearch/core -I../../third_party/iresearch/external -I../../third_party/iresearch/external/frozen/include -I../../third_party/parallel-hashmap -o query_driver.o query_driver.cpp

.PHONY: clean
clean:
	rm -rf $(CLEANLIST)

