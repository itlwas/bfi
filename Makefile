CC = tcc
CFLAGS = -m32 -Os -Wall -fomit-frame-pointer -ffast-math -fdata-sections -ffunction-sections -DNDEBUG -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -s
SRCFILES = main.c memory.c logger.c fileio.c syntax.c interpreter.c utils.c
TARGET = bfi.exe
.PHONY: all clean strip_bin
all: clean $(TARGET) strip_bin
$(TARGET): $(SRCFILES)
	@del $@ 2>nul || true
	$(CC) $(CFLAGS) -o $@ $^
clean:
	@if exist $(TARGET) del $(TARGET)