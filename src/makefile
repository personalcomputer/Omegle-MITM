OBJS = ChatLog.o ChatRoom.o MitmLogger.o main.o
INCLUDEDIR = include #relative to source
LIBDIR = . #relative to binary
FLAGS = -lboost_filesystem -lboost_system -lboost_regex -Wall -iquote$(INCLUDEDIR) -L../$(LIBDIR) -lomegle -Wl,-rpath,$(LIBDIR) -std=c++0x

../Omegle-MITM: $(OBJS)
	g++ $(OBJS) $(FLAGS) -o ../Omegle-MITM

ChatLog.o: ChatLog.cpp ChatLog.h
	g++ $(FLAGS) -c ChatLog.cpp -o ChatLog.o

ChatRoom.o: ChatRoom.cpp ChatLog.h ChatRoom.h Error.h
	g++ $(FLAGS) -c ChatRoom.cpp -o ChatRoom.o

MitmLogger.o: MitmLogger.cpp ChatLog.h ChatRoom.h Error.h MitmLogger.h
	g++ $(FLAGS) -c MitmLogger.cpp -o MitmLogger.o

main.o: main.cpp MitmLogger.h
	g++ $(FLAGS) -c main.cpp -o main.o

clean:
	rm -f ../Omegle-MITM $(OBJS)
