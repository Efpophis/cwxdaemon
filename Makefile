GPP=g++ -std=c++0x


all: cwxdaemon

cwxdaemon: keyer.o clisocket.o servsocket.o cwxdaemon.o udpsocket.o
	$(GPP) -o $@ $^

%.o:%.c *.h Makefile
	$(GPP) -c -o $@ $<

%.o:%.cpp *.h Makefile
	$(GPP) -c -o $@ $<

clean:
	rm -f *.o cwxdaemon
