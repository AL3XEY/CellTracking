all: cellTracking

run: cellTracking 
	./cellTracking datas/stack-0.img

cellTracking: cellTracking.cpp CImg.h
	g++ -o cellTracking cellTracking.cpp -O2 -L/usr/X11R6/lib  -lm  -lpthread -lX11
	
clean:
	rm -f *.o *~ cellTracking
	clear
