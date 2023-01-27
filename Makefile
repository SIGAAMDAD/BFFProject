CC        = g++
CFLAGS    = -Wall -Ofast
SDIR      = code
O         = obj
EXE       = bff
LIB       = libbffproject.so.0.1
LIBS      = -ldl

.PHONY: all clean clean.lib

all: $(EXE)

OBJS= \
	$(O)/bff.o \
	$(O)/bff_parse.o \
	$(O)/bff_parse_element.o \

$(O)/%.o: $(SDIR)/bff_file/%.cpp
	$(CC) $(CFLAGS) -shared -fPIC -o $@ -c $<
$(O)/main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIB): $(OBJS)
	$(CC) $(CFLAGS) -shared -fPIC $(OBJS) -o $(LIB)

$(EXE): $(O)/main.o $(LIB)
	$(CC) $(CFLAGS) $(OBJS) $(O)/main.o -o $(EXE) $(LIBS)

clean:
	rm $(O)/main.o
	rm $(EXE)

clean.lib:
	rm -rf $(OBJS)
	rm $(LIB)