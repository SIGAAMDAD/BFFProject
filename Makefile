CC        = g++
CFLAGS    = -Ofast -Wall
SDIR      = code
O         = obj
EXE       = bff

.PHONY: all clean clean.lib

all: $(EXE)

OBJS= \
	$(O)/bff.o \
	$(O)/bff_parse.o \
	$(O)/bff_parse_element.o \
	$(O)/bff_compile.o

$(O)/%.o: $(SDIR)/bff_file/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
$(O)/main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

$(EXE): $(OBJS) $(O)/main.o
	$(CC) $(CFLAGS) $(OBJS) $(O)/main.o -o $(EXE)

clean:
	rm $(O)/main.o
	rm $(EXE)

clean.lib:
	rm -rf $(OBJS)