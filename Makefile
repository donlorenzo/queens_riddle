CC=g++
LIBS=-lalleg


all: queens2.0 queens2.5

queens2.0: queens2.0.cpp
	$(CC) $? $(LIBS) -o $@

queens2.5: queens2.5.cpp
	$(CC) $? $(LIBS) -o $@

clean:
	-rm queens2.0 queens2.5

.phony: all clean
