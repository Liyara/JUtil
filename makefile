CXX := g++
INCLUDE_PATHS := -I./include/ -I/usr/include/
LIB_PATHS := -lpthread 
CXXFLAGS := -std=gnu++17 $(INCLUDE_PATHS) -Wfatal-errors
MODULES := $(basename $(notdir $(shell find src/ -name '*.cpp')))
SOURCES := $(addsuffix .cpp,$(addprefix src/,$(MODULES)))
OBJECTS := $(addsuffix .o,$(addprefix obj/,$(MODULES)))
INCLUDES := includes/%.h includes/%.hpp
TARGET := ./bin/JUtil
MAIN_OBJECT := obj/main.o
MAIN_SRC := main.cpp
STATIC_LIB := ./lib/libjutil.a

.PHONY: all debug release clean run

all: debug

debug: $(TARGET)

drun: debug run

gdb:
	gdb $(TARGET)

release: $(STATIC_LIB)

$(TARGET): clean $(MAIN_OBJECT) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(MAIN_OBJECT) $(OBJECTS) $(LIB_PATHS) -o $(TARGET)

$(STATIC_LIB): $(OBJECTS)
	ar rcs $(STATIC_LIB) $(OBJECTS)

obj/main.o: $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -c -o $(MAIN_OBJECT) $(MAIN_SRC)

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f obj/*.o
	rm -f $(TARGET)
	rm -f $(STATIC_LIB)
	rm -f winobj/*.o
	rm -f $(WIN_TARGET)

run:
	$(TARGET)