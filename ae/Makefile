bin: demo1 demo2


demo1:
	gcc -g -Wall -o demo1 demo1.c zmalloc.c ae.c ae_epoll.c

demo2:
	gcc -g -Wall -o demo2 demo2.c zmalloc.c ae.c ae_epoll.c anet.c

clean:
	rm demo1 demo2 *.o 
