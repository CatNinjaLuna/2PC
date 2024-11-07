CPPFLAGS = -std=c++20 -Wall -Werror -pedantic -ggdb -pthread
HDRS = UDPSocket.h TCPServer.h TCPClient.h
ICE3 = send_udp recv_udp dns_lookup ice3_recv ice3_send
ICE4 = participant coordinator

%.o : %.cpp $(HDRS)
	g++ $(CPPFLAGS) -c $< -o $@

ice4 : $(ICE4)

ice3 : $(ICE3)

participant : TCPServer.o participant.o
	g++ -lpthread $^ -o $@

coordinator : TCPClient.o coordinator.o
	g++ -lpthread $^ -o $@


clean :
	rm -f *.o $(ICE3) $(ICE4)

