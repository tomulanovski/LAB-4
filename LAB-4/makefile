CFLAGS = -m32 -Wall -g

TASK4FLAGS = -fno-pie -fno-stack-protector

all: task1 task4

task1: hexeditplus.o
	gcc $(CFLAGS) hexeditplus.o -o task1

task4: digcount.o
	gcc $(CFLAGS) $(TASK4FLAGS) digcount.o -o task4

digcount.o: digcount.c
	gcc $(CFLAGS) $(TASK4FLAGS) -c digcount.c 

hexeditplus.o: hexeditplus.c
	gcc $(CFLAGS) -c hexeditplus.c

.PHONY: clean
	
clean:	
	rm *.o task1 task4