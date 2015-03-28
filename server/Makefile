INC = ../include
CFLAGS = -I$(INC)

server: server.cpp $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(INC)/connection.h $(INC)/reliableUDP.h $(INC)/types.h
	g++ -o server server.cpp $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(CFLAGS)
