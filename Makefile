all: cbp station

cbp: cbp.c
	gcc -o cbp cbp.c -Wall

station: station.c
	gcc -o station station.c -Wall

clean:
	rm -f cbp station *.o logs/*
	killall cbp