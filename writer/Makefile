.SUFFIXES: .c .o
dev_root=/root/cpp_local/src/


CXX        = /opt/centos/devtoolset-1.1/root/usr/bin/g++ -g -std=c++11
CC        = /opt/centos/devtoolset-1.1/root/usr/bin/gcc -g 
CXX=/opt/centos/devtoolset-1.1/root/usr/bin/g++ -g -std=c++11 -Werror

COMPILE = $(CC) -Wall -O2 -D_FILE_OFFSET_BITS=64 -DOS_LINUX
INC_PATH = -I/usr/local/include
LIB_PATH = -L/usr/local/lib
TARGET_PATH = /usr/local/bin



QQ_INC=-I$(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/
QQ_OBJ=$(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/libopenapi.a  $(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/libprotobuf.a


INC_PATH += $(QQ_INC) $(QQ_OBJ)

SHARED_OBJS = TencentStorageServiceWriter.o

ALL_OBJS = $(SHARED_OBJS)

ALL_PRGS = $(SHARED_OBJS)

all: $(ALL_OBJS) $(ALL_PRGS)
.o:
	$(COMPILE) -o $@ $<  $(SHARED_OBJS) $(COMMON_LIB) $(LIB_PATH) $(INC_PATH)
.c:
	$(COMPILE) -o $@ $<  $(ALL_OBJS) $(COMMON_LIB) $(LIB_PATH) $(INC_PATH)
.c.o:
	$(COMPILE) -c -o  $@ $<  $(INC_PATH)
install:
	ar rcs lib_common.a $(ALL_OBJS)
clean:
	rm -f $(ALL_OBJS) $(ALL_PRGS)
	
	
	
	
	
	
	
	
	##################
	
	
	
	.SUFFIXES: .c .o
dev_root=/root/cpp_local/src/
transfer_src=$(dev_root)/transfer
box_src=$(dev_root)/box
writer_src=$(dev_root)/writer


CXX        = /opt/centos/devtoolset-1.1/root/usr/bin/g++ -g -std=c++11
CC        = /opt/centos/devtoolset-1.1/root/usr/bin/gcc -g 

COMPILE = $(CC) -Wall -O2 -D_FILE_OFFSET_BITS=64 -DOS_LINUX
INC_PATH = -I/usr/local/include
LIB_PATH = -L/usr/local/lib
TARGET_PATH = /usr/local/bin


SHARED_OBJS = $(dev_root)/common/logger.o $(dev_root)/common/object.o\
$(dev_root)/common/tools.o $(dev_root)/common/bpt.o $(dev_root)/common/BptDelegate.o

SHARED_OBJS += $(transfer_src)/zerocopy_stream.o $(transfer_src)/tcp/connection_pool.o \
$(transfer_src)/tcp/sockopt.o $(transfer_src)/tcp/logger.o $(transfer_src)/tcp/hash.o \
$(transfer_src)/tcp/shared_func.o $(transfer_src)/tcp/ini_file_reader.o \
$(transfer_src)/tcp/sched_thread.o $(transfer_src)/tcp/pthread_func.o \
$(transfer_src)/tcp/http_func.o

SHARED_OBJS += $(box_src)/box.object.o

#SHARED_OBJS += $(writer_src)/TencentStorageServiceWriter.o
ALL_OBJS = $(SHARED_OBJS)

ALL_PRGS = 

all:  $(ALL_OBJS) $(ALL_PRGS)
.o:
	$(COMPILE) -o $@ $<  $(SHARED_OBJS) $(COMMON_LIB) $(LIB_PATH) $(INC_PATH)
.c:
	$(COMPILE) -o $@ $<  $(ALL_OBJS) $(COMMON_LIB) $(LIB_PATH) $(INC_PATH)
.c.o:
	$(COMPILE) -c -o  $@ $<  $(INC_PATH)
clean:
	rm -f $(ALL_OBJS) $(ALL_PRGS)
	
	
	
	
	
	##########
	
	
	.SUFFIXES: .c .o
dev_root=/root/cpp_local/src/
transfer_src=$(dev_root)/transfer
box_src=$(dev_root)/box
writer_src=$(dev_root)/writer


CXX        = /opt/centos/devtoolset-1.1/root/usr/bin/g++ -g -std=c++11
CC        = /opt/centos/devtoolset-1.1/root/usr/bin/gcc -g 

COMPILE = $(CC) -Wall -O2 -D_FILE_OFFSET_BITS=64 -DOS_LINUX
INC_PATH = -I/usr/local/include
LIB_PATH = -L/usr/local/lib
TARGET_PATH = /usr/local/bin


SHARED_OBJS = $(dev_root)/common/logger.o $(dev_root)/common/object.o\
$(dev_root)/common/tools.o $(dev_root)/common/bpt.o $(dev_root)/common/BptDelegate.o

SHARED_OBJS += $(transfer_src)/zerocopy_stream.o $(transfer_src)/tcp/connection_pool.o \
$(transfer_src)/tcp/sockopt.o $(transfer_src)/tcp/logger.o $(transfer_src)/tcp/hash.o \
$(transfer_src)/tcp/shared_func.o $(transfer_src)/tcp/ini_file_reader.o \
$(transfer_src)/tcp/sched_thread.o $(transfer_src)/tcp/pthread_func.o \
$(transfer_src)/tcp/http_func.o

SHARED_OBJS += $(box_src)/box.object.o

SHARED_OBJS += $(writer_src)/TencentStorageServiceWriter.o





QQ_INC=-I$(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/
QQ_OBJ=$(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/libopenapi.a  $(dev_root)/store_photo_sdk/$(SYSTEM_BIT)/libprotobuf.a

QQ_CXX=$(CXX)  $(QQ_INC) $(QQ_OBJ)



ALL_OBJS = $(SHARED_OBJS)
ALL_PRGS = 
vpath %.c $(dev_root)/store_photo_sdk/64/
vpath %.h $(dev_root)/store_photo_sdk/64/



all:  $(ALL_OBJS)
#%.o: %.c
#	$(COMPILE.c) -I $(OUTPUT_OPTION) $<
 
# Applicable for create executable (without extension) from object .o object file
# $^ matches all the pre-requisites (no duplicates)
#%: %.o
#	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
clean:
	rm -f $(ALL_OBJS) $(ALL_PRGS)
	
	
