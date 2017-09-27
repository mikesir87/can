TARGET = can
CC = gcc
RM = rm -rf
RMDIR = rm -rf
MKDIR = mkdir -p
DEPS=$(OBJS:.o=.d)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

.PHONY all clean

OBJS = can.o dstring.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -fr *.o can


