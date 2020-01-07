#------------------------------------------------------------------------------
#  Makefile for CMPS 111 Lab0
#
#  make                     makes main
#  make clean               removes all binaries
#  make spotless            removes all binaries and non-sourcefiles ()
#  make memcheck            runs main under valgrind
#  make clean               removes all binaries
#  Makefile.deps            builds dependency file TODO (David Raston) Figure out what this means
#  make submit              submits files for grading on the UNIX timeshare
#------------------------------------------------------------------------------

CLIENT         = main
ADT1           = vstring
ADT2           = wordList
SOURCE         = $(CLIENT).cpp
SOURCE2        = $(CLIENT2).cpp
SOURCE3        = $(CLIENT3).cpp
OBJECT         = $(CLIENT).o
OBJECT2        = $(CLIENT2).o
OBJECT3        = $(CLIENT3).o
ADT1_SOURCE    = $(ADT1).c
ADT1_OBJECT    = $(ADT1).o
ADT1_HEADER    = $(ADT1).h
ADT2_SOURCE    = $(ADT2).c
ADT2_OBJECT    = $(ADT2).o
ADT2_HEADER    = $(ADT2).h
COMPILE        = gcc -c -std=c99 -Wall
LINK           = gcc -o
REMOVE         = rm -f
MEMCHECK       = valgrind --leak-check=full

$(CLIENT) : $(OBJECT) $(ADT1_OBJECT) $(ADT2_OBJECT)
	$(LINK) $(CLIENT) $(OBJECT) $(ADT1_OBJECT) $(ADT2_OBJECT)

$(CLIENT2) : $(OBJECT2) $(ADT1_OBJECT) $(ADT2_OBJECT)
	$(LINK) $(CLIENT2) $(OBJECT2) $(ADT1_OBJECT) $(ADT2_OBJECT)

$(CLIENT3) : $(OBJECT3) $(ADT2_OBJECT)
	$(LINK) $(CLIENT3) $(OBJECT3) $(ADT2_OBJECT)

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

clean :
	$(REMOVE) $(CLIENT) $(OBJECT) $(CLIENT2) $(OBJECT2) $(CLIENT3) $(OBJECT3) $(ADT1_OBJECT) $(ADT2_OBJECT)

memcheck : $(CLIENT)
	$(MEMCHECK) $(CLIENT)

submit :
	submit  cse101-pt.f19  pa5 $(SOURCE) $(SOURCE2) $(SOURCE3) $(ADT1_HEADER) $(ADT2_HEADER) $(ADT1_SOURCE) $(ADT2_SOURCE) Makefile README
