CXX := g++
INCLUDE_PATHS := -I./include/
CXXFLAGS := -std=gnu++17 $(INCLUDE_PATHS) -lpthread -Wfatal-errors
MODULES := $(basename $(notdir $(shell find src/ -name '*.cpp')))
SOURCES := $(addsuffix .cpp,$(addprefix src/,$(MODULES)))
OBJECTS := $(addsuffix .o,$(addprefix obj/,$(MODULES)))
INCLUDES := includes/%.h includes/%.hpp
CXX_LIB := ar rvs
DEBUG_TARGET := ./bin/jutil
RELEASE_TARGET := ./bin/libjutil.a
MAIN_OBJECT :=	obj/main.o
MAIN_SOURCE := main.cpp

debug: $(MAIN_OBJECT) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(DEBUG_TARGET) $(MAIN_OBJECT) $(OBJECTS)

release: $(OBJECTS)
	$(CXX_LIB) $(RELEASE_TARGET) $(OBJECTS)

obj/main.o: $(MAIN_SOURCE)
	$(CXX) $(CXXFLAGS) -c -o $(MAIN_OBJECT) $(MAIN_SOURCE)

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f obj/*.o
	rm -f bin/*

run:
	$(DEBUG_TARGET)