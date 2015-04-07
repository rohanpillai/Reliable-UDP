INC = ../include
CFLAGS = -I$(INC) -I.

server: server.cpp serverConnection.cpp serverConnection.h $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(INC)/connection.h $(INC)/reliableUDP.h $(INC)/types.h
	g++ -o server -g server.cpp serverConnection.cpp $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(CFLAGS)
