#
#	Gist library
#
################

OBJS =		gist.o gist-index.o \
		gist-int.o gist-float.o gist-str.o gist-long.o \
		gist-op1.o gist-op2.o gist-op3.o gist-op4.o


CXXFLAGS =	-g -Wall
## CXXFLAGS =	-g -O3 -Wall
## CXXFLAGS =	-O3 -Wall
OPTIM =

################

all:		T libgist.a


T:		T.o libgist.a
	$(CXX) -o T $(CXXFLAGS) T.o libgist.a -lgc -ldl


libgist.a:	$(OBJS)
	ar rv libgist.a $?


$(OBJS) T.o:	gist.h gist-internal.h


clean:
	rm -f *.o T libgist.a
