TARGET = lucc
BINDIR = ../bin
CFLAGS = -g -w

$(BINDIR)/$(TARGET):
	$(CC) $(CFLAGS) -o $@ lucc.c
	@echo $(TARGET)" Compiled successfully!"

clean:
	rm $(BINDIR)/$(TARGET)
