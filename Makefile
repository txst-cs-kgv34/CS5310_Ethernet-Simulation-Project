all: cbp station

cbp: cbp.c wrapsock.c
	gcc -o cbp cbp.c wrapsock.c -Wall

station: station.c wrapsock.c
	gcc -o station station.c wrapsock.c -Wall

clean:
	rm -f cbp station *.o logs/*
	killall cbp