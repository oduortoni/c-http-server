OBJECTS=hlistener.o hhandlerfunc.o main.o

run: $(OBJECTS)
	cc -o bin/server $(OBJECTS)
	./bin/server

main.o:
	cc -o main.o -c main.c

hlistener.o:
	cc -o hlistener.o -c http_listener.c

hhandlerfunc.o:
	cc -o hhandlerfunc.o -c http_handlerfunc.c

clean:
	rm -f $(OBJECTS)
	rm -f bin/server
