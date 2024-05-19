all: main

main: client.o table.o main.o
	g++ main.o table.o client.o -o main.exe

main.o: main.cpp
	g++ -c main.cpp

table.o: client.o
	g++ -c table.cpp

client.o:
	g++ -c client.cpp

clean:
	rm -rf *.o main.exe
