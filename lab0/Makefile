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

#EXECBIN = name of the executable
EXECBIN        = wordcount
MODULES        = vstring wordList
MKFILE         = Makefile
SOURCES        = $(MODULES:=.cpp) main.cpp
OBJECTS        = $(SOURCES:.cpp=.o)
HEADERS        = $(MODULES:.cpp=.hpp)
ALLSOURCES     = $(HEADERS) $(SOURCES) $(MKFILE)
DEPSFILE       = $(MKFILE).deps
GPPWARN        = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS        = $(GPPWARN) -fdiagnostics-color=never
NOINCL         = ci clean spotless
NEEDINCL       = $(filter $(NOINCL), $(MAKECMDGOALS))
MAKEDEPSCPP    = g++ -std=gnu++2a -MM $(GPPOPTS)
COMPILE        = g++ -c -Wall -Wextra
LINK           = g++ -o
REMOVE         = rm -f
MEMCHECK       = valgrind --leak-check=full -v

all : $(EXECBIN)

$(EXECBIN) : $(OBJECTS)
	$(LINK) $@ $^

%.o : %.cpp
	$(COMPILE) $<

clean :
	$(REMOVE) $(OBJS) 

spotless : clean 
	$(REMOVE) $(EXECBIN)

memcheck : $(EXECBIN)
	$(MEMCHECK) $^

submit : $(ALLSOURCES) README
	submit  cse111-wm.w20  lab0 $^

deps : $(HEADERS) $(SOURCES)
	@ echo "# $(DEPSFILE) created `LC_TIME=C date`" >$(DEPSFILE)
	$^ >>${DEPSFILE}

ifeq ($(NEEDINCL), )
include $(DEPSFILE)
endif

  # someTarget: foo.c bar.c baz.check
  # $@ = someTarget
  # $^ = foo.c bar.c baz.c
  # $< = foo.c