SYSTEM_BIT := $(shell getconf LONG_BIT)

.SUFFIXES: .c .o .cc
dev_root=./src
lib_addto=/usr/local/webserver
INSTALL_DIR=/usr/local/webserver/fdfs2qq_mongo

C_SHARED_OBJS =  $(dev_root)/third_part/fastdfs/common/connection_pool.o \
$(dev_root)/third_part/fastdfs/common/sockopt.o $(dev_root)/third_part/fastdfs/common/logger.o $(dev_root)/third_part/fastdfs/common/hash.o \
$(dev_root)/third_part/fastdfs/common/shared_func.o $(dev_root)/third_part/fastdfs/common/ini_file_reader.o \
$(dev_root)/third_part/fastdfs/common/sched_thread.o $(dev_root)/third_part/fastdfs/common/pthread_func.o \
$(dev_root)/third_part/fastdfs/common/http_func.o $(dev_root)/common/ini.o $(dev_root)/common/INIReader.o

CXX_SHARED_OBJS = $(dev_root)/third_part/ganji/util/log/thread_fast_log.o $(dev_root)/common/object.o\
$(dev_root)/common/tools.o $(dev_root)/common/bpt.o $(dev_root)/common/BptDelegate.o\
$(dev_root)/common/workqueue.o $(dev_root)/third_part/ganji/util/log/thread_fast_log.o

CXX_SHARED_OBJS += $(dev_root)/box/box_object.o\
$(dev_root)/transfer/zerocopy_stream.o

W_SHARED_OBJS = $(dev_root)/writer/TencentStorageServiceWriter.o

SHARED_OBJS =$(C_SHARED_OBJS)
SHARED_OBJS +=$(CXX_SHARED_OBJS)
SHARED_OBJS +=$(W_SHARED_OBJS)





CXX        = /usr/bin/g++ -g -std=c++0x -fPIC  
CC        = /usr/bin/gcc -g -fPIC   -D_GNU_SOURCE
CXX_COMPILE = /usr/bin/g++ -g -std=c++0x -fPIC   
COMPILE= /usr/bin/g++ -g -fPIC   -D_GNU_SOURCE

INC_PATH = -I/usr/local/include -I$(dev_root) \
	-I$(dev_root)/third_part/store_photo_sdk/$(SYSTEM_BIT)/ \
	-I$(dev_root)/third_part/\
	-I$(lib_addto)/libevent2/include/
	
LIB_PATH = -L/usr/local/lib 
OBJS =  $(dev_root)/third_part/store_photo_sdk/$(SYSTEM_BIT)/libopenapi.a $(dev_root)/third_part/store_photo_sdk/$(SYSTEM_BIT)/libprotobuf.a
#OBJS += $(lib_addto)/mongo-c-driver/lib/libmongoc-1.0.so
OBJS += $(lib_addto)/libevent2/lib/libevent.so 


ALL_OBJS = $(SHARED_OBJS)

LDFLAGS = -rdynamic  -lcrypto  -lstdc++  -lpthread  -D__CLOUDPIC_INTERFACE__  
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
		

	$(CXX) $(CFLAGS) -o cli_consume.exe  \
		$(dev_root)/jobschedule/ConsumerCli.cc \
		$(dev_root)/box/StorageConfig.cc \
		libcommon.a \
		$(OBJS)\
		$(INC_PATH) \
		$(LIB_PATH) \
		$(LDFLAGS)  -lrt  \
		$(GTEST_INC) $(GTEST_LIB) 
	
	$(CXX) $(CFLAGS) -o cli_produce.exe  \
			$(dev_root)/jobschedule/ProducerCli.cc \
		$(dev_root)/box/StorageConfig.cc \
		libcommon.a \
		$(OBJS)\
		$(INC_PATH) \
		$(LIB_PATH) \
		$(LDFLAGS)  -lrt  \
		$(GTEST_INC) $(GTEST_LIB) 
		
	if test ! -s "unittest_bin";\
	then\
		mkdir  unittest_bin;\
	fi;	 
	cp -r *.exe unittest_bin/
	rm -fr  ./*.exe
install:
	mkdir -p ${INSTALL_DIR}/bin ${INSTALL_DIR}/sbin ${INSTALL_DIR}/etc
	cp -fr unittest_bin/*.exe  ${INSTALL_DIR}/bin	
	cp -fr config/*  ${INSTALL_DIR}/etc     
	chmod +x ${INSTALL_DIR}/sbin/*
	rm -fr ${INSTALL_DIR}/etc/*.sh
	rm -fr $(ALL_OBJS) $(ALL_PRGS) libcommon.a *.exe ./unittest_bin *_bplus_tree.db core.*

clean:
	rm -fr $(ALL_OBJS) $(ALL_PRGS) libcommon.a *.exe ./unittest_bin *_bplus_tree.db core.*

