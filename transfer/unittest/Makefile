cxx=/opt/centos/devtoolset-1.1/root/usr/bin/g++	 -g  -Werror -O2
inc=-I ../
obj=../lib_common.a
flag=-lpthread
all:tcp boost
tcp:test.connctpool.cc
	$(cxx) -o tcp test.connctpool.cc -I../  ../tcp/lib_common.a -lpthread
boost:boost_ipc_client.cc boost_ipc_server.cc
	$(cxx) -o boost_client.exe boost_ipc_client.cc  -I/usr/local/webserver/boost_1_44_0/include/ /usr/local/webserver/boost_1_44_0/lib/libboost_system.a
	$(cxx) -o boost_server.exe boost_ipc_server.cc  -I/usr/local/webserver/boost_1_44_0/include/ /usr/local/webserver/boost_1_44_0/lib/libboost_system.a
