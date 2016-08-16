SYSTEM_BIT := $(shell getconf LONG_BIT)


CXX        = /opt/centos/devtoolset-1.1/root/usr/bin/g++ -g 
USR_DIR    =/usr/local/webserver
DEV_DIR    =../../




CPP_DEFS   =-D=HAVE_CONFIG_H
CPP_OPTS   =-Werror 
CFLAGS_CXX =-lstdc++ -std=c++11  -lpthread  

INCS_DIRS  = -I"../" -I "../../"

OBJECTS    += -rdynamic   -lz -lcrypt -lnsl -lm -lssl -lcrypto  -lstdc++  -lpthread -lgomp -D__CLOUDPIC_INTERFACE__ 
QQ_OBJECTS    = ${DEV_DIR}/store_photo_sdk/$(SYSTEM_BIT)/libopenapi.a  ${DEV_DIR}/store_photo_sdk/$(SYSTEM_BIT)/libprotobuf.a
QQ_INCS_DIRS = -I${DEV_DIR}/store_photo_sdk/$(SYSTEM_BIT)/



gtest_INCS_DIRS  += -I${USR_DIR}/gtest/include/
gtest_LIBS_DIRS  += -L${USR_DIR}/gtest/lib/
gtest_OBJECTS    += ${USR_DIR}/gtest/lib/libgtest.a ${USR_DIR}/gtest/lib/libgtest_main.a

LINKS       = FdfsConf.o FdfsStoragMap.o bpt.o BptDelegate.o FdfsSender.o FdfsJobschedule.o


all:f m bpt job run 
f:${DEV_DIR}/fdfs/FdfsConf.cc ${DEV_DIR}/fdfs/FdfsConf.h
	${CXX} -fPIC ${CFLAGS_CXX} -c ${DEV_DIR}/fdfs/FdfsConf.cc  ${INCS_DIRS}    ${LIBS_DIRS} ${OBJECTS} 

m:${DEV_DIR}/fdfs/FdfsStoragMap.cc ${DEV_DIR}/fdfs/FdfsStoragMap.h
	${CXX} -fPIC ${CFLAGS_CXX} -c ${DEV_DIR}/fdfs/FdfsStoragMap.cc ${DEPS_EXEC} ${INCS_DIRS}    ${LIBS_DIRS}    ${OBJECTS}

s:${DEV_DIR}/fdfs/FdfsSender.cc ${DEV_DIR}/fdfs/FdfsSender.h
	${CXX} -fPIC ${CFLAGS_CXX}-c ${DEV_DIR}/fdfs/FdfsSender.cc ${DEPS_EXEC} ${INCS_DIRS}  ${QQ_INCS_DIRS}  ${LIBS_DIRS}  ${QQ_OBJECTS}   -D__CLOUDPIC_INTERFACE__ 


bpt:${DEV_DIR}/jobschedule/BptDelegate.cc ${DEV_DIR}/jobschedule/BptDelegate.h ${DEV_DIR}/jobschedule/bpt/predefined.h ${DEV_DIR}/jobschedule/bpt/bpt.h
	${CXX}  -fPIC ${CFLAGS_CXX} -c ../BptDelegate.cc  ${DEV_DIR}/jobschedule/bpt/bpt.cc   ${DEPS_EXEC} ${INCS_DIRS}  ${LIBS_DIRS}   ${LIBS}  ${OBJECTS} 

job:../FdfsJobschedule.cc ../FdfsJobschedule.h
	${CXX}  -fPIC ${CFLAGS_CXX} -c ../FdfsJobschedule.cc     ${DEPS_EXEC} ${INCS_DIRS} ${QQ_INCS_DIRS} ${LIBS_DIRS}   ${LIBS}  ${OBJECTS} 


run:test.fdfs2qq_cli.cc
	ar ru libcommon.a FdfsConf.o FdfsStoragMap.o bpt.o BptDelegate.o FdfsJobschedule.o
	${CXX}  ${CFLAGS_CXX}-o run.exe test.fdfs2qq_cli.cc  libcommon.a   ${DEPS_EXEC} ${INCS_DIRS}  ${LIBS_DIRS}   ${LIBS}  ${OBJECTS} 
	

clean:
	$(RM) -r *.exe *.dSYM *.o core.* ../lib/*.so *.a
	

