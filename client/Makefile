INC = ../include
CFLAGS = -I$(INC) -I.

client: client.cpp clientConnection.cpp clientConnection.h $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(INC)/connection.h $(INC)/reliableUDP.h $(INC)/types.h
	g++ -o client -g client.cpp clientConnection.cpp $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(CFLAGS)
