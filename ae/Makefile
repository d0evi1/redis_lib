bin: demo1 demo2 demo3 demo4


demo1:
	gcc -g -Wall -o demo1 demo1.c zmalloc.c ae.c ae_kqueue.c #ae_epoll.c

demo2:
	gcc -g -Wall -o demo2 demo2.c zmalloc.c ae.c ae_kqueue.c anet.c #ae_epoll.c

demo3:
	gcc -g -Wall -o demo3 demo3.c zmalloc.c ae.c ae_kqueue.c

demo4:
	gcc -g -Wall -o demo4 demo4_udpserver.c zmalloc.c ae.c ae_select.c



clean:
	rm demo1 demo2 demo3 demo4; rm -rf *.dSYM 
