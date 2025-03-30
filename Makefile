CC = tcc
CFLAGS = -m32 -Os -Wall -fomit-frame-pointer -ffast-math -fdata-sections -ffunction-sections -DNDEBUG -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -s
SRCFILES = main.c memory.c logger.c fileio.c syntax.c interpreter.c utils.c benchmark.c executor.c
ifeq ($(OS),Windows_NT)
TARGET = bfi.exe
RM_CMD = del /F /Q
NULL_DEV = nul
else
TARGET = bfi
RM_CMD = rm -f
NULL_DEV = /dev/null
endif
.PHONY: all clean
all: clean $(TARGET)
$(TARGET): $(SRCFILES)
	$(CC) $(CFLAGS) -o $@ $^
ifeq ($(OS),Windows_NT)
else
	-strip -s $@ >$(NULL_DEV) 2>$(NULL_DEV)
endif
clean:
	-$(RM_CMD) $(TARGET) >$(NULL_DEV) 2>$(NULL_DEV)