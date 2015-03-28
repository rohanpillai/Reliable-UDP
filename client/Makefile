INC = ../include
CFLAGS = -I$(INC)

client: client.cpp $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(INC)/connection.h $(INC)/reliableUDP.h $(INC)/types.h
	g++ -o client client.cpp $(INC)/connection.cpp $(INC)/reliableUDP.cpp $(CFLAGS)
