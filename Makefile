SYSTEM_BIT := $(shell getconf LONG_BIT)
.SUFFIXES: .c .o .cc
dev_root=./src



C_SHARED_OBJS =  $(dev_root)/transfer/tcp/connection_pool.o \
$(dev_root)/transfer/tcp/sockopt.o $(dev_root)/transfer/tcp/logger.o $(dev_root)/transfer/tcp/hash.o \
$(dev_root)/transfer/tcp/shared_func.o $(dev_root)/transfer/tcp/ini_file_reader.o \
$(dev_root)/transfer/tcp/sched_thread.o $(dev_root)/transfer/tcp/pthread_func.o \
$(dev_root)/transfer/tcp/http_func.o $(dev_root)/common/ini.o $(dev_root)/common/INIReader.o

CXX_SHARED_OBJS = $(dev_root)/common/logger.o $(dev_root)/common/object.o\
$(dev_root)/common/tools.o $(dev_root)/common/bpt.o $(dev_root)/common/BptDelegate.o

CXX_SHARED_OBJS += $(dev_root)/box/box_object.o $(dev_root)/transfer/zerocopy_stream.o

W_SHARED_OBJS = $(dev_root)/writer/TencentStorageServiceWriter.o

SHARED_OBJS =$(C_SHARED_OBJS)
SHARED_OBJS +=$(CXX_SHARED_OBJS)
SHARED_OBJS +=$(W_SHARED_OBJS)





CXX        = /opt/centos/devtoolset-1.1/root/usr/bin/g++ -g -std=c++11 -fPIC  
CC        = /opt/centos/devtoolset-1.1/root/usr/bin/gcc -g -fPIC   -D_GNU_SOURCE
CXX_COMPILE = $(CXX) -Werror -O2 
COMPILE= $(CC) -Werror -O2 

INC_PATH = -I/usr/local/include -I$(dev_root) -I$(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/
LIB_PATH = -L/usr/local/lib 
OBJS =  $(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/libopenapi.a $(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/libprotobuf.a


GTEST_INC =-I /usr/local/webserver/gtest/include/
GTEST_LIB =/usr/local/webserver/gtest/lib/libgtest_main.a /usr/local/webserver/gtest/lib/libgtest.a


ALL_OBJS = $(SHARED_OBJS)

LDFLAGS = -rdynamic   -lz -lcrypt -lnsl -lm -lssl -lcrypto  -lstdc++  -lpthread -lgomp -D__CLOUDPIC_INTERFACE__  
#LIBS = -rdynamic   -lz -lcrypt -lnsl -lm -lssl -lcrypto  -lstdc++  -lpthread -lgomp -D__CLOUDPIC_INTERFACE__  
.PHONY: all test clean

all: $(SHARED_OBJS) prog.exe
.c:
	$(COMPILE) -o $@ $<  $(STATIC_OBJS) $(LIB_PATH) $(INC_PATH)
.c.o:
	$(COMPILE) -c -o $@ $<  $(INC_PATH)
.c.lo:
	$(COMPILE) -c -fPIC -o $@ $<  $(INC_PATH) 
.cc:
	$(CXX_COMPILE) -o $@ $<  $(STATIC_OBJS) $(LIB_PATH) $(INC_PATH)
.cc.o:
	$(CXX_COMPILE) -c -o $@ $<  $(INC_PATH) $(LDFLAGS)
.o:
	$(COMPILE) -o $@ $<  $(STATIC_OBJS) $(LIB_PATH) $(INC_PATH)
prog.exe: $(SHARED_OBJS) 
	ar cru libcommon.a $^
	$(CXX) $(CFLAGS) -o $@ $(dev_root)/writer/unittest/qqsend.cc $(ALL_OBJS)  $(INC_PATH) $(LIB_PATH) $(OBJS)   $(LDFLAGS)
test:
	rm -f *.db
#	$(CXX) $(CFLAGS) -o unittest.iniconfig.exe $(dev_root)/common/unittest/test.iniconfig.cc libcommon.a   \
		$(INC_PATH) $(LIB_PATH) $(LDFLAGS) $(GTEST_INC) $(GTEST_LIB)
#	$(CXX) $(CFLAGS) -o unittest.storageconf.exe $(dev_root)/box/unittest/test.storageconf.cc \
		$(dev_root)/box/StorageConfig.cc libcommon.a   \
		$(INC_PATH) $(LIB_PATH) $(LDFLAGS) $(GTEST_INC) $(GTEST_LIB)


		
	$(CXX) $(CFLAGS) -o unittest.cli_produce.exe  $(dev_root)/jobschedule/run.cc \
		$(dev_root)/jobschedule/TrackerCli.cc \
		$(dev_root)/jobschedule/ProducerCli.cc \
		$(dev_root)/box/StorageConfig.cc libcommon.a ./src/store_photo_sdk/64/libopenapi.a  ./src/store_photo_sdk/64/libprotobuf.a  \
		$(INC_PATH) $(LIB_PATH) /usr/lib64/libevent.so  $(LDFLAGS)  -lrt $(GTEST_INC) $(GTEST_LIB)  -lrt
	
		
	if test ! -s "unittest_bin";\
	then\
		mkdir  unittest_bin;\
	fi;	 
	cp -r $(dev_root)/common/unittest/*.ini unittest_bin/
	cp -r unittest.*.exe unittest_bin/
	rm -fr  ./unittest.*.exe
clean:
	rm -f $(ALL_OBJS) $(ALL_PRGS) libcommon.a *.exe ./unittest *_bplus_tree.db

