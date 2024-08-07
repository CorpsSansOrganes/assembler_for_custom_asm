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
LIST_OBJ := list.o
FILE_HANDLING_OBJ := file_handling.o file_handling_test.o 
LINTING_OBJ := linting.o file_handling.o
SYMBOL_TABLE_OBJ := $(LIST_OBJ) symbol_table.o
PREPROCESSING_OBJ := macro_table.o string_utils.o preprocessing.o

TEST_LIST_OBJ := $(LIST_OBJ) list_test.o test_utils.o
TEST_FILE_HANDLING_OBJ := $(FILE_HANDLING_OBJ) file_handling_test.o 
TEST_LINTING_OBJ := $(LINTING_OBJ) linting_test.o test_utils.o
TEST_SYMBOL_TABLE := $(SYMBOL_TABLE_OBJ) symbol_table_test.o test_utils.o

# ----------
# Executables
#  ---------
# File Handling test rule
test_file_handling: $(addprefix $(OBJ_DEBUG)/, $(TEST_FILE_HANDLING_OBJ))
	$(CC) $(CFLAGS_DEBUG) -o $@ $^ -I$(INCLUDE)

# List test rule
test_list: $(addprefix $(OBJ_DEBUG)/, $(TEST_LIST_OBJ))
	$(CC) $(CFLAGS_DEBUG) -o $@ $^ -I$(INCLUDE)

# Linting test rule
test_linting: $(addprefix $(OBJ_DEBUG)/, $(TEST_LINTING_OBJ))
	$(CC) $(CFLAGS_DEBUG) -o $@ $^ -I$(INCLUDE)

# Symbol table test
test_symbol_table: $(addprefix $(OBJ_DEBUG)/, $(TEST_SYMBOL_TABLE))
	$(CC) $(CFLAGS_DEBUG) -o $@ $^ -I$(INCLUDE)


# ----------
# Object files 
#  ---------
# Pattern for compiling source .o files
$(OBJ_DEBUG)/%.o: $(SRC)/%.c $(INCLUDE)/%.h
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@ -I$(INCLUDE)

# Pattern for compiling test .o files.
$(OBJ_DEBUG)/%_test.o: $(TEST)/%_test.c $(INCLUDE)/%.h
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@ -I$(INCLUDE)

# Compile test_utils.o
$(OBJ_DEBUG)/test_utils.o: $(TEST)/test_utils.c $(INCLUDE)/test_utils.h
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@ -I$(INCLUDE)

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DEBUG)/*.o test_*
