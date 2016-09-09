.PHONY: all test clean

all:
test:
	$(CXX) $(CFLAGS) -o ini.unittest.exe $(dev_root)/common/unittest/test.iniconfig.cc libcommon.a   $(INC_PATH) $(LIB_PATH) $(LDFLAGS)
	if [ -f unittest_bin ] ;\
	then\
		rm unittest_bin;\
		mkdir  unittest_bin;\
	fi;	 
	cp $(dev_root)/common/unittest/*.ini ./unittest_bin/
	cp -r *unittest.exe* ./unittest_bin/
	rm -fr  ./*unittest.exe*