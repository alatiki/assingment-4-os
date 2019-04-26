all: cse4001_sync

cse4001_sync:
	g++ -Wall -g -o cse4001_sync cse4001_sync.cpp  -Wall -lpthread -lm

clean:
	rm cse4001_sync
	

