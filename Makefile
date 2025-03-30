CC = tcc
CFLAGS = -m32 -Os -Wall -fomit-frame-pointer -ffast-math -fdata-sections -ffunction-sections -DNDEBUG -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -s -Isrc/include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
ifeq ($(OS),Windows_NT)
  TARGET = $(BIN_DIR)/bfi.exe
  RM = del /F /Q
  RMDIR = rd /S /Q
  MKDIR = mkdir
  PATH_SEP = \\
  NULL_DEV = nul
  MKDIR_CMD = if not exist $(1) $(MKDIR) $(1)
  BIN_DIR_WIN = $(subst /,$(PATH_SEP),$(BIN_DIR))
  OBJ_DIR_WIN = $(subst /,$(PATH_SEP),$(OBJ_DIR))
  TARGET_WIN = $(subst /,$(PATH_SEP),$(TARGET))
else
  TARGET = $(BIN_DIR)/bfi
  RM = rm -f
  RMDIR = rm -rf
  MKDIR = mkdir -p
  PATH_SEP = /
  NULL_DEV = /dev/null
  MKDIR_CMD = $(MKDIR) $(1)
  BIN_DIR_WIN = $(BIN_DIR)
  OBJ_DIR_WIN = $(OBJ_DIR)
  TARGET_WIN = $(TARGET)
endif
.PHONY: all clean dirs
all: dirs $(TARGET)
dirs:
	-@$(call MKDIR_CMD,$(BIN_DIR_WIN)) 2>$(NULL_DEV)
	-@$(call MKDIR_CMD,$(OBJ_DIR_WIN)) 2>$(NULL_DEV)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^
ifneq ($(OS),Windows_NT)
	-strip -s $@ 2>$(NULL_DEV)
endif
clean:
ifeq ($(OS),Windows_NT)
	-@if exist $(OBJ_DIR_WIN)\*.o $(RM) $(OBJ_DIR_WIN)\*.o 2>$(NULL_DEV)
	-@if exist $(TARGET_WIN) $(RM) $(TARGET_WIN) 2>$(NULL_DEV)
else
	-$(RM) $(OBJ_DIR)/*.o 2>$(NULL_DEV)
	-$(RM) $(TARGET) 2>$(NULL_DEV)
endif