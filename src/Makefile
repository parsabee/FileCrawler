CFLAG= -W -Wall -g -lpthread
PROGS = file_crawler
OBJECTS = file_crawler.o iterator.o linkedlist.o orderedset.o regexp.o tsiterator.o tslinkedlist.o tsorderedset.o

all: $(PROGS)
file_crawler: $(OBJECTS)
	cc -o file_crawler $^

file_crawler.o: file_crawler.c regexp.h tslinkedlist.h tsorderedset.h tsiterator.h
iterator.o: iterator.c iterator.h
linkedlist.o: linkedlist.c linkedlist.h iterator.h
orderedset.o: orderedset.c orderedset.h iterator.h
regexp.o: regexp.c regexp.h
tsiterator.o: tsiterator.c tsiterator.h
tslinkedlist.o: tslinkedlist.c tslinkedlist.h linkedlist.h tsiterator.h iterator.h
tsorderedset.o: tsorderedset.c tsorderedset.h orderedset.h tsiterator.h iterator.h

clean:
	rm $(OBJECTS)