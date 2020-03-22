#
# this makefile will compile and link the DataCorruptor application
# 
# =======================================================
#                  dataReader
# =======================================================
# FINAL BINARY Target
../bin/dr : ../obj/dr.o ../obj/logger.o
	cc ../obj/dr.o ../obj/logger.o -o ../bin/dr
#
# =======================================================
#                     Dependencies
# =======================================================                     
../obj/dr.o : ../src/dataReader.c ../inc/dataReader.h 
	cc -c ../src/dataReader.c -o ../obj/dr.o

../obj/logger.o :  ../../Common/src/dataLogger.c   ../../Common/inc/dataLogger.h
	cc -c ../../Common/src/dataLogger.c -o ../obj/logger.o

#
# =======================================================
# Other targets
# =======================================================                     
clean:
	rm -f ../bin/dr*
	rm -f ../obj/dr.*
	rm -f ../obj/logger.*
	rm -f ../src/dataReader.c~
	rm -f ../../Common/src/dataLogger.c~
