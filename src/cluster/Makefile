SRCDIR := ../common
HEADERDIR := ../../include
OBJDIR := ../../obj

CC := g++
CFLAGS := -std=c++11 -Ofast -g3 -Wall -Wextra -I$(HEADERDIR)


BINARY := cluster

SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
SOURCES += cluster_app.cpp 
SOURCES += cluster_utils.cpp

OBJECTS := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))


$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BINARY)


$(OBJDIR):
	@mkdir $(OBJDIR)


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)



.PHONY: clean

clean:
	rm -rf output $(BINARY) $(OBJDIR)/*
