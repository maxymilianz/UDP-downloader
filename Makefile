COMPILER = g++
FLAGS = -std=c++11 -Wall -Wextra


transport: transport.o socket_tools.o sliding_window.o segment_data.o segment_request.o
	$(COMPILER) -o transport transport.o socket_tools.o sliding_window.o segment_data.o segment_request.o $(FLAGS)

socket_tools.o: socket_tools.cpp socket_tools.h
	$(COMPILER) -o socket_tools.o -c socket_tools.cpp $(FLAGS)

segment_request.o: segment_request.cpp segment_request.h
	$(COMPILER) -o segment_request.o -c segment_request.cpp $(FLAGS)

segment_data.o: segment_data.cpp segment_data.h
	$(COMPILER) -o segment_data.o -c segment_data.cpp $(FLAGS)

sliding_window.o: sliding_window.cpp sliding_window.h
	$(COMPILER) -o sliding_window.o -c sliding_window.cpp $(FLAGS)

transport.o: transport.cpp transport.h
	$(COMPILER) -o transport.o -c transport.cpp $(FLAGS)

clean:
	rm *.o

distclean:
	rm *.o transport