#
# this makefile will compile and link the DataCorruptor application
# 
# =======================================================
#                  DataCorruptor
# =======================================================
# FINAL BINARY Target
../bin/dx : ../obj/dx.o ../obj/logger.o
	cc ../obj/dx.o ../obj/logger.o -o ../bin/dx
#
# =======================================================
#                     Dependencies
# =======================================================                     
../obj/dx.o : ../src/dataCorruptor.c ../inc/dataCorruptor.h 
	cc -c ../src/dataCorruptor.c -o ../obj/dx.o

../obj/logger.o :  ../../Common/src/dataLogger.c   ../../Common/inc/dataLogger.h
	cc -c ../../Common/src/dataLogger.c -o ../obj/logger.o

#
# =======================================================
# Other targets
# =======================================================                     
clean:
	rm -f ../bin/dx*
	rm -f ../obj/dx.*
	rm -f ../obj/logger.*
	rm -f ../src/dataCorruptor.c~
	rm -f ../../Common/src/dataLogger.c~
