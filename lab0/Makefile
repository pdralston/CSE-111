#------------------------------------------------------------------------------
#  Makefile for CMPS 111 Lab0
#
#  make                     makes main
#  make.exe                 build executable from objectfiles
#  make clean               removes all binaries
#  make spotless            removes all binaries and executables
#  make memcheck            runs main under valgrind
#  deps                     builds dependency file TODO (David Raston) Figure out what this means
#  make submit              submits files for grading on the UNIX timeshare
#------------------------------------------------------------------------------

CLIENT         = main
ADT1           = vstring
ADT2           = wordList
SOURCE         = $(CLIENT).cpp
OBJECT         = $(CLIENT).o
ADT1_SOURCE    = $(ADT1).cpp
ADT1_OBJECT    = $(ADT1).o
ADT1_HEADER    = $(ADT1).hpp
ADT2_SOURCE    = $(ADT2).cpp
ADT2_OBJECT    = $(ADT2).o
ADT2_HEADER    = $(ADT2).hpp
OBJS           = $(OBJECT) $(ADT1_OBJECT) $(ADT2_OBJECT)
EXECBIN        = $(CLIENT).exe
EXE            = $(CLIENT) $(EXECBIN) $(CLIENT).out
COMPILE        = gcc -c -std=c99 -Wall
LINK           = gcc -o
REMOVE         = rm -f
MEMCHECK       = valgrind --leak-check=full -v

$(CLIENT) : $(OBJECT) $(ADT1_OBJECT) $(ADT2_OBJECT)
	$(LINK) $(CLIENT) $(OBJECT) $(ADT1_OBJECT) $(ADT2_OBJECT)

$(OBJECT) : $(SOURCE) $(ADT1_HEADER) $(ADT2_HEADER)
	$(COMPILE) $(SOURCE)

$(OBJECT2) : $(SOURCE2) $(ADT1_HEADER) $(ADT2_HEADER)
	$(COMPILE) $(SOURCE2)

$(OBJECT3) : $(SOURCE3) $(ADT2_HEADER)
	$(COMPILE) $(SOURCE3)

$(ADT1_OBJECT) : $(ADT1_SOURCE) $(ADT1_HEADER)
	$(COMPILE) $(ADT1_SOURCE)

$(ADT2_OBJECT) : $(ADT2_SOURCE) $(ADT2_HEADER)
	$(COMPILE) $(ADT2_SOURCE)

$(EXECBIN) :
	 $(LINK) $(CLIENT) $(OBJECT) $(ADT1_OBJECT) $(ADT2_OBJECT)

deps : 
	

clean :
	$(REMOVE) $(OBJS) 

spotless : 
	$(REMOVE) $(OBJS) $(EXE)

memcheck : $(CLIENT)
	$(MEMCHECK) $(CLIENT)

submit :
	submit  cse111-wm.w20  lab0 $(SOURCE) $(ADT1_HEADER) $(ADT2_HEADER) $(ADT1_SOURCE) $(ADT2_SOURCE) Makefile README
