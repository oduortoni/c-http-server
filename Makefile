OBJECTS=hlistener.o hhandlerfunc.o main.o hserver.o hhandle.o

go:
	make clean
	make compile
	make run
	
compile: $(OBJECTS)
	cc -o bin/server $(OBJECTS)

run:
	./bin/server

init:
	mkdir bin

main.o:
	cc -c main.c -o main.o

hhandle.o:
	cc -c http_handle.c -o hhandle.o

hserver.o:
	cc -c http_server.c -o hserver.o

hlistener.o:
	cc -c http_listener.c -o hlistener.o

hhandlerfunc.o:
	cc -c http_handlerfunc.c -o hhandlerfunc.o

clean:
	rm -f $(OBJECTS)
	rm -f bin/server
