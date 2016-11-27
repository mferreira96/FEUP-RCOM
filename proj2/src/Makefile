main: main.o connection.o url.o
	gcc -o main main.o connection.o url.o

main.o: main.c
	gcc -c main.c  -Wall

connection.o: connection.c
	gcc -c connection.c -Wall

url.o: url.c
	gcc -c url.c -Wall	
