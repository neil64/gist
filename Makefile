#
#	Gist library
#
################

OBJS =		gist.o gist-op1.o gist-op2.o gist-op3.o


CXXFLAGS =	-g -Wall
## CXXFLAGS =	-g -O -Wall
## CXXFLAGS =	-O -Wall
OPTIM =

################

all:		T libgist.a


T:		T.o libgist.a
	$(CXX) -o T $(CXXFLAGS) T.o libgist.a -lgc -ldl


libgist.a:	$(OBJS)
	ar rv libgist.a $?


$(OBJS):	gist.h gist-internal.h


clean:
	rm -f *.o T libgist.a
