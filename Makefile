CC=			gcc
CFLAGS=		-std=c99 -g -Wall #-O3
CPPFLAGS=
INCLUDES=
OBJS=		libsais.o libsais64.o sys.o kthread.o io.o rld0.o rle.o rope.o mrope.o \
			fm-index.o sais-ss.o build.o
PROG=		ropebwt3
LIBS=		-lpthread -lz -lm

ifneq ($(asan),)
	CFLAGS+=-fsanitize=address
	LIBS+=-fsanitize=address -ldl
endif

ifneq ($(omp),)
	CPPFLAGS=-DLIBSAIS_OPENMP
	CFLAGS+=-fopenmp
	LIBS+=-fopenmp
endif

.SUFFIXES:.c .o
.PHONY:all clean depend

.c.o:
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

all:$(PROG)

ropebwt3:$(OBJS) main.o
		$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
		rm -fr *.o a.out $(PROG) *~ *.a *.dSYM

depend:
		(LC_ALL=C; export LC_ALL; makedepend -Y -- $(CFLAGS) $(CPPFLAGS) -- *.c)

# DO NOT DELETE

build.o: rb3priv.h fm-index.h rld0.h mrope.h rope.h io.h ketopt.h
fm-index.o: rb3priv.h fm-index.h rld0.h mrope.h rope.h rle.h kthread.h
io.o: rb3priv.h io.h kseq.h
kthread.o: kthread.h
libsais.o: libsais.h
libsais64.o: libsais.h libsais64.h
main.o: rb3priv.h fm-index.h rld0.h mrope.h rope.h ketopt.h
mrope.o: mrope.h rope.h rle.h
rld0.o: rld0.h
rle.o: rle.h
rope.o: rle.h rope.h
sais-ss.o: rb3priv.h libsais.h libsais64.h
sys.o: rb3priv.h
