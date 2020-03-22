#
# this makefile will compile and link the dataCreator application
# 
# =======================================================
#                  dataCreator
# =======================================================
# FINAL BINARY Target
../bin/dc : ../obj/dc.o ../obj/logger.o
	cc ../obj/dc.o ../obj/logger.o -o ../bin/dc
#
# =======================================================
#                     Dependencies
# =======================================================                     
../obj/dc.o : ../src/dataCreator.c ../inc/dataCreator.h 
	cc -c ../src/dataCreator.c -o ../obj/dc.o

../obj/logger.o :  ../../Common/src/dataLogger.c   ../../Common/inc/dataLogger.h
	cc -c ../../Common/src/dataLogger.c -o ../obj/logger.o

#
# =======================================================
# Other targets
# =======================================================                     
clean:
	rm -f ../bin/dc*
	rm -f ../obj/dc.*
	rm -f ../obj/logger.*
	rm -f ../src/dataCreator.c~
	rm -f ../../Common/src/dataLogger.c~
