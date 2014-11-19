VPATH=../ae ../comm ../dict

COMM_INC=-I../comm
COMM_LIB=

DICT_INC=-I../dict
DICT_LIB=

AE_INC=-I../ae 
AE_LIB=

ALL_INC = $(AE_INC) $(COMM_INC) $(DICT_INC)
ALL_LIB = $(AE_LIB) $(COMM_LIB) $(DICT_LIB)


all: demo1 demo2 demo3 demo4 demo5 


#-----------------------------------------------
# -g(-ggdb):生成调试信息
# -Wall: 警告信息
#-----------------------------------------------
CFLAGS = -g -Wall


#-------------------------------------------------
#定义make识别后缀
#-------------------------------------------------
.SUFFIXES: .c .cpp .o .h

#-------------------------------------------------
# 双后缀规则
# 自动化变量: $@ -- 所有的.o目标集
# 自动化变量：$< -- 所有的.c依赖目标集
#-------------------------------------------------
.c.o:
	$(CC) -c $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)
.cpp.o:
	$(CXX) -c $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)



demo1: demo1_timer.o zmalloc.o ae.o ae_select.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)		
	

demo2: demo2_tcpserver.o zmalloc.o ae.o ae_select.o anet.o 
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)		
	

demo3: demo3_udpserver.o zmalloc.o ae.o ae_select.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)		
	
demo4: demo4_zmalloc.o zmalloc.o 
	$(CXX) $(CFLAGS) -DHAVE_MALLOC_SIZE -o $@ $^ $(ALL_INC) $(ALL_LIB)		
	
demo5: demo5_dict.o dict.o zmalloc.o redisassert.o
	$(CXX) $(CFLAGS) -o $@ $^ $(ALL_INC) $(ALL_LIB)		
	
.PHONY: clean

clean:
	@rm demo1 demo2 demo3 demo4 demo5; rm -rf *.o 
