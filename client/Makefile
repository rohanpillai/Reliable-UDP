INC = ../include
CFLAGS = -I$(INC) -I.

client: client.cpp clientConnection.cpp clientConnection.h $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(INC)/connection.h $(INC)/reliableUDP.h $(INC)/types.h
	g++ -o client client.cpp clientConnection.cpp $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(CFLAGS)
