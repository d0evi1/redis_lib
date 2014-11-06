bin: demo1 demo2 demo3


demo1:
	gcc -g -Wall -o demo1 demo1_timer.c zmalloc.c ae.c ae_select.c #ae_epoll.c

demo2:
	gcc -g -Wall -o demo2 demo2_tcpserver.c zmalloc.c ae.c ae_select.c anet.c #ae_epoll.c

demo3:
	gcc -g -Wall -o demo3 demo3_udpserver.c zmalloc.c ae.c ae_select.c

clean:
	rm demo1 demo2 demo3; rm -rf *.dSYM 
