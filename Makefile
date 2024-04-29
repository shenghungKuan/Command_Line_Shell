# Output binary name
bin=shshsh
lib=libshell.so

# Set the following to '0' to disable log messages:
LOGGER ?= 0

# Compiler/linker flags
CFLAGS += -g -Wall -fPIC -DLOGGER=$(LOGGER)
LDLIBS += -lm -lreadline
LDFLAGS += -L. -Wl,-rpath='$$ORIGIN'

src=history.c shshsh.c elist.c
obj=$(src:.c=.o)

all: $(bin) $(lib)

$(bin): $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -o $@

$(lib): $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -shared -o $@

shshsh.o: shshsh.c history.h logger.h elist.h
history.o: history.c history.h logger.h elist.h
elist.o: elist.c elist.h

clean:
	rm -f $(bin) $(obj) $(lib)

