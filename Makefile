CC=			gcc
CFLAGS=		-g -Wall -Wc++-compat -O3
CPPFLAGS=
INCLUDES=
OBJS=		libsais16.o libsais16x64.o kalloc.o kthread.o misc.o io.o rld0.o rle.o rope.o mrope.o \
			dawg.o fm-index.o ssa.o sais-ss.o build.o search.o bwa-sw.o
PROG=		ropebwt3
LIBS=		-lpthread -lz -lm

ifneq ($(asan),)
	CFLAGS+=-fsanitize=address
	LIBS+=-fsanitize=address -ldl
endif

ifneq ($(omp),0)
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
bwa-sw.o: rb3priv.h fm-index.h rld0.h mrope.h rope.h align.h kalloc.h dawg.h
bwa-sw.o: khashl-km.h ksort.h
dawg.o: dawg.h kalloc.h libsais16.h io.h rb3priv.h khashl-km.h
fm-index.o: rb3priv.h fm-index.h rld0.h mrope.h rope.h rle.h kthread.h
fm-index.o: kalloc.h khashl-km.h
io.o: rb3priv.h io.h kseq.h
kalloc.o: kalloc.h
kthread.o: kthread.h
libsais16.o: libsais16.h
libsais16x64.o: libsais16.h libsais16x64.h
main.o: rb3priv.h fm-index.h rld0.h mrope.h rope.h io.h ketopt.h
misc.o: rb3priv.h
mrope.o: mrope.h rope.h rle.h
rld0.o: rld0.h
rle.o: rle.h
rope.o: rle.h rope.h
sais-ss.o: rb3priv.h libsais16.h libsais16x64.h
search.o: fm-index.h rb3priv.h rld0.h mrope.h rope.h align.h io.h ketopt.h
search.o: kthread.h kalloc.h
ssa.o: rb3priv.h fm-index.h rld0.h mrope.h rope.h kalloc.h kthread.h ketopt.h
