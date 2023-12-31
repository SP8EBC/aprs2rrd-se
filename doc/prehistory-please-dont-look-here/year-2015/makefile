CC=g++
CFLAGS=-c -Wall -DNDEBUG -I. -I/usr/include/mysql++ -I/usr/include/mysql
LDFLAGS=-L. -L/usr/lib/i386-linux-gnu -L/usr/lib -L/lib  -lconfig -lconfig++ -lmysqlpp -lmysqlclient
SOURCES=main.cpp AprsPacket.cpp AprsThreadConfig.cpp AprsThread.cpp AprsWXData.cpp DataPresence.cpp MySqlConnInterface.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=aprs2rrd

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	mkdir bin
	$(CC) -o bin/$@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
