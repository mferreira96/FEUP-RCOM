main: main.o applicationLayer.o DataLink.o Cli.o
	gcc -o main main.o applicationLayer.o DataLink.o Cli.o

main.o: main.c
	gcc -c main.c

applicationLayer.o: applicationLayer.c
	gcc -c applicationLayer.c -Wall

DataLink.o: DataLink.c
	gcc -c DataLink.c -Wall

Cli.o: Cli.c
	gcc -c Cli.c -Wall

