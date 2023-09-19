# How to use this Makefile...
###################
###################
##               ##
##  $ make help  ##
##               ##
###################
###################

# IMPORTANT NOTES:
#   1. Set EXECUTABLE to the command name you want for your program.
#   2. Your program logic (where main() is) should be in a file named project*.cpp or
#      specified in the PROJECTFILE variable.

#######################
# User Configuration #
#######################
EXECUTABLE   = pathopt
PROJECTFILE  = $(or $(wildcard project*.cpp $(EXECUTABLE).cpp), main.cpp)
#######################
# End User Configuration #
#######################

# C++ Compiler Configuration
CXX         = g++
CXXFLAGS    = -std=c++17 -Wconversion -Wall -Werror -Wextra -pedantic

# Object Files
SOURCES     = $(wildcard *.cpp)
TESTSOURCES = $(wildcard test*.cpp)
SOURCES     := $(filter-out $(TESTSOURCES), $(SOURCES))
OBJECTS     = $(SOURCES:%.cpp=%.o)

# Rules
.PHONY: all clean debug release valgrind profile gprof static alltests

all: debug release profile valgrind

debug: CXXFLAGS += -g3 -DDEBUG -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG
debug:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o  $(EXECUTABLE)_debug

release: CXXFLAGS += -O3 -DNDEBUG
release: $(EXECUTABLE)

valgrind: CXXFLAGS += -g3
valgrind:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)_valgrind

profile: CXXFLAGS += -g3 -O3
profile:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)_profile

gprof: CXXFLAGS += -pg
gprof:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)_profile

static:
	cppcheck --enable=all $(SOURCES) *.h *.hpp

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLE)

# Unit Testing Support
TESTS       = $(TESTSOURCES:%.cpp=%)
define make_tests
    ifeq ($$(PROJECTFILE),)
        @echo Edit PROJECTFILE variable to .cpp file with main\(\)
        @exit 1
    endif
    SRCS = $$(filter-out $$(PROJECTFILE), $$(SOURCES))
    OBJS = $$(SRCS:%.cpp=%.o)
    HDRS = $$(wildcard *.h *.hpp)
    $(1): CXXFLAGS += -g3 -DDEBUG
    $(1): $$(OBJS) $$(HDRS) $(1).cpp
    $$(CXX) $$(CXXFLAGS) $$(OBJS) $(1).cpp -o $(1)
endef
$(foreach test, $(TESTS), $(eval $(call make_tests, $(test))))

alltests: $(TESTS)

# Cleanup
clean:
	rm -Rf *.dSYM
	rm -f $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE)_debug
	rm -f $(EXECUTABLE)_valgrind $(EXECUTABLE)_profile $(TESTS)
