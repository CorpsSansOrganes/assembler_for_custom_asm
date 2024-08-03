# Compiler
CC := gcc

# Release build flags
CFLAGS_RELEASE := -Wall -ansi -pedantic

# Debug build flags
CFLAGS_DEBUG := -g -Wall -ansi -pedantic

# Directories
SRC := ./src
TEST := ./test
OBJ_RELEASE := ./obj/release
OBJ_DEBUG := ./obj/debug
INCLUDE := ./include

# Dependencies
LIST_OBJ := list.o list_test.o
FILE_HANDLING_OBJ := file_handling.o file_handling_test.o

# ----------
# Executables
#  ---------
# File Handling test rule
file_handling_test: $(OBJ_DEBUG)/file_handling.o $(OBJ_DEBUG)/file_handling_test.o
	$(CC) $(CFLAGS_DEBUG) -o $@ $^ -I$(INCLUDE)


# ----------
# Object files 
#  ---------
# Compile file_handling.o
$(OBJ_DEBUG)/file_handling.o: $(SRC)/file_handling.c $(INCLUDE)/file_handling.h
	@mkdir -p $(OBJ_DEBUG)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@ -I$(INCLUDE)

# Compile file_handling_test.o
$(OBJ_DEBUG)/file_handling_test.o: $(TEST)/file_handling_test.c $(INCLUDE)/file_handling.h
	@mkdir -p $(OBJ_DEBUG)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@ -I$(INCLUDE)


# Clean up build artifacts
clean:
	rm -rf $(OBJ_DEBUG)/*.o file_handling_test
