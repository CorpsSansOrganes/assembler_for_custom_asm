# Compiler
CC := gcc

# Release build flags
CFLAGS := -Wall -ansi -pedantic

# Debug build flags
CFLAGS := -g -Wall -ansi -pedantic

# Directories
SRC := ./src
TEST := ./test
OBJ_RELEASE := ./obj/release
OBJ_DEBUG := ./obj/debug
INCLUDE := ./include

# Dependencies
LIST_OBJ := list.o list_test.o

# By default, build in release mode
mode := RELEASE

# Set compilation flags & obj directory according to build mode.
MODE_UPPER := $(shell echo $(mode) | tr '[:lower:]' '[:upper:]')
ifeq ($(MODE_UPPER), RELEASE)
	OBJ := $(OBJ_RELEASE)
	FLAGS := $(CFLAGS)
else ifeq ($(MODE_UPPER), DEBUG)
	FLAGS := $(DFLAGS)
	OBJ := $(OBJ_DEBUG)
else
	$(error Invalid mode: $(MODE_UPPER))
endif

# Executable recipe
all : list

list: $(addprefix $(OBJ)/, $(LIST_OBJ))
	@echo "Building $@ in $(MODE_UPPER) mode"
	$(CC) $(FLAGS) -I$(INCLUDE) -o $@_$(MODE_UPPER).out $^

# Object creation rules.
$(OBJ)/%.o: $(SRC)/%.cpp 
	$(CC) $(FLAGS) -I$(INCLUDE) -c $< -o $@

$(OBJ)/%_test.o: $(TEST)/%_test.cpp 
	$(CC) $(FLAGS) -I$(INCLUDE) -c $< -o $@

# Phony targets
clean:
	rm -rf $(OBJ_DEBUG)/*.o $(OBJ_RELEASE)/*.o ./*.out

.PHONY: all clean 
