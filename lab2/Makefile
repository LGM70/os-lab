all: sellTickets.c calPrint.c
	gcc sellTickets.c -o sellTickets -lpthread
	gcc calPrint.c -o calPrint -lpthread

sellTickets: sellTickets.c
	gcc sellTickets.c -o sellTickets -lpthread

calPrint: calPrint.c
	gcc calPrint.c -o calPrint -lpthread

clean:
	rm sellTickets
	rm calPrint

