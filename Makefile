#
#	Gist library
#
################

OBJS =		gist.o gist-index.o gist-array.o gist-table.o gist-int.o \
		gist-float.o gist-str.o gist-long.o gist-bool.o gist-ptr.o \
		gist-op1.o gist-op2.o gist-op3.o gist-op4.o gist-op5.o \
		gist-file.o gist-regex.o \
		gist-osdep.o gist-meth1.o gist-meth2.o \
		gist-debug.o \
		sgml.o


CXXFLAGS =	-g -Wall
## CXXFLAGS =	-m32
CXXFLAGS +=     -I/opt/local/include
CXXFLAGS +=     -L/opt/local/lib
## CXXFLAGS =	-g -O3 -Wall
## CXXFLAGS =	-O3 -Wall
OPTIM =

INSDIR =	/usr/local

################

all:		T libgist.a


T:		T.o libgist.a
	$(CXX) -o T $(CXXFLAGS) T.o libgist.a -lgc -ldl


libgist.a:	$(OBJS)
	ar rv libgist.a $?


install:	libgist.a
	cp gist.h $(INSDIR)/include
	cp sgml.h $(INSDIR)/include/gist
	cp libgist.a $(INSDIR)/lib


clean:
	rm -f *.o T libgist.a


T.o:		gist.h sgml.h
gist-*.o:	gist.h gist-internal.h
gist.o:		gist-version.h
sgml.o:		gist.h 
