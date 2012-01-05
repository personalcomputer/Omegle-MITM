OBJS = ChatLog.o ChatRoom.o MitmLogger.o SpamDefinitions.o main.o
INCLUDEDIR = /usr/include/
LIBDIR = /usr/lib/
FLAGS = -lboost_filesystem -lboost_system -lboost_regex -Wall -g -iquote$(INCLUDEDIR) -L$(LIBDIR) -lomegle -Wl,-rpath,$(LIBDIR) -std=c++0x

../Omegle-MITM: $(OBJS)
	g++ $(OBJS) $(FLAGS) -o ../Omegle-MITM

ChatLog.o: ChatLog.cpp ChatLog.h
	g++ $(FLAGS) -c ChatLog.cpp -o ChatLog.o

ChatRoom.o: ChatRoom.cpp ChatLog.h ChatRoom.h Error.h SpamDefinitions.h
	g++ $(FLAGS) -c ChatRoom.cpp -o ChatRoom.o

MitmLogger.o: MitmLogger.cpp ChatLog.h ChatRoom.h Error.h MitmLogger.h SpamDefinitions.h
	g++ $(FLAGS) -c MitmLogger.cpp -o MitmLogger.o

SpamDefinitions.o: SpamDefinitions.cpp SpamDefinitions.h
	g++ $(FLAGS) -c SpamDefinitions.cpp -o SpamDefinitions.o

main.o: main.cpp MitmLogger.h
	g++ $(FLAGS) -c main.cpp -o main.o

clean:
	rm -f ../Omegle-MITM $(OBJS)
