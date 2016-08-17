SYSTEM_BIT := $(shell getconf LONG_BIT)
.SUFFIXES: .c .o .cc
dev_root=./src/



C_SHARED_OBJS =  $(dev_root)/transfer/tcp/connection_pool.o \
$(dev_root)/transfer/tcp/sockopt.o $(dev_root)/transfer/tcp/logger.o $(dev_root)/transfer/tcp/hash.o \
$(dev_root)/transfer/tcp/shared_func.o $(dev_root)/transfer/tcp/ini_file_reader.o \
$(dev_root)/transfer/tcp/sched_thread.o $(dev_root)/transfer/tcp/pthread_func.o \
$(dev_root)/transfer/tcp/http_func.o

CXX_SHARED_OBJS = $(dev_root)/common/logger.o $(dev_root)/common/object.o\
$(dev_root)/common/tools.o $(dev_root)/common/bpt.o $(dev_root)/common/BptDelegate.o
CXX_SHARED_OBJS += $(dev_root)/box/box_object.o $(dev_root)/transfer/zerocopy_stream.o

W_SHARED_OBJS = $(dev_root)/writer/TencentStorageServiceWriter.o

SHARED_OBJS =$(C_SHARED_OBJS)
SHARED_OBJS +=$(CXX_SHARED_OBJS)
SHARED_OBJS +=$(W_SHARED_OBJS)





CXX        = /opt/centos/devtoolset-1.1/root/usr/bin/g++ -g -std=c++11 -fPIC  
CC        = /opt/centos/devtoolset-1.1/root/usr/bin/gcc -g -fPIC  
CXX_COMPILE = $(CXX) -Werror -O2 
COMPILE= $(CC) -Werror -O2 

INC_PATH = -I/usr/local/include -I$(dev_root) -I$(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/
LIB_PATH = -L/usr/local/lib 
OBJS =  $(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/libopenapi.a $(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/libprotobuf.a
#TARGET_PATH = /usr/local/bin
#INC_DIR = store_photo_sdk
#SRC_DIR = src


ALL_OBJS = $(SHARED_OBJS)

LDFLAGS = -rdynamic   -lz -lcrypt -lnsl -lm -lssl -lcrypto  -lstdc++  -lpthread -lgomp -D__CLOUDPIC_INTERFACE__  
#LIBS = -rdynamic   -lz -lcrypt -lnsl -lm -lssl -lcrypto  -lstdc++  -lpthread -lgomp -D__CLOUDPIC_INTERFACE__  


all: $(SHARED_OBJS) prog.exe
.o:
	$(COMPILE) -o $@ $<  $(STATIC_OBJS) $(LIB_PATH) $(INC_PATH)
.cc:
	$(CXX_COMPILE) -o $@ $<  $(STATIC_OBJS) $(LIB_PATH) $(INC_PATH)
.cc.o:
	$(CXX_COMPILE) -c -o $@ $<  $(INC_PATH) $(LDFLAGS)
.c:
	$(COMPILE) -o $@ $<  $(STATIC_OBJS) $(LIB_PATH) $(INC_PATH)
.c.o:
	$(COMPILE) -c -o $@ $<  $(INC_PATH)
.c.lo:
	$(COMPILE) -c -fPIC -o $@ $<  $(INC_PATH) 
prog.exe: $(SHARED_OBJS) 
	ar cru libcommon.a $^
	$(CXX) $(CFLAGS) -o $@ $(dev_root)/writer/unittest/qqsend.cc $(ALL_OBJS)  $(INC_PATH) $(LIB_PATH) $(OBJS)   $(LDFLAGS)
clean:
	rm -f $(ALL_OBJS) $(ALL_PRGS) libcommon.a prog.exe

