#
# this makefile will compile and link the all applications and move to common/bin folder
# 
default:
	$(MAKE) -C ../DR/makefile
	$(MAKE) -C ../DC/makefile
	$(MAKE) -C ../DX/makefile
	cp  ../DR/bin/* ../Common/bin
	cp  ../DC/bin/* ../Common/bin
	cp  ../DX/bin/* ../Common/bin
clean:
	$(MAKE) -C ../DR/makefile clean
	$(MAKE) -C ../DC/makefile clean
	$(MAKE) -C ../DX/makefile clean
	rm ../Common/bin/*