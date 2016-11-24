all: tp3

run: tp3 
	./tp3 datas/stack-0.img

tp3: tp3.cpp CImg.h
	g++ -o tp3 tp3.cpp -O2 -L/usr/X11R6/lib  -lm  -lpthread -lX11
	
clean:
	rm -f *.o *~ tp3
	clear
